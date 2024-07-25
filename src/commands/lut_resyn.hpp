#ifndef LUT_RESYN_HPP
#define LUT_RESYN_HPP

#include <mockturtle/mockturtle.hpp>

using namespace lsmap;

namespace alice {

class lut_resyn_command : public command {
 public:
  explicit lut_resyn_command(const environment::ptr& env)
      : command(env, "4-lut networks resyn to target networks") {
    add_option("cut_size, -k", cut_size,
               "set the cut size from 2 to 8, default = 4");
    add_flag("--verbose, -v", "print the information");
    add_flag("--xmg, -x", "using xmg as target logic network");
    add_flag("--xag, -g", "using xag as target logic network");
    add_flag("--aig, -a", "using aig as target logic network");
  }

  rules validity_rules() const {
    return {has_store_element<klut_network>(env)};
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    klut_network klut = store<klut_network>().current();

    begin = clock();
    if (is_set("xmg")) {
      xmg_network xmg;
      {
        xmg_npn_resynthesis resyn;
        xmg = node_resynthesis<xmg_network>(klut, resyn);
      }
      store<xmg_network>().extend();
      store<xmg_network>().current() = cleanup_dangling(xmg);
    } else if (is_set("aig")) {
      aig_network aig;
      {
        xag_npn_resynthesis<aig_network> resyn;
        aig = node_resynthesis<aig_network>(klut, resyn);
      }
      store<aig_network>().extend();
      store<aig_network>().current() = cleanup_dangling(aig);
    } else if (is_set("xag")) {
      xag_network xag;
      {
        xag_npn_resynthesis<xag_network> resyn;
        xag = node_resynthesis<xag_network>(klut, resyn);
      }
      store<xag_network>().extend();
      store<xag_network>().current() = cleanup_dangling(xag);
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
  int cut_size = 4;
};

ALICE_ADD_COMMAND(lut_resyn, "Rewriting")

}  // namespace alice

#endif
