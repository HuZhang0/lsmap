#ifndef XMG_RW_HPP
#define XMG_RW_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"

using namespace std;
using namespace percy;

namespace alice {
class xmg_rw_command : public command {
 public:
  explicit xmg_rw_command(const environment::ptr &env)
      : command(env, "xmg_cut_rewriting") {
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    begin = clock();
    if (!store<xmg_network>().empty()) {
      auto xmg = store<xmg_network>().current();
      cut_rewriting_params ps;
      xmg_npn_resynthesis resyn;
      ps.cut_enumeration_ps.cut_size = 4;
      cut_rewriting_with_compatibility_graph(xmg, resyn, ps);
      store<xmg_network>().extend();
      store<xmg_network>().current() = cleanup_dangling(xmg);
    } else {
      std::cerr << "There is not an XMG network stored.\n";
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
  int lut_size = 4;
  int cut_size = 4;
};
ALICE_ADD_COMMAND(xmg_rw, "Optimization")
}  // namespace alice
#endif
