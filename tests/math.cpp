#include <string>
#include <iostream>
#include "maths.h"
#include <gtest/gtest.h>

TEST(ZCalcTest, Basic) {
    auto res = math::evaluate("1+1");
    EXPECT_EQ(res, 2);
    EXPECT_EQ(math::evaluate("1+2*3"), 7);
    EXPECT_EQ(math::evaluate("-10^2"), -100);
    // float magic!
    EXPECT_TRUE(math::evaluate("0.1+0.2") == cln::cl_LF("0.3"));
    EXPECT_TRUE(math::evaluate("10^(-1)") == cln::cl_RA("1/10"));
    EXPECT_FALSE(math::evaluate("((").has_value());
    EXPECT_FALSE(math::evaluate("5+()").has_value());
    EXPECT_FALSE(math::evaluate("***").has_value());
    EXPECT_FALSE(math::evaluate("7+5()").has_value());
}