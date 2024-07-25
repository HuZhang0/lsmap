#ifndef LUT_MAPPING_HPP
#define LUT_MAPPING_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"

using namespace std;
using namespace percy;
using namespace lsmap;

namespace alice {
class lut_mapping_command : public command {
 public:
  explicit lut_mapping_command(const environment::ptr& env)
      : command(env, "LUT Mapping from mockturtle") {
    add_option("cut_limit, -l", cut_limit,
               "set the cut size from 2 to 8, default = 8");
    add_option("lut_size, -k", lut_size,
               "set the lut size from 2 to 8, default = 4");
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    mockturtle::lut_mapping_params ps;
    begin = clock();
    if (!store<aig_network>().empty()) {
      auto aig = store<aig_network>().current();
      mockturtle::mapping_view<mockturtle::aig_network, true> mapped{aig};
      ps.cut_enumeration_ps.cut_size = lut_size;
      ps.cut_enumeration_ps.cut_limit = cut_limit;
      mockturtle::lut_mapping<
          mockturtle::mapping_view<mockturtle::aig_network, true>, true>(mapped,
                                                                         ps);
      const auto klut = *collapse_mapped_network<klut_network>(mapped);
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
  int lut_size = 6;
  int cut_limit = 8;
};
ALICE_ADD_COMMAND(lut_mapping, "Mapping")
}  // namespace alice
#endif
