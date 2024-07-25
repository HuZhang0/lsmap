#ifndef WINDOW_REWRITE_HPP
#define WINDOW_REWRITE_HPP
#include <iterator>
#include <kitty/print.hpp>
#include <mockturtle/mockturtle.hpp>
#include <mockturtle/traits.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"

using namespace std;
using namespace mockturtle;

namespace alice {
class winrw_command : public command {
 public:
  explicit winrw_command(const environment::ptr &env)
      : command(env, "window rewriting") {
    add_flag("--aig, -a", "rewrite for aig");
    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    window_rewriting_params ps;
    window_rewriting_stats st;
    begin = clock();
    if (is_set("aig")) {
      if (!store<aig_network>().empty()) {
        auto aig = store<aig_network>().current();
        window_rewriting(aig, ps, &st);
        store<aig_network>().extend();
        store<aig_network>().current() = cleanup_dangling(aig);
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
ALICE_ADD_COMMAND(winrw, "Optimization")
}  // namespace alice
#endif
