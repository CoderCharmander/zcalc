#include "ratio_eq.h"
#include <cln/real_ring.h>

namespace zcas {

bytecode ratio_eq::build(const std::vector<cln::cl_R>& definitions, size_t input, size_t output) {
    std::vector<cln::cl_R> consts(definitions);

}
cln::cl_R ratio_eq::calculate(const std::vector<cln::cl_R> &definitions, size_t output) {
    cln::cl_R out = 1;
    for (int i = 0; i < monomial.size(); ++i) {
        auto [_, exp] = monomial[i];
        if (i == output) break;
        cln::cl_R cl_exp{-exp};
        cl_exp /= monomial[output].second;
        out *= cln::expt(definitions[i], cl_exp);
    }
    out *= cln::expt(constant, 1 / cln::cl_R(-monomial[output].second));
    return out;
}

} // namespace cas