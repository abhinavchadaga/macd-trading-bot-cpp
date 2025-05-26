#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>

// test cmake setup

int
main ()
{
  boost::asio::io_context ioc;
  boost::system::error_code ec;
  boost::beast::flat_buffer buf;
  return 0;
}