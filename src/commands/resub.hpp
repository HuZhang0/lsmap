#ifndef RESUB_HPP
#define RESUB_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"

using namespace std;
using namespace mockturtle;

namespace alice {
class resub_command : public command {
 public:
  explicit resub_command(const environment::ptr &env) : command(env, "resub") {
    add_flag("--aig, -a", "aig resub");
    add_flag("--max_pis, -p",
             "Maximum number of PIs of reconvergence-driven cuts, default = 8");
    add_flag("--max_divisors, -d",
             "Maximum number of divisors to consider, default = 150");
    add_flag("--max_inserts, -i",
             "Maximum number of nodes added by resubstitution, default = 2");
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    begin = clock();

    if (is_set("aig")) {
      if (!store<aig_network>().empty()) {
        auto aig = store<aig_network>().current();
        resubstitution_params ps;
        ps.max_pis = max_pis;
        ps.max_divisors = max_divisors;
        ps.max_inserts = max_inserts;
        if (is_set("verbose")) {
          ps.verbose = true;
        }
        aig_resubstitution(aig, ps);
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
  int max_pis = 8;
  int max_divisors = 150;
  int max_inserts = 2;
};
ALICE_ADD_COMMAND(resub, "Optimization")
}  // namespace alice
#endif
