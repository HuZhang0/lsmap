#ifndef DM_HPP
#define DM_HPP
#include <mockturtle/mockturtle.hpp>
#include "../core/direct_mapping.hpp"

namespace alice {
class dm_command : public command {
 public:
  explicit dm_command(const environment::ptr& env)
      : command(env, "Direct mapping of logic networks by one-to-one mapping") {
    add_flag("--aig2xmg", "AIG to XMG");
    add_flag("--aig2xag", "AIG to XAG");
    add_flag("--xmg2mig", "XMG to MIG");
    add_flag("--mig2xmg", "MIG to XMG");
  }

 protected:
  void execute() {
    if (is_set("aig2xmg")) {
      assert(store<aig_network>().size() > 0);
      aig_network aig = store<aig_network>().current();

      store<xmg_network>().extend();
      store<xmg_network>().current() = lsmap::xmg_from_aig(aig);
    } else if (is_set("aig2xag")) {
      assert(store<aig_network>().size() > 0);
      aig_network aig = store<aig_network>().current();

      store<xag_network>().extend();
      store<xag_network>().current() = lsmap::xag_from_aig(aig);
    } else if (is_set("xmg2mig")) {
      assert(store<xmg_network>().size() > 0);
      xmg_network xmg = store<xmg_network>().current();

      store<mig_network>().extend();
      store<mig_network>().current() = lsmap::mig_from_xmg(xmg);
    } else if (is_set("mig2xmg")) {
      assert(store<mig_network>().size() > 0);
      mig_network mig = store<mig_network>().current();

      store<xmg_network>().extend();
      store<xmg_network>().current() = lsmap::xmg_from_mig(mig);
    } else {
      std::cout << "You must specify the transformation parameter, press '-h' "
                   "for help\n";
    }
  }
};

ALICE_ADD_COMMAND(dm, "Optimization")
}  // namespace alice

#endif
