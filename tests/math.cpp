#include <string>
#include <iostream>
#include "maths.h"
#include <gtest/gtest.h>
#include <cln/real_io.h>

TEST(ZCalcTest, OrderOfOps) {
    EXPECT_EQ(math::evaluate("1+2*3"), 7);
    EXPECT_EQ(math::evaluate("-10^2"), -100);
    EXPECT_EQ(math::evaluate("(-10)^2"), 100);
    EXPECT_EQ(math::evaluate("(3)(3)3"), 27);
    EXPECT_EQ(math::evaluate("2(-10)^2"), 200);
    EXPECT_TRUE(math::evaluate("10^(-1)") == cln::cl_RA("1/10"));
}
TEST(ZCalcTest, Floats) {
    // float magic!
    auto a = math::evaluate("0.1+0.2");
    EXPECT_TRUE(a.has_value());
    EXPECT_TRUE(a == math::evaluate("0.3"));
    EXPECT_TRUE(a == cln::cl_RA("3/10"));
}
TEST(ZCalcTest, Errors) {
    EXPECT_FALSE(math::evaluate("((").has_value());
    EXPECT_FALSE(math::evaluate("5+()").has_value());
    EXPECT_FALSE(math::evaluate("***").has_value());
    EXPECT_FALSE(math::evaluate("7+5()").has_value());
    EXPECT_FALSE(math::evaluate("7+(5)?").has_value());
}

TEST(ZCalcTest, Basic) {
    EXPECT_EQ(math::evaluate("1+1"), 2);
    EXPECT_EQ(math::evaluate("1-1"), 0);
    EXPECT_EQ(math::evaluate("1--1"), 2);
    EXPECT_EQ(math::evaluate("6/3"), 2);
    EXPECT_EQ(math::evaluate("3*3"), 9);
}
