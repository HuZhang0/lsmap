#ifndef EMAP_HPP
#define EMAP_HPP
#include <mockturtle/mockturtle.hpp>

namespace alice {

class emap_command : public command {
 public:
  explicit emap_command(const environment::ptr& env)
      : command(env, "Standard cell mapping : using AIG as default") {
    add_flag("--xmg, -x", "Standard cell mapping for XMG");
    add_flag("--aig, -a", "Standard cell mapping for AIG");
    add_flag("--mig, -m", "Standard cell mapping for MIG");
    add_flag("--xag, -g", "Standard cell mapping for MIG");
    add_flag("--area, -A", "Do area-oriented mapping ");
    add_flag("--mul, -M", "map_multioutput");
    // add_flag("--node, -n", "Standard cell mapping with node map");
    add_option("--output, -o", filename, "the verilog filename");
    add_flag("--verbose, -v", "print the information");
  }

  rules validity_rules() const {
    return {has_store_element<std::vector<mockturtle::gate>>(env)};
  }

 private:
  std::string filename = "emap.v";

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    /* derive genlib */
    std::vector<mockturtle::gate> gates =
        store<std::vector<mockturtle::gate>>().current();
    tech_library_params tps;
    tps.ignore_symmetries = false;
    mockturtle::tech_library<9> lib(gates, tps);
    mockturtle::emap_params ps;
    ps.matching_mode = emap_params::hybrid;
    ps.area_oriented_mapping = false;
    ps.map_multioutput = true;
    ps.relax_required = 0;
    mockturtle::emap_stats st;
    begin = clock();
    if (is_set("xmg")) {
      if (store<xmg_network>().size() == 0u) {
        std::cerr << "[e] no XMG in the store\n";
      } else {
        if (is_set("area")) {
          ps.area_oriented_mapping = true;
        }
        if (is_set("mul")) {
          ps.map_multioutput = true;
        }
        auto xmg = store<xmg_network>().current();

        cell_view<block_network> res = mockturtle::emap<9>(xmg, lib, ps, &st);

        if (is_set("output")) {
          mockturtle::write_verilog_with_cell(res, filename);
        }

        std::cout << fmt::format(
            "[i] Mapped XMG into #gates = {} area = {:.2f} delay = {:.2f}\n",
            res.num_gates(), st.area, st.delay);
      }
    } else if (is_set("aig")) {
      if (store<aig_network>().size() == 0u) {
        std::cerr << "[e] no AIG in the store\n";
      } else {
        if (is_set("area")) {
          ps.area_oriented_mapping = true;
        }
        if (is_set("mul")) {
          ps.map_multioutput = true;
        }
        auto aig = store<aig_network>().current();

        cell_view<block_network> res = mockturtle::emap<9>(aig, lib, ps, &st);

        if (is_set("output")) {
          mockturtle::write_verilog_with_cell(res, filename);
        }

        std::cout << fmt::format(
            "Mapped AIG into #gates = {} area = {:.2f} delay = {:.2f}\n",
            res.num_gates(), st.area, st.delay);
      }
    } else if (is_set("mig")) {
      if (store<mig_network>().size() == 0u) {
        std::cerr << "[e] no MIG in the store\n";
      } else {
        if (is_set("area")) {
          ps.area_oriented_mapping = true;
        }
        if (is_set("mul")) {
          ps.map_multioutput = true;
        }
        auto mig = store<mig_network>().current();

        cell_view<block_network> res = mockturtle::emap<9>(mig, lib, ps, &st);

        if (is_set("output")) {
          mockturtle::write_verilog_with_cell(res, filename);
        }

        std::cout << fmt::format(
            "Mapped MIG into #gates = {} area = {:.2f} delay = {:.2f}\n",
            res.num_gates(), st.area, st.delay);
      }
    } else if (is_set("xag")) {
      if (store<xag_network>().size() == 0u) {
        std::cerr << "[e] no XAG in the store\n";
      } else {
        if (is_set("area")) {
          ps.area_oriented_mapping = true;
        }
        if (is_set("mul")) {
          ps.map_multioutput = true;
        }
        auto xag = store<xag_network>().current();

        cell_view<block_network> res = mockturtle::emap<9>(xag, lib, ps, &st);

        if (is_set("output")) {
          mockturtle::write_verilog_with_cell(res, filename);
        }

        std::cout << fmt::format(
            "Mapped XAG into #gates = {} area = {:.2f} delay = {:.2f}\n",
            res.num_gates(), st.area, st.delay);
      }
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;

    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }
};

ALICE_ADD_COMMAND(emap, "Mapping")

}  // namespace alice

#endif
