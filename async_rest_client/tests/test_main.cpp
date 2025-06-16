#include "my_logger.hpp"

#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    init_logger("async_rest_client_tests");

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
