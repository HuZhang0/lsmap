#ifndef XMG_B_HPP
#define XMG_B_HPP
#include <iterator>
#include <mockturtle/mockturtle.hpp>

#include "../store.hpp"
#include "core/misc.hpp"
using namespace std;
using namespace mockturtle;
using namespace lsmap;

namespace alice {
class xmg_b_command : public command {
 public:
  explicit xmg_b_command(const environment::ptr &env)
      : command(env, "xmg cut rewrite") {
    add_flag("--verbose, -v", "print the information");
    add_flag("--sop, -s", "sop balance");
  }

  balancing_params ps;
  lut_map_params sop_ps;
  lut_map_stats sop_st;

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    begin = clock();
    if (is_set("sop")) {
      if (!store<xmg_network>().empty()) {
        sop_ps.cut_enumeration_ps.cut_size = 6u;
        sop_ps.cut_enumeration_ps.cut_limit = 8u;
        xmg_network xmg = store<xmg_network>().current();
        xmg = sop_balancing(xmg, sop_ps, &sop_st);
        store<xmg_network>().extend();
        store<xmg_network>().current() = cleanup_dangling(xmg);
      }
    } else {
      if (!store<xmg_network>().empty()) {
        ps.cut_enumeration_ps.cut_size = 6u;
        ps.cut_enumeration_ps.cut_limit = 8u;
        sop_rebalancing<xmg_network> balance_fn;
        xmg_network xmg = store<xmg_network>().current();
        xmg = balancing(xmg, {balance_fn}, ps);
        store<xmg_network>().extend();
        store<xmg_network>().current() = cleanup_dangling(xmg);
      }
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
};
ALICE_ADD_COMMAND(xmg_b, "Optimization")
}  // namespace alice
#endif
