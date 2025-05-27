add_library(web_socket_session INTERFACE)

target_include_directories(web_socket_session
    INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(web_socket_session
    INTERFACE
    Boost::asio
    Boost::beast
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_compile_features(web_socket_session INTERFACE cxx_std_20)