#pragma once

#include "concepts.hpp"

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

namespace async_rest_client
{

namespace beast = boost::beast;

template<SupportedStreamType StreamType>
struct connection_context
{
    StreamType&         stream;
    beast::flat_buffer& buffer;
};

using https_connection_context = connection_context<beast::ssl_stream<beast::tcp_stream>>;
using http_connection_context  = connection_context<beast::tcp_stream>;

} // namespace async_rest_client
