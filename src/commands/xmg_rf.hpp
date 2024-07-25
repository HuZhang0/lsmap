#ifndef XMG_RF_HPP
#define XMG_RF_HPP
#include <iterator>
#include <mockturtle/mockturtle.hpp>

#include "../store.hpp"

using namespace std;
using namespace mockturtle;

namespace alice {
class xmg_rf_command : public command {
 public:
  explicit xmg_rf_command(const environment::ptr &env)
      : command(env, "xmg refactor") {
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    begin = clock();
    if (!store<xmg_network>().empty()) {
      auto xmg = store<xmg_network>().current();
      refactoring_params ps;
      sop_factoring<xmg_network> fallback;
      dsd_resynthesis<xmg_network, decltype(fallback)> rf_resyn(fallback);
      refactoring(xmg, rf_resyn, ps);
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
};
ALICE_ADD_COMMAND(xmg_rf, "Optimization")
}  // namespace alice
#endif
