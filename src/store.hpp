#ifndef STORE_HPP
#define STORE_HPP

#include <alice/alice.hpp>
#include <mockturtle/mockturtle.hpp>
#include <fmt/format.h>
#include <kitty/kitty.hpp>

#include <mockturtle/io/write_verilog.hpp>
#include <mockturtle/io/write_aiger.hpp>
#include <mockturtle/io/write_blif.hpp>
#include <mockturtle/io/blif_reader.hpp>
#include <mockturtle/io/genlib_reader.hpp>
#include <mockturtle/views/names_view.hpp>
#include <lorina/genlib.hpp>
#include <lorina/diagnostics.hpp>

using namespace mockturtle;

namespace alice {
//aig
ALICE_ADD_STORE(aig_network, "aig", "a", "AIG", "AIGs")

ALICE_PRINT_STORE(aig_network, os, element) {
  os << "AIG PI/PO = " << element.num_pis() << "/" << element.num_pos() << "\n";
}

ALICE_DESCRIBE_STORE(aig_network, element) {
  return fmt::format("{} nodes", element.size());
}

/* mig */
  ALICE_ADD_STORE( mig_network, "mig", "m", "MIG", "MIGs" )

  ALICE_PRINT_STORE( mig_network, os, element )
  {
    os << "MIG PI/PO = " << element.num_pis() << "/" << element.num_pos() << "\n";
  }

  ALICE_DESCRIBE_STORE( mig_network, element )
  {
    return fmt::format( "{} nodes", element.size() );
  }

/* xmg */
  ALICE_ADD_STORE( xmg_network, "xmg", "x", "xmg", "xmgs" )

  ALICE_PRINT_STORE( xmg_network, os, element )
  {
    os << fmt::format( " xmg i/o = {}/{} gates = {} ", element.num_pis(), element.num_pos(), element.num_gates() );
    os << "\n";
  }

  ALICE_DESCRIBE_STORE( xmg_network, element )
  {
    return fmt::format( "{} nodes", element.size() );
  }

/* xag */
  ALICE_ADD_STORE( xag_network, "xag", "g", "xag", "xags" )

  ALICE_PRINT_STORE( xag_network, os, element )
  {
    os << fmt::format( " xag i/o = {}/{} gates = {} ", element.num_pis(), element.num_pos(), element.num_gates() );
    os << "\n";
  }

  ALICE_DESCRIBE_STORE( xag_network, element )
  {
    return fmt::format( "{} nodes", element.size() );
  }



//k-lut
ALICE_ADD_STORE( klut_network, "lut", "l", "LUT network", "LUT networks" )

  ALICE_PRINT_STORE( klut_network, os, element )
  {
    os << fmt::format( " klut i/o = {}/{} gates = {} ", element.num_pis(), element.num_pos(), element.num_gates() );
    os << "\n";
  }

  ALICE_DESCRIBE_STORE( klut_network, element )
  {
    return fmt::format( "{} nodes", element.size() );
  }

  ALICE_PRINT_STORE_STATISTICS( klut_network, os, lut )
  {
    mockturtle::depth_view depth_lut{lut};
    os << fmt::format( "LUTs   i/o = {}/{}   gates = {}   level = {}",
          lut.num_pis(), lut.num_pos(), lut.num_gates(), depth_lut.depth() );
    os << "\n";
  }

//opt_network
class optimum_network {
 public:
  optimum_network() = default;

  optimum_network(const kitty::dynamic_truth_table &function)
      : function(function) {}

  optimum_network(kitty::dynamic_truth_table &&function)
      : function(std::move(function)) {}

  bool exists() const {
    static std::vector<std::unordered_set<
        kitty::dynamic_truth_table, kitty::hash<kitty::dynamic_truth_table>>>
        hash;

    if (function.num_vars() >= hash.size()) {
      hash.resize(function.num_vars() + 1);
    }

    return !hash[function.num_vars()].insert(function).second;
  }

 public: /* field access */
  kitty::dynamic_truth_table function{0};
  std::string network;
};

ALICE_ADD_STORE(optimum_network, "opt", "o", "network", "networks")

ALICE_DESCRIBE_STORE(optimum_network, opt) {
  if (opt.network.empty()) {
    return fmt::format("{}", kitty::to_hex(opt.function));
  } else {
    return fmt::format("{}, optimum network computed",
                       kitty::to_hex(opt.function));
  }
}

ALICE_PRINT_STORE(optimum_network, os, opt) {
  os << fmt::format("function (hex): {}\nfunction (bin): {}\n",
                    kitty::to_hex(opt.function),
                    kitty::to_binary(opt.function));

  if (opt.network.empty()) {
    os << "no optimum network computed\n";
  } else {
    os << fmt::format("optimum network: {}\n", opt.network);
  }
}

/* genlib */
  ALICE_ADD_STORE( std::vector<mockturtle::gate>, "genlib", "f", "GENLIB", "GENLIBs" )

  ALICE_PRINT_STORE( std::vector<mockturtle::gate>, os, element )
  {
    os << "GENLIB gate size = " << element.size() << "\n";
  }

  ALICE_DESCRIBE_STORE( std::vector<mockturtle::gate>, element )
  {
    return fmt::format( "{} gates", element.size() );
  }

  ALICE_ADD_FILE_TYPE( genlib, "Genlib" );

  ALICE_READ_FILE( std::vector<mockturtle::gate>, genlib, filename, cmd )
  {
    std::vector<mockturtle::gate> gates;
    if( lorina::read_genlib( filename, mockturtle::genlib_reader( gates ) ) != lorina::return_code::success )
    {
      std::cout << "[w] parse error\n";
    }
    return gates;
  }

  ALICE_WRITE_FILE( std::vector<mockturtle::gate>, genlib, gates, filename, cmd )
  {
    std::cout << "[e] not supported" << std::endl;
  }

  ALICE_PRINT_STORE_STATISTICS( std::vector<mockturtle::gate>, os, gates )
  {
    os << fmt::format( "Entered genlib library with {} gates", gates.size() );
    os << "\n";
  }

  /* show */
  template <>
  bool can_show<aig_network>(std::string& extension, command& cmd) {
    extension = "dot";

    return true;
  }

  template <>
  void show<aig_network>(std::ostream& os, const aig_network& element,
                         const command& cmd) {
    gate_dot_drawer<aig_network> drawer;
    write_dot(element, os, drawer);
  }

  template <>
  bool can_show<mig_network>(std::string& extension, command& cmd) {
    extension = "dot";

    return true;
  }

  template <>
  void show<mig_network>(std::ostream& os, const mig_network& element,
                         const command& cmd) {
    gate_dot_drawer<mig_network> drawer;
    write_dot(element, os, drawer);
  }

  template <>
  bool can_show<xmg_network>(std::string& extension, command& cmd) {
    extension = "dot";

    return true;
  }

  template <>
  void show<xmg_network>(std::ostream& os, const xmg_network& element,
                         const command& cmd) {
    gate_dot_drawer<xmg_network> drawer;
    write_dot(element, os, drawer);
  }

  template <>
  bool can_show<klut_network>(std::string& extension, command& cmd) {
    extension = "dot";

    return true;
  }

  template <>
  void show<klut_network>(std::ostream& os, const klut_network& element,
                          const command& cmd) {
    gate_dot_drawer<klut_network> drawer;
    write_dot(element, os, drawer);
  }

  template <>
  bool can_show<xag_network>(std::string& extension, command& cmd) {
    extension = "dot";

    return true;
  }

  template <>
  void show<xag_network>(std::ostream& os, const xag_network& element,
                         const command& cmd) {
    gate_dot_drawer<xag_network> drawer;
    write_dot(element, os, drawer);
  }

/*Convert*/
  ALICE_CONVERT( aig_network, element, xmg_network )
  {
    aig_network aig = element;

    /* LUT mapping */
    mapping_view<aig_network, true> mapped_aig{aig};
    lut_mapping_params ps;
    ps.cut_enumeration_ps.cut_size = 4;
    lut_mapping<mapping_view<aig_network, true>, true>( mapped_aig, ps );

    /* collapse into k-LUT network */
    const auto klut = *collapse_mapped_network<klut_network>( mapped_aig );

    /* node resynthesis */
    xmg_npn_resynthesis resyn;
    auto xmg = node_resynthesis<xmg_network>( klut, resyn );

    return xmg;
  }


/*Read and Write */

ALICE_ADD_FILE_TYPE(aiger, "Aiger");

ALICE_READ_FILE(aig_network, aiger, filename, cmd) {
  aig_network aig;
  if (lorina::read_aiger(filename, mockturtle::aiger_reader(aig)) !=
      lorina::return_code::success) {
    std::cout << "[w] parse error\n";
  }
  return aig;
}

ALICE_PRINT_STORE_STATISTICS(aig_network, os, aig) {
  auto aig_copy = mockturtle::cleanup_dangling(aig);
  mockturtle::depth_view depth_aig{aig_copy};
  os << fmt::format("AIG   i/o = {}/{}   gates = {}   level = {}",
                    aig.num_pis(), aig.num_pos(), aig.num_gates(),
                    depth_aig.depth());
  os << "\n";
}

ALICE_ADD_FILE_TYPE( verilog, "Verilog" );

ALICE_READ_FILE( xmg_network, verilog, filename, cmd )
  {
    xmg_network xmg;

    if ( lorina::read_verilog( filename, mockturtle::verilog_reader( xmg ) ) != lorina::return_code::success )
    {
      std::cout << "[w] parse error\n";
    }
    return xmg;
  }

  ALICE_WRITE_FILE( xmg_network, verilog, xmg, filename, cmd )
  {
    mockturtle::write_verilog( xmg, filename );
  }

  ALICE_PRINT_STORE_STATISTICS( xmg_network, os, xmg )
  {
    auto xmg_copy = mockturtle::cleanup_dangling( xmg );
    mockturtle::depth_view depth_xmg{xmg_copy};
    os << fmt::format( "XMG   i/o = {}/{}   gates = {}   level = {}",
          xmg.num_pis(), xmg.num_pos(), xmg.num_gates(), depth_xmg.depth() );
    os << "\n";
  }

ALICE_READ_FILE( mig_network, verilog, filename, cmd )
  {
    mig_network mig;
    if( lorina::read_verilog( filename, mockturtle::verilog_reader( mig ) ) != lorina::return_code::success )
    {
      std::cout << "[w] parse error\n";
    }
    return mig;
  }

  ALICE_WRITE_FILE( mig_network, verilog, mig, filename, cmd )
  {
     mockturtle::write_verilog( mig, filename );
  }

  ALICE_PRINT_STORE_STATISTICS( mig_network, os, mig )
  {
    auto mig_copy = mockturtle::cleanup_dangling( mig );
    mockturtle::depth_view depth_mig{mig_copy};
    os << fmt::format( "MIG   i/o = {}/{}   gates = {}   level = {}",
          mig.num_pis(), mig.num_pos(), mig.num_gates(), depth_mig.depth() );
    os << "\n";
  }

  ALICE_READ_FILE( xag_network, verilog, filename, cmd )
  {
    xag_network xag;
    if( lorina::read_verilog( filename, mockturtle::verilog_reader( xag ) ) != lorina::return_code::success )
    {
      std::cout << "[w] parse error\n";
    }

    return xag;
  }

  ALICE_WRITE_FILE( xag_network, verilog, xag, filename, cmd )
  {
     mockturtle::write_verilog( xag, filename );
  }

  ALICE_PRINT_STORE_STATISTICS( xag_network, os, xag )
  {
    auto xag_copy = mockturtle::cleanup_dangling( xag );
    mockturtle::depth_view depth_xag{xag_copy};
    os << fmt::format( "XAG   i/o = {}/{}   gates = {}   level = {}",
          xag.num_pis(), xag.num_pos(), xag.num_gates(), depth_xag.depth() );
    os << "\n";
  }

  ALICE_ADD_FILE_TYPE( bench, "BENCH" );
  ALICE_READ_FILE( klut_network, bench, filename, cmd )
  {
    klut_network klut;
    if( lorina::read_bench( filename, mockturtle::bench_reader( klut ) ) != lorina::return_code::success )
    {
      std::cout << "[w] parse error\n";
    }
    return klut;
  }
  ALICE_WRITE_FILE( aig_network, bench, aig, filename, cmd )
  {
     mockturtle::write_bench( aig, filename );
  }
  ALICE_WRITE_FILE( xmg_network, bench, xmg, filename, cmd )
  {
     mockturtle::write_bench( xmg, filename );
  }
  ALICE_WRITE_FILE( mig_network, bench, mig, filename, cmd )
  {
     mockturtle::write_bench( mig, filename );
  }
  ALICE_WRITE_FILE( xag_network, bench, xag, filename, cmd )
  {
     mockturtle::write_bench( xag, filename );
  }

  ALICE_WRITE_FILE( klut_network, bench, klut, filename, cmd )
  {
     mockturtle::write_bench( klut, filename );
  }
  ALICE_WRITE_FILE( aig_network, aiger, aig, filename, cmd )
  {
	  mockturtle::write_aiger(aig, filename);
  }


  ALICE_ADD_FILE_TYPE( blif, "Blif" );

  ALICE_READ_FILE( klut_network, blif, filename, cmd )
  {
    klut_network klut;

    if ( lorina::read_blif( filename, mockturtle::blif_reader( klut ) ) != lorina::return_code::success )
    {
      std::cout << "[w] parse error\n";
    }

    return klut;
  }
  ALICE_WRITE_FILE( klut_network, blif, klut, filename, cmd )
  {
    mockturtle::write_blif( klut, filename );
  } 

}
#endif