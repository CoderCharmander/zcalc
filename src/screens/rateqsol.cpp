#include "rateqsol.h"
#include "ratio_eq.h"

namespace screens::rateqsol {

void solvinator(uint8_t eq) {}

const menu::menu_item eq_menuitems[] = {{"R = U/I", false, solvinator, 0}, {"F = k * Q1*Q2 / r^2", false, solvinator, 1}};

const zcas::ratio_eq equations[] = {{1, {{cln::cl_symbol{"U"}, 1}, {cln::cl_symbol{"I"}, -1}, {cln::cl_symbol{"R"}, -1}}},
                              {"9000000000", {{cln::cl_symbol{"Q1"}, 1}, {cln::cl_symbol{"Q2"}, 1}, {cln::cl_symbol{"r"}, -2}, {cln::cl_symbol{"F"}, -1}}}};

menu::menu eqs_menu(eq_menuitems, sizeof(eq_menuitems) / sizeof(eq_menuitems[0]));

} // namespace screens::rateqsol