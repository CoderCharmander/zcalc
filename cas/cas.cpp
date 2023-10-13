#include "cas.h"
#include <cln/complex.h>
#include <cln/integer_io.h>
#include <sstream>

namespace zcas {
using result = std::expected<cln::cl_R, const char *>;
result parse_sum(const char *exp, size_t &idx);
result evaluate(const std::string &str) {
    size_t idx = 0;
    auto r = parse_sum(str.c_str(), idx);
    if (idx != str.size()) { return std::unexpected("invalid syntax"); }
    return r;
}

// Parse a number. Supports decimal points and returns a rational.
result parse_number(const char *exp, size_t &idx) {
    cln::cl_R a, b = 1;
    std::string number("0");
    for (; exp[idx] >= '0' && exp[idx] <= '9'; ++idx) {
        number.push_back(exp[idx]);
    }
    // we're doing EXACT fractions here!!
    if (exp[idx] == '.') {
        ++idx;
        for (; exp[idx] >= '0' && exp[idx] <= '9'; ++idx) {
            number.push_back(exp[idx]);
            b *= 10;
        }
    }
    try {
        a = cln::cl_R(number.c_str());
    } catch (cln::read_number_bad_syntax_exception &e) { return std::unexpected("invalid number"); }
    if (b != 1) {
        return a / b;
    } else {
        return a;
    }
}

result parse_base(const char *exp, size_t &idx) {
    if (!exp[idx]) { return std::unexpected("unexp EOL"); }
    std::expected<cln::cl_R, const char *> o;
    if (exp[idx] == '(') {
        auto r = parse_sum(exp, ++idx);
        if (!r) { return r; }
        if (exp[idx] != ')') { return std::unexpected("() mismatch"); }
        ++idx;
        return r;
    } else if (exp[idx] == '.' || (exp[idx] >= '0' && exp[idx] <= '9')) {
        return parse_number(exp, idx);
    } else {
        return std::unexpected("invalid syntax");
    }
}

result parse_exp(const char *exp, size_t &idx) {
    auto o = parse_base(exp, idx);
    if (!o) { return o; }
    while (exp[idx] == '^') {
        auto r = parse_base(exp, ++idx);
        if (!r) { return r; }
        // let's just assume that we aren't getting complex numbers
        *o = cln::realpart(cln::expt(*o, *r));
    }
    auto r = parse_exp(exp, idx);
    if (r) {
        return *o * *r;
    } else {
        return o;
    }
}

result parse_neg(const char *exp, size_t &idx) {
    bool neg = false;
    while (exp[idx] == '-' || exp[idx] == '+') {
        if (exp[idx] == '-') { neg = !neg; }
        ++idx;
    }
    auto o = parse_exp(exp, idx);
    if (!o) { return o; }
    if (neg) { return -*o; }
    return o;
}

result parse_prod(const char *exp, size_t &idx) {
    auto o = parse_neg(exp, idx);
    if (!o) { return o; }
    while (exp[idx] == '*' || exp[idx] == '/') {
        if (exp[idx] == '*') {
            auto r = parse_neg(exp, ++idx);
            if (!r) { return r; }
            *o *= *r;
        } else {
            auto r = parse_neg(exp, ++idx);
            if (!r) { return r; }
            *o /= *r;
        }
    }
    return o;
}

// Parse a sum expression (e.g. the whole expression itself)
result parse_sum(const char *exp, size_t &idx) {
    auto o = parse_prod(exp, idx);
    if (!o) { return o; }
    while (exp[idx] == '+' || exp[idx] == '-') {
        if (exp[idx] == '+') {
            auto r = parse_prod(exp, ++idx);
            if (!r) { return r; }
            *o += *r;
        } else {
            auto r = parse_prod(exp, ++idx);
            if (!r) { return r; }
            *o -= *r;
        }
    }
    return o;
}
} // namespace zcas
