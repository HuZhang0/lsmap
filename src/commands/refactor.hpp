#ifndef REFACTOR_HPP
#define REFACTOR_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>

#include "../store.hpp"

using namespace std;
using namespace mockturtle;

namespace alice {
class refactor_command : public command {
 public:
  explicit refactor_command(const environment::ptr &env)
      : command(env, "aig refactor") {
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    begin = clock();

    if (!store<aig_network>().empty()) {
      auto aig = store<aig_network>().current();
      refactoring_params ps;
      sop_factoring<aig_network> sop_resyn;
      dsd_resynthesis<aig_network, decltype(sop_resyn)> dsd_resyn(sop_resyn);
      refactoring(aig, dsd_resyn, ps);
      auto opt_aig = cleanup_dangling(aig);
      store<aig_network>().extend();
      store<aig_network>().current() = opt_aig;
    } else {
      std::cerr << "There is not an AIG network stored.\n";
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
};
ALICE_ADD_COMMAND(refactor, "Optimization")
}  // namespace alice
#endif
