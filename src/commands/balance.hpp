#ifndef BALANCE_HPP
#define BALANCE_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>
#include <mockturtle/traits.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"

using namespace std;
using namespace mockturtle;

namespace alice {
class balance_command : public command {
 public:
  explicit balance_command(const environment::ptr &env)
      : command(env, "balance") {
    add_option("cut_limit, -l", cut_limit,
               "set the cut size from 2 to 8, default = 8");
    add_option("cut_size, -k", cut_size,
               "set the lut size from 2 to 8, default = 6");
    add_flag("--aig, -a", "aig balance");
    add_flag("--min, -m", "minimize_levels, default = yes");
    add_flag("--sop, -s", "sop balance");
    add_flag("--esop, -e", "esop balance");
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    aig_balancing_params ps;
    begin = clock();
    if (is_set("aig")) {
      if (!store<aig_network>().empty()) {
        auto aig = store<aig_network>().current();
        aig_balance(aig);
        store<aig_network>().extend();
        store<aig_network>().current() = cleanup_dangling(aig);
      } else {
        std::cerr << "There is not an AIG network stored.\n";
      }
      if (is_set("min")) {
        if (!store<aig_network>().empty()) {
          auto aig = store<aig_network>().current();
          ps.minimize_levels = false;
          aig_balance(aig);
          store<aig_network>().extend();
          store<aig_network>().current() = cleanup_dangling(aig);
        } else {
          std::cerr << "There is not an AIG network stored.\n";
        }
      }
      if (is_set("sop")) {
        if (!store<aig_network>().empty()) {
          auto aig = store<aig_network>().current();
          lut_map_params sop_ps;
          lut_map_stats sop_st;
          sop_ps.cut_enumeration_ps.cut_size = cut_size;
          sop_ps.cut_enumeration_ps.cut_limit = cut_limit;

          aig_network aig1 = sop_balancing(aig, sop_ps, &sop_st);
          store<aig_network>().extend();
          store<aig_network>().current() = cleanup_dangling(aig1);
        } else {
          std::cerr << "There is not an AIG network stored.\n";
        }
      }
      if (is_set("esop")) {
        if (!store<aig_network>().empty()) {
          auto aig = store<aig_network>().current();
          lut_map_params esop_ps;
          lut_map_stats esop_st;
          esop_ps.cut_enumeration_ps.cut_size = cut_size;
          esop_ps.cut_enumeration_ps.cut_limit = cut_limit;
          aig_network aig1 = esop_balancing(aig, esop_ps, &esop_st);
          store<aig_network>().extend();
          store<aig_network>().current() = cleanup_dangling(aig1);
        } else {
          std::cerr << "There is not an AIG network stored.\n";
        }
      }
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
  int cut_size = 6;
  int cut_limit = 8;
};
ALICE_ADD_COMMAND(balance, "Optimization")
}  // namespace alice
#endif
