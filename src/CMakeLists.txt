add_library(web_socket_session
    web_socket_session.cpp
)

target_include_directories(web_socket_session
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(web_socket_session
    PUBLIC
    Boost::asio
    Boost::beast
    Boost::system
    OpenSSL::SSL
    OpenSSL::Crypto
)

target_compile_features(web_socket_session PUBLIC cxx_std_20)
