#ifndef EXACT_MAP_HPP
#define EXACT_MAP_HPP

#include <mockturtle/mockturtle.hpp>

using namespace mockturtle;

namespace alice {
class exact_map_command : public command {
 public:
  explicit exact_map_command(const environment::ptr &env)
      : command(env, "exact map for logic networks transformation") {
    add_flag("--logic_sharing, -l", "Enable logic sharing");
    add_flag("--aig, -a", "using aig as target logic network");
    add_flag("--xmg, -x", "using xmg as target logic network");
    add_flag("--xag, -g", "using xag as target logic network");
    add_flag("--opt, -o", "logic opt");
    add_flag("--dc, -d", "dont cares");
    add_flag("--area, -A", "dont cares");
  }

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    exact_library_params eps;
    begin = clock();
    if (is_set("xmg")) {
      if (is_set("opt")) {
        xmg_network xmg = store<xmg_network>().current();
        xmg_npn_resynthesis resyn;
        exact_library<xmg_network> lib(resyn, eps);
        map_params ps;
        if (is_set("logic_sharing")) {
          ps.enable_logic_sharing = true;
        }
        if (is_set("dc")) {
          ps.use_dont_cares = true;
        }
        map_stats st;

        auto xmg2 = exactmap(xmg, lib, ps, &st);

        store<xmg_network>().extend();
        store<xmg_network>().current() = cleanup_dangling(xmg2);
      } else {
        assert(store<aig_network>().size() > 0);
        aig_network aig = store<aig_network>().current();

        xmg_npn_resynthesis resyn;
        exact_library<xmg_network> lib(resyn, eps);
        map_params ps;
        if (is_set("logic_sharing")) {
          ps.enable_logic_sharing = true;
        }
        map_stats st;

        auto xmg = exactmap(aig, lib, ps, &st);

        store<xmg_network>().extend();
        store<xmg_network>().current() = cleanup_dangling(xmg);
      }
    } else if (is_set("aig")) {
      if (is_set("opt")) {
        aig_network aig = store<aig_network>().current();
        xag_npn_resynthesis<aig_network> resyn;
        exact_library<aig_network> lib(resyn, eps);
        map_params ps;
        if (is_set("logic_sharing")) {
          ps.enable_logic_sharing = true;
        }
        if (is_set("dc")) {
          ps.use_dont_cares = true;
        }
        map_stats st;
        auto aig2 = exactmap(aig, lib, ps, &st);
        store<aig_network>().extend();
        store<aig_network>().current() = cleanup_dangling(aig2);
      } else {
        xmg_network xmg = store<xmg_network>().current();
        xag_npn_resynthesis<aig_network> resyn;
        exact_library<aig_network> lib(resyn, eps);
        map_params ps;
        if (is_set("logic_sharing")) {
          ps.enable_logic_sharing = true;
        }
        map_stats st;
        auto aig = exactmap(xmg, lib, ps, &st);
        store<aig_network>().extend();
        store<aig_network>().current() = cleanup_dangling(aig);
      }
    } else if (is_set("xag")) {
      if (is_set("opt")) {
        xag_network xag = store<xag_network>().current();
        xag_npn_resynthesis<xag_network> resyn;
        exact_library<xag_network> lib(resyn, eps);
        map_params ps;
        if (is_set("logic_sharing")) {
          ps.enable_logic_sharing = true;
        }
        if (is_set("dc")) {
          ps.use_dont_cares = true;
        }
        if (is_set("area")) {
          ps.skip_delay_round = true;
        }
        map_stats st;
        auto xag2 = exactmap(xag, lib, ps, &st);
        store<xag_network>().extend();
        store<xag_network>().current() = cleanup_dangling(xag2);
      } else {
        aig_network aig = store<aig_network>().current();
        xag_npn_resynthesis<xag_network> resyn;
        exact_library<xag_network> lib(resyn, eps);
        map_params ps;
        if (is_set("logic_sharing")) {
          ps.enable_logic_sharing = true;
        }
        if (is_set("area")) {
          ps.skip_delay_round = true;
        }
        map_stats st;
        xag_network xag = exactmap(aig, lib, ps, &st);
        store<xag_network>().extend();
        store<xag_network>().current() = cleanup_dangling(xag);
      }
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
};

ALICE_ADD_COMMAND(exact_map, "Optimization")
}  // namespace alice
#endif
