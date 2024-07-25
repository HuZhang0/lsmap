#ifndef TECHMAP_HPP
#define TECHMAP_HPP

#include <mockturtle/mockturtle.hpp>

namespace alice {

class techmap_command : public command {
 public:
  explicit techmap_command(const environment::ptr& env)
      : command(env, "Standard cell mapping : using AIG as default") {
    add_option("cut_limit, -l", cut_limit,
               "set the cut size from 2 to 49, default = 49");
    add_option("cut_size, -k", cut_size,
               "set the lut size from 2 to 8, default = 6");
    add_flag("--xmg, -x", "Standard cell mapping for XMG");
    add_flag("--mig, -m", "Standard cell mapping for MIG");
    add_option("--output, -o", filename, "the verilog filename");
    add_flag("--area, -A", "area mapping");
    add_flag("--verbose, -v", "print the information");
  }

  rules validity_rules() const {
    return {has_store_element<std::vector<mockturtle::gate>>(env)};
  }

 private:
  std::string filename = "techmap.v";

 protected:
  void execute() {
    clock_t begin, end;
    double totalTime;
    std::vector<mockturtle::gate> gates =
        store<std::vector<mockturtle::gate>>().current();
    mockturtle::tech_library_params tps;
    mockturtle::tech_library<6> lib(gates, tps);
    mockturtle::map_params ps;
    mockturtle::map_stats st;
    ps.cut_enumeration_ps.cut_size = cut_size;
    ps.cut_enumeration_ps.cut_limit = cut_limit;
    if (is_set("area")) {
      ps.skip_delay_round = true;
    }
    begin = clock();
    if (is_set("xmg")) {
      if (store<xmg_network>().size() == 0u) {
        std::cerr << "[e] no XMG in the store\n";
      } else {
        auto xmg = store<xmg_network>().current();
        auto res = mockturtle::map(xmg, lib, ps, &st);
        if (is_set("output")) {
          mockturtle::write_verilog_with_binding(res, filename);
        }
        std::cout << fmt::format(
            "[i] Mapped XMG into #gates = {} area = {:.2f} delay = {:.2f}\n",
            res.num_gates(), st.area, st.delay);
      }
    } else if (is_set("mig")) {
      if (store<mig_network>().size() == 0u) {
        std::cerr << "[e] no MIG in the store\n";
      } else {
        auto mig = store<mig_network>().current();
        auto res = mockturtle::map(mig, lib, ps, &st);
        if (is_set("output")) {
          mockturtle::write_verilog_with_binding(res, filename);
        }
        std::cout << fmt::format(
            "Mapped MIG into #gates = {} area = {:.2f} delay = {:.2f}\n",
            res.num_gates(), st.area, st.delay);
      }
    } else {
      if (store<aig_network>().size() == 0u) {
        std::cerr << "[e] no AIG in the store\n";
      } else {
        auto aig = store<aig_network>().current();
        auto res = mockturtle::map(aig, lib, ps, &st);
        if (is_set("output")) {
          mockturtle::write_verilog_with_binding(res, filename);
        }
        std::cout << fmt::format(
            "Mapped AIG into #gates = {} area = {:.2f} delay = {:.2f}\n",
            res.num_gates(), st.area, st.delay);
      }
    }
    end = clock();
    totalTime = (double)(end - begin) / CLOCKS_PER_SEC;
    cout.setf(ios::fixed);
    cout << "[CPU time]   " << setprecision(2) << totalTime << " s" << endl;
  }

 private:
  int cut_size = 6;
  int cut_limit = 49;
};

ALICE_ADD_COMMAND(techmap, "Mapping")

}  // namespace alice

#endif
