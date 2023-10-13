#include <cln/real.h>
#include <string>
#include <expected>

namespace zcas {

std::expected<cln::cl_R, const char*> evaluate(const std::string& str);

}