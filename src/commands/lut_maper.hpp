#ifndef LUT_MAPER_HPP
#define LUT_MAPER_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>

#include "../store.hpp"

using namespace std;
using namespace percy;
using namespace mockturtle;
using namespace lsmap;

namespace alice {
class lut_mapper_command : public command {
 public:
  explicit lut_mapper_command(const environment::ptr& env)
      : command(env, "LUT Mapper from mockturtle") {
    add_option("cut_size, -k", cut_size,
               "set the cut size from 2 to 8, default = 6");
    add_option("cut_limit, -l", cut_limit,
               "set the lut limit from 2 to 49, default = 49");
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    mockturtle::lut_map_params ps;
    begin = clock();
    if (!store<aig_network>().empty()) {
      auto aig = store<aig_network>().current();
      mockturtle::mapping_view<mockturtle::aig_network, true> mapped{aig};
      ps.cut_enumeration_ps.cut_size = cut_size;
      ps.cut_enumeration_ps.cut_limit = cut_limit;
      const auto klut =
          lut_map<mapping_view<aig_network, true>, true>(mapped, ps);
      store<klut_network>().extend();
      store<klut_network>().current() = klut;
    } else {
      std::cerr << "There is not an AIG network stored.\n";
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
  int cut_size = 6;
  int cut_limit = 49;
};
ALICE_ADD_COMMAND(lut_mapper, "Mapping")
}  // namespace alice
#endif
