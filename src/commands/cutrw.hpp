#ifndef CUTRW_HPP
#define CUTRW_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"

using namespace std;
using namespace percy;

namespace alice {
class cutrw_command : public command {
 public:
  explicit cutrw_command(const environment::ptr &env)
      : command(env, "cut_rewriting") {
    add_flag("--aig, -a", "rewrite for aig");
    add_flag("--xmg, -x", "rewrite for xmg");
    add_flag("--exact_xmg, -m", "rewrite with exact synthesis for xmg");
    add_flag("--exact_aig, -n", "rewrite with exact synthesis for aig");
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    cut_rewriting_params ps;
    begin = clock();
    if (is_set("aig")) {
      if (!store<aig_network>().empty()) {
        auto aig = store<aig_network>().current();
        xag_npn_resynthesis<aig_network> resyn;
        ps.cut_enumeration_ps.cut_size = 4;
        cut_rewriting_with_compatibility_graph(aig, resyn, ps);
        store<aig_network>().extend();
        store<aig_network>().current() = cleanup_dangling(aig);
      } else {
        std::cerr << "There is not an AIG network stored.\n";
      }
    } else if (is_set("xmg")) {
      if (!store<xmg_network>().empty()) {
        auto xmg = store<xmg_network>().current();
        xmg_npn_resynthesis resyn;
        ps.cut_enumeration_ps.cut_size = 4;
        cut_rewriting_with_compatibility_graph(xmg, resyn, ps);
        store<xmg_network>().extend();
        store<xmg_network>().current() = cleanup_dangling(xmg);
      } else {
        std::cerr << "There is not an XMG network stored.\n";
      }
    } else if (is_set("exact_xmg")) {
      if (!store<xmg_network>().empty()) {
        mockturtle::xmg_network xmg =
            store<mockturtle::xmg_network>().current();
        exact_xmg_resynthesis<xmg_network> resyn;
        ps.cut_enumeration_ps.cut_size = 4;
        xmg = cut_rewriting(xmg, resyn, ps);
        store<xmg_network>().extend();
        store<xmg_network>().current() = cleanup_dangling(xmg);
      }
    } else if (is_set("exact_aig")) {
      if (!store<aig_network>().empty()) {
        mockturtle::aig_network aig =
            store<mockturtle::aig_network>().current();
        exact_aig_resynthesis<aig_network> resyn;
        ps.cut_enumeration_ps.cut_size = 4;
        aig = cut_rewriting(aig, resyn, ps);
        store<aig_network>().extend();
        store<aig_network>().current() = cleanup_dangling(aig);
      }
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
ALICE_ADD_COMMAND(cutrw, "Optimization")
}  // namespace alice
#endif
