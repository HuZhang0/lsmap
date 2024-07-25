#ifndef XMGRW_HPP
#define XMGRW_HPP
#include <iterator>
#include <mockturtle/mockturtle.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"
#include "core/misc.hpp"
using namespace std;
using namespace mockturtle;
using namespace lsmap;

namespace alice {
class xmgrw_command : public command {
 public:
  explicit xmgrw_command(const environment::ptr &env)
      : command(env, "xmg algebraic depth rewrite") {
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    begin = clock();
    if (!store<xmg_network>().empty()) {
      xmg_network xmg = store<xmg_network>().current();
      xmg_algebraic_depth_rewriting_params rw_ps;
      depth_view<xmg_network> depth_xmg{xmg};
      xmg_algebraic_depth_rewriting(depth_xmg, rw_ps);
      xmg = cleanup_dangling(xmg);
      store<xmg_network>().extend();
      store<xmg_network>().current() = xmg;
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
};
ALICE_ADD_COMMAND(xmgrw, "Optimization")
}  // namespace alice
#endif
