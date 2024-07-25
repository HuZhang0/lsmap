#ifndef XMG_RS_HPP
#define XMG_RS_HPP
#include <iterator>
#include <mockturtle/mockturtle.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"
#include "core/misc.hpp"
using namespace std;
using namespace mockturtle;
using namespace lsmap;

namespace alice {
class xmg_rs_command : public command {
 public:
  explicit xmg_rs_command(const environment::ptr &env)
      : command(env, "xmg resub") {
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    begin = clock();
    if (!store<xmg_network>().empty()) {
      xmg_network xmg = store<xmg_network>().current();
      mockturtle::resubstitution_params rs_ps;
      mockturtle::resubstitution_stats rs_st;
      xmg_resubstitution(xmg, rs_ps, &rs_st);

      store<xmg_network>().extend();
      store<xmg_network>().current() = cleanup_dangling(xmg);
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
};
ALICE_ADD_COMMAND(xmg_rs, "Optimization")
}  // namespace alice
#endif
