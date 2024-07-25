#ifndef REWRITE_HPP
#define REWRITE_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"

using namespace std;
using namespace percy;
using namespace mockturtle;

namespace alice {
class rw_command : public command {
 public:
  explicit rw_command(const environment::ptr &env) : command(env, "rewriting") {
    add_flag("--sop, -s", "rewrite with sop");
    add_flag("--npn, -n", "rewrite with npn");
    add_flag("--dsd, -d", "rewrite with dsd");
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    rewrite_params ps;
    rewrite_stats st;
    begin = clock();

    if (is_set("npn")) {
      if (!store<aig_network>().empty()) {
        auto aig = store<aig_network>().current();
        xag_npn_resynthesis<aig_network> resyn;
        exact_library<aig_network> exact_lib(resyn);
        rewrite(aig, exact_lib, ps, &st);
        auto opt_aig = cleanup_dangling(aig);
        store<aig_network>().extend();
        store<aig_network>().current() = opt_aig;
      } else {
        std::cerr << "There is not an AIG network stored.\n";
      }
    } else if (is_set("sop")) {
      if (!store<aig_network>().empty()) {
        auto aig = store<aig_network>().current();
        sop_factoring<aig_network> resyn;
        exact_library<aig_network> exact_lib(resyn);
        rewrite(aig, exact_lib, ps, &st);
        auto opt_aig = cleanup_dangling(aig);
        store<aig_network>().extend();
        store<aig_network>().current() = opt_aig;
      } else {
        std::cerr << "There is not an AIG network stored.\n";
      }
    } else if (is_set("dsd")) {
      if (!store<aig_network>().empty()) {
        auto aig = store<aig_network>().current();
        xag_npn_resynthesis<aig_network> fallback;
        dsd_resynthesis<aig_network, decltype(fallback)> resyn(fallback);
        exact_library<aig_network> exact_lib(resyn);
        rewrite(aig, exact_lib, ps, &st);
        auto opt_aig = cleanup_dangling(aig);
        store<aig_network>().extend();
        store<aig_network>().current() = opt_aig;
      } else {
        std::cerr << "There is not an AIG network stored.\n";
      }
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
};
ALICE_ADD_COMMAND(rw, "Optimization")
}  // namespace alice
#endif
