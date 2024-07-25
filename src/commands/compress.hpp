#ifndef COMPRESS_HPP
#define COMPRESS_HPP
#include <iterator>
#include <mockturtle/algorithms/aig_balancing.hpp>
#include <mockturtle/algorithms/cleanup.hpp>
#include <mockturtle/algorithms/node_resynthesis/dsd.hpp>
#include <mockturtle/algorithms/node_resynthesis/sop_factoring.hpp>
#include <mockturtle/algorithms/node_resynthesis/xag_npn.hpp>
#include <mockturtle/algorithms/refactoring.hpp>
#include <mockturtle/algorithms/rewrite.hpp>
#include <mockturtle/generators/arithmetic.hpp>
#include <mockturtle/mockturtle.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/networks/klut.hpp>
#include <mockturtle/views/mapping_view.hpp>
#include <percy/percy.hpp>

#include "../store.hpp"
#include "core/misc.hpp"
using namespace std;
using namespace mockturtle;
using namespace lsmap;

namespace alice {
class compress_command : public command {
 public:
  explicit compress_command(const environment::ptr &env)
      : command(env,
                "abc flow compress: b -l; rw -l; rwz -l; b -l; rwz -l; b -l") {
    add_flag("--compress2, -c",
             "abc flow compress2:b -l; rw -l; rf -l; b -l; rw -l; rwz -l; b "
             "-l; rfz -l; "
             "rwz -l; b -l ");

    add_flag("--verbose, -v", "print the information");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    aig_balancing_params b_ps;
    rewrite_params rw_ps;
    refactoring_params rf_ps;
    resubstitution_params resub_ps;
    resubstitution_params resub_st;
    begin = clock();
    if (is_set("compress2")) {
      if (!store<aig_network>().empty()) {
        aig_network aig = store<aig_network>().current();
        auto aig1 = cleanup_dangling(aig);
        b_ps.minimize_levels = false;
        aig_balance(aig1);
        // rw -l
        xag_npn_resynthesis<aig_network> resyn;
        exact_library_params eps;
        eps.np_classification = false;
        exact_library<aig_network> exact_lib(resyn, eps);
        rewrite(aig1, exact_lib, rw_ps);
        // rwz -l
        rw_ps.allow_zero_gain = true;
        rewrite(aig1, exact_lib, rw_ps);
        // resub
        sim_resubstitution(aig1, resub_ps);
        // b -l
        b_ps.minimize_levels = false;
        aig_balance(aig1);
        // rwz -l
        rw_ps.allow_zero_gain = true;
        rewrite(aig1, exact_lib, rw_ps);
        // ref
        sop_factoring<aig_network> fallback;
        dsd_resynthesis<aig_network, decltype(fallback)> rf_resyn(fallback);
        rf_ps.allow_zero_gain = true;
        refactoring(aig1, rf_resyn, rf_ps);
        // b -l
        b_ps.minimize_levels = false;
        aig_balance(aig1);
        // resub
        sim_resubstitution(aig1, resub_ps);

        store<aig_network>().extend();
        aig1 = cleanup_dangling(aig1);
        store<aig_network>().current() = aig1;
      } else {
        std::cerr << "There is not an AIG network stored.\n";
      }
    } else {
      if (!store<aig_network>().empty()) {
        aig_network aig = store<aig_network>().current();
        auto aig2 = cleanup_dangling(aig);
        xag_npn_resynthesis<aig_network> resyn;
        exact_library_params eps;
        eps.np_classification = false;
        exact_library<aig_network> exact_lib(resyn, eps);
        // b -l
        b_ps.minimize_levels = false;
        aig_balance(aig2);
        // rw -l
        rw_ps.preserve_depth = true;
        rw_ps.allow_zero_gain = false;
        rewrite(aig2, exact_lib, rw_ps);
        // rf -l
        sop_factoring<aig_network> fallback;
        dsd_resynthesis<aig_network, decltype(fallback)> rf_resyn(fallback);
        // xag_npn_resynthesis<aig_network> rf_resyn;
        refactoring(aig2, rf_resyn);
        // b -l
        b_ps.minimize_levels = false;
        aig_balance(aig2);
        // rw -l
        rw_ps.preserve_depth = true;
        rw_ps.allow_zero_gain = false;
        rewrite(aig2, exact_lib, rw_ps);
        // rwz -l
        rw_ps.preserve_depth = true;
        rw_ps.allow_zero_gain = false;
        rewrite(aig2, exact_lib, rw_ps);
        // b -l
        b_ps.minimize_levels = false;
        aig_balance(aig2);
        // rfz -l
        rf_ps.allow_zero_gain = true;
        refactoring(aig2, rf_resyn, rf_ps);
        // rwz -l
        rw_ps.preserve_depth = true;
        rw_ps.allow_zero_gain = false;
        rewrite(aig2, exact_lib, rw_ps);
        // b -l
        b_ps.minimize_levels = false;
        aig_balance(aig2);
        store<aig_network>().extend();
        aig2 = cleanup_dangling(aig2);
        store<aig_network>().current() = aig2;
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
ALICE_ADD_COMMAND(compress, "Optimization")
}  // namespace alice
#endif
