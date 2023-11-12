#include <string>
#include <iostream>
#include "cas.h"
#include <gtest/gtest.h>
#include <cln/real_io.h>
#include <ratio_eq.h>

TEST(ZCalcTest, OrderOfOps) {
    EXPECT_EQ(zcas::evaluate("1+2*3"), 7);
    EXPECT_EQ(zcas::evaluate("-10^2"), -100);
    EXPECT_EQ(zcas::evaluate("(-10)^2"), 100);
    EXPECT_EQ(zcas::evaluate("(3)(3)3"), 27);
    EXPECT_EQ(zcas::evaluate("2(-10)^2"), 200);
    EXPECT_TRUE(zcas::evaluate("10^(-1)") == cln::cl_RA("1/10"));
}
TEST(ZCalcTest, Floats) {
    // float magic!
    auto a = zcas::evaluate("0.1+0.2");
    EXPECT_TRUE(a.has_value());
    EXPECT_TRUE(a == zcas::evaluate("0.3"));
    EXPECT_TRUE(a == cln::cl_RA("3/10"));
}
TEST(ZCalcTest, Errors) {
    EXPECT_FALSE(zcas::evaluate("((").has_value());
    EXPECT_FALSE(zcas::evaluate("5+()").has_value());
    EXPECT_FALSE(zcas::evaluate("***").has_value());
    EXPECT_FALSE(zcas::evaluate("7+5()").has_value());
    EXPECT_FALSE(zcas::evaluate("7+(5)?").has_value());
}

TEST(ZCalcTest, Basic) {
    EXPECT_EQ(zcas::evaluate("1+1"), 2);
    EXPECT_EQ(zcas::evaluate("1-1"), 0);
    EXPECT_EQ(zcas::evaluate("1--1"), 2);
    EXPECT_EQ(zcas::evaluate("6/3"), 2);
    EXPECT_EQ(zcas::evaluate("3*3"), 9);
}

TEST(ZCalcTest, RationalEquationSanity) {
    zcas::ratio_eq thing;
    thing.monomial = {{cln::cl_symbol{"x"}, 1}, {cln::cl_symbol{"y"}, -1}};
    thing.constant = 1;
    EXPECT_EQ(thing.calculate({2, 1}, 1), 2);
    EXPECT_EQ(thing.calculate({2, 1}, 0), 1);
}

TEST(ZCalcTest, RationalEquationStress) {
    zcas::ratio_eq thing;
    thing.monomial = {{cln::cl_symbol{"x"}, 2}, {cln::cl_symbol{"y"}, -1}};
    thing.constant = 1;
    EXPECT_EQ(thing.calculate({2, 0}, 1), 4);
    EXPECT_EQ(thing.calculate({0, 4}, 0), 2);
}
