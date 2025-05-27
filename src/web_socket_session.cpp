#include "web_socket_session.hpp"

std::shared_ptr<web_socket_session>
web_socket_session::create (net::io_context &ioc,
        const web_socket_session_config &config,
        frame_handler on_frame)
{
  auto session = std::make_shared<web_socket_session> (ioc, config);
  session->_frame_handler = std::move (on_frame);
  return session;
}

web_socket_session::web_socket_session (net::io_context &ioc, web_socket_session_config cfg)
  : _config{ std::move (cfg) },
    _resolver{ net::make_strand (ioc) }
    , _ws{ net::make_strand (ioc), _config.ssl_ctxt }
    , _ping_timer{ net::make_strand (ioc) }
    , _connected{ false }
    , _should_reconnect{ true }
{
}

void
web_socket_session::start ()
{
  if (_connected)
    return;

  resolve ();
}

void
web_socket_session::stop ()
{
  _should_reconnect = false;
  _connected = false;
  _ping_timer.cancel ();

  if (_ws.is_open ())
    {
      _ws.async_close (websocket::close_code::normal,
                       [self = shared_from_this (), this](
                       const beast::error_code &ec)
                       {
                         if (ec)
                           {
                             fail (ec, "close");
                           }
                       });
    }
}

void
web_socket_session::send (const std::string_view text)
{
  _write_queue.emplace (text);
  if (_connected && _write_queue.size () == 1)
    {
      do_write ();
    }
}

void
web_socket_session::fail (const beast::error_code &ec, char const *what)
{
  std::cerr << what << ": " << ec.message () << "\n";
  _connected = false;
  if (_should_reconnect)
    {
      reconnect ();
    }
}

void
web_socket_session::resolve ()
{
  _resolver.async_resolve (
      _config.host,
      _config.port,
      [self = shared_from_this ()](const beast::error_code &ec,
                                   const tcp::resolver::results_type &
                                   results)
      {
        self->on_resolve (ec, results);
      });
}

void
web_socket_session::on_resolve (const beast::error_code &ec,
            const tcp::resolver::results_type &results)
{
  if (ec)
    {
      return fail (ec, "resolve");
    }

  beast::get_lowest_layer (_ws).expires_after (std::chrono::seconds (30));
  beast::get_lowest_layer (_ws).async_connect (
      results,
      [self = shared_from_this ()](const beast::error_code &ec,
                                   const
                                   tcp::resolver::results_type::endpoint_type
                                   &ep)
      {
        self->on_connect (ec, ep);
      });
}

void
web_socket_session::on_connect (beast::error_code error_code,
            const tcp::resolver::results_type::endpoint_type &endpoint_type)
{
  if (error_code)
    {
      return fail (error_code, "connect");
    }

  beast::get_lowest_layer (_ws).expires_after (std::chrono::seconds (30));

  if (!SSL_set_tlsext_host_name (_ws.next_layer ().native_handle (),
                                 _config.host.c_str ()))
    {
      error_code = beast::error_code (static_cast<int> (::ERR_get_error ()),
                                      net::error::get_ssl_category ());
      return fail (error_code, "connect");
    }

  _host_port = _config.host + ':' + std::to_string (endpoint_type.port ());

  _ws.next_layer ().async_handshake (
      ssl::stream_base::client,
      [self = shared_from_this ()](const beast::error_code &ec)
      {
        self->on_ssl_handshake (ec);
      });
}

void
web_socket_session::on_ssl_handshake (const beast::error_code &error_code)
{
  if (error_code)
    {
      return fail (error_code, "ssl_handshake");
    }

  beast::get_lowest_layer (_ws).expires_never ();
  _ws.set_option (
      websocket::stream_base::timeout::suggested (beast::role_type::client));
  _ws.set_option (websocket::stream_base::decorator (
      [](websocket::request_type &req)
      {
        req.set (http::field::user_agent,
                 std::string (BOOST_BEAST_VERSION_STRING) +
                 " macd-trading-bot");
      }));

  _ws.async_handshake (_host_port, _config.endpoint,
                       [self = shared_from_this ()](
                       const beast::error_code &ec)
                       {
                         self->on_handshake (ec);
                       });
}

void
web_socket_session::on_handshake (const beast::error_code &ec)
{
  if (ec)
    {
      return fail (ec, "handshake");
    }

  _connected = true;

  if (!_config.auth_msg.empty ())
    {
      send (_config.auth_msg);
    }
  if (!_config.sub_msg.empty ())
    {
      send (_config.sub_msg);
    }

  // Flush any queued messages
  if (!_write_queue.empty ())
    {
      do_write ();
    }

  arm_heartbeat ();
  do_read ();
}

void
web_socket_session::do_read ()
{
  _ws.async_read (
      _buffer,
      [self = shared_from_this ()](const beast::error_code &ec,
                                   const std::size_t bytes_transferred)
      {
        self->on_read (ec, bytes_transferred);
      });
}

void
web_socket_session::on_read (const beast::error_code &ec, std::size_t bytes_transferred)
{
  boost::ignore_unused (bytes_transferred);

  if (ec)
    {
      return fail (ec, "read");
    }

  if (_frame_handler)
    {
      const auto data = beast::buffers_to_string (_buffer.data ());
      std::cout << "Received message: " << data << std::endl;
      _frame_handler (data);
    }

  _buffer.clear ();
  do_read ();
}

void
web_socket_session::do_write ()
{
  if (_write_queue.empty () || !_connected)
    return;

  _ws.async_write (
      net::buffer (_write_queue.front ()),
      [self = shared_from_this ()](const beast::error_code &ec,
                                   std::size_t bytes_transferred)
      {
        self->on_write (ec, bytes_transferred);
      });
}

void
web_socket_session::on_write (const beast::error_code &ec, std::size_t bytes_transferred)
{
  boost::ignore_unused (bytes_transferred);

  if (ec)
    {
      return fail (ec, "write");
    }

  _write_queue.pop ();
  if (!_write_queue.empty ())
    {
      do_write ();
    }
}

void
web_socket_session::arm_heartbeat ()
{
  _ping_timer.expires_after (std::chrono::seconds (30));
  _ping_timer.async_wait (
      [self = shared_from_this ()](const beast::error_code &ec)
      {
        self->on_ping_timer (ec);
      });
}

void
web_socket_session::on_ping_timer (const beast::error_code &ec)
{
  if (ec || !_connected)
    return;

  _ws.async_ping ({},
                  [self = shared_from_this ()](const beast::error_code &ec)
                  {
                    if (ec)
                      {
                        self->fail (ec, "ping");
                        return;
                      }
                    self->arm_heartbeat ();
                  });
}

void
web_socket_session::reconnect ()
{
  if (!_should_reconnect)
    return;

  std::this_thread::sleep_for (std::chrono::seconds (5));

  if (_should_reconnect)
    {
      start ();
    }
}