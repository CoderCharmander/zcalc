#include "maths.h"
#include <cln/complex.h>
#include <cln/input.h>
#include <cln/io.h>
#include <cln/real_io.h>

namespace math {
using result = std::expected<cln::cl_R, const char *>;
result parse_sum(const char *exp, size_t &idx);
result evaluate(const std::string &str) {
  size_t idx = 0;
  auto r = parse_sum(str.c_str(), idx);
  if (idx != str.size()) {
    return std::unexpected("invalid syntax");
  }
  return r;
}

result parse_base(const char *exp, size_t &idx) {
  if (!exp[idx]) {
    return std::unexpected("unexp EOL");
  }
  if (exp[idx] == '(') {
    auto r = parse_sum(exp, ++idx);
    if (!r) {
      return r;
    }
    if (exp[idx] != ')') {
      return std::unexpected("() mismatch");
    }
    ++idx;
    return r;
  } else if (exp[idx] == '.' || (exp[idx] >= '0' && exp[idx] <= '9')) {
    const char *begin = &exp[idx];
    const char *end = begin;
    while (*end && (*end == '.' || (*end >= '0' && *end <= '9'))) {
      ++end, ++idx;
    }
    try {
      const char *eop;
      cln::cl_read_flags cl_R_read_flags = {
          cln::syntax_real,
          cln::lsyntax_all,
          10,
          {cln::float_format_lfloat_min, cln::float_format_lfloat_min, true}};
      cln::cl_R r = cln::read_real(cl_R_read_flags, begin, end, NULL);
      return r;
    } catch (cln::read_number_bad_syntax_exception e) {
      return std::unexpected("invalid number");
    }
  } else {
    return std::unexpected("invalid syntax");
  }
}

result parse_exp(const char *exp, size_t &idx) {
  auto o = parse_base(exp, idx);
  if (!o) {
    return o;
  }
  while (exp[idx] && exp[idx] == '^') {
    auto r = parse_base(exp, ++idx);
    if (!r) {
      return r;
    }
    // let's just assume that we aren't getting complex numbers
    *o = cln::realpart(cln::expt(*o, *r));
  }
  return o;
}

result parse_neg(const char *exp, size_t &idx) {
  bool neg = false;
  while (exp[idx] && (exp[idx] == '-' || exp[idx] == '+')) {
    if (exp[idx] == '-') {
      neg = !neg;
    }
    ++idx;
  }
  auto o = parse_exp(exp, idx);
  if (!o) {
    return o;
  }
  if (neg) {
    return -*o;
  }
  return o;
}

result parse_prod(const char *exp, size_t &idx) {
  auto o = parse_neg(exp, idx);
  if (!o) {
    return o;
  }
  while (exp[idx] && (exp[idx] == '*' || exp[idx] == '/')) {
    if (exp[idx] == '*') {
      auto r = parse_neg(exp, ++idx);
      if (!r) {
        return r;
      }
      *o *= *r;
    } else {
      auto r = parse_neg(exp, ++idx);
      if (!r) {
        return r;
      }
      *o /= *r;
    }
  }
  return o;
}

result parse_sum(const char *exp, size_t &idx) {
  auto o = parse_prod(exp, idx);
  if (!o) {
    return o;
  }
  while (exp[idx] && (exp[idx] == '+' || exp[idx] == '-')) {
    if (exp[idx] == '+') {
      auto r = parse_prod(exp, ++idx);
      if (!r) {
        return r;
      }
      *o += *r;
    } else {
      auto r = parse_prod(exp, ++idx);
      if (!r) {
        return r;
      }
      *o -= *r;
    }
  }
  return o;
}
} // namespace math