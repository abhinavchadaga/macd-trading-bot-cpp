#include "async_rest_client/utils.hpp"
#include "my_logger.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

namespace async_rest_client
{

class AsyncRestClientUtilsTest : public ::testing::Test
{
};

TEST_F(AsyncRestClientUtilsTest, MakeHttpHttpsUrl_ValidHttpUrl)
{
    const std::string http_url = "http://example.com";
    const auto        result   = make_http_https_url(http_url);

    EXPECT_EQ(result.scheme(), "http");
    EXPECT_EQ(result.host(), "example.com");
    EXPECT_EQ(result.port(), "80");
}

TEST_F(AsyncRestClientUtilsTest, MakeHttpHttpsUrl_ValidHttpsUrl)
{
    const std::string https_url = "https://example.com";
    const auto        result    = make_http_https_url(https_url);

    EXPECT_EQ(result.scheme(), "https");
    EXPECT_EQ(result.host(), "example.com");
    EXPECT_EQ(result.port(), "443");
}

TEST_F(AsyncRestClientUtilsTest, MakeHttpHttpsUrl_HttpUrlWithExistingPort)
{
    const std::string http_url_with_port = "http://example.com:8080";
    const auto        result             = make_http_https_url(http_url_with_port);

    EXPECT_EQ(result.scheme(), "http");
    EXPECT_EQ(result.host(), "example.com");
    EXPECT_EQ(result.port(), "8080");
}

TEST_F(AsyncRestClientUtilsTest, MakeHttpHttpsUrl_HttpsUrlWithExistingPort)
{
    const std::string https_url_with_port = "https://example.com:8443";
    const auto        result              = make_http_https_url(https_url_with_port);

    EXPECT_EQ(result.scheme(), "https");
    EXPECT_EQ(result.host(), "example.com");
    EXPECT_EQ(result.port(), "8443");
}

TEST_F(AsyncRestClientUtilsTest, MakeHttpHttpsUrl_UrlWithoutSchemeThrows)
{
    const std::string url_without_scheme = "example.com";

    EXPECT_THROW(
        {
            try
            {
                make_http_https_url(url_without_scheme);
            }
            catch (const boost::system::system_error& e)
            {
                EXPECT_EQ(e.code(), std::errc::protocol_not_supported);
                throw;
            }
        },
        boost::system::system_error);
}

TEST_F(AsyncRestClientUtilsTest, MakeHttpHttpsUrl_UnsupportedSchemeThrows)
{
    const std::string ftp_url = "ftp://example.com";

    EXPECT_THROW(
        {
            try
            {
                make_http_https_url(ftp_url);
            }
            catch (const boost::system::system_error& e)
            {
                EXPECT_EQ(e.code(), std::errc::protocol_not_supported);
                throw;
            }
        },
        boost::system::system_error);
}

TEST_F(AsyncRestClientUtilsTest, MakeHttpHttpsUrl_HttpUrlWithPath)
{
    const std::string http_url_with_path = "http://example.com/api/v1";
    const auto        result             = make_http_https_url(http_url_with_path);

    EXPECT_EQ(result.scheme(), "http");
    EXPECT_EQ(result.host(), "example.com");
    EXPECT_EQ(result.port(), "80");
    EXPECT_EQ(result.path(), "/api/v1");
}

TEST_F(AsyncRestClientUtilsTest, MakeHttpHttpsUrl_HttpsUrlWithPath)
{
    const std::string https_url_with_path = "https://example.com/api/v1";
    const auto        result              = make_http_https_url(https_url_with_path);

    EXPECT_EQ(result.scheme(), "https");
    EXPECT_EQ(result.host(), "example.com");
    EXPECT_EQ(result.port(), "443");
    EXPECT_EQ(result.path(), "/api/v1");
}

} // namespace async_rest_client
