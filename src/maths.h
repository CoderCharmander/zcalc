#include <cln/real.h>
#include <string>
#include <expected>
namespace math {
    std::expected<cln::cl_R, const char*> evaluate(const std::string& str);

}