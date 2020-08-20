/**
 * @file board_reader.cxx
 *
 * board_reader loads an artdaq CommandableFragmentGenerator and writes its output to HDF5
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "appfwk/CommandLineInterpreter.hpp"
#include "appfwk/DAQProcess.hpp"
#include "artdaq/HDF5Output.hpp"
#include "artdaq/FragmentGenerator.hpp"

#include <string>

namespace dunedaq::artdaq {
/**
 * @brief ModuleList for the echo_test_app
 */
class board_reader_contructor : public dunedaq::appfwk::GraphConstructor
{
  // Inherited via ModuleList
  void ConstructGraph(dunedaq::appfwk::DAQModuleMap& user_module_map,
                      dunedaq::appfwk::CommandOrderMap& /*command_order_map*/) const override
  {
    user_module_map["generator"].reset(new FragmentGenerator("generator"));
    user_module_map["writer"].reset(new HDF5Output("writer"));
  }
};
} // namespace dunedaq::artdaq

/**
 * @brief echo_test_app main entry point
 * @param argc Number of arguments
 * @param argv Arguments
 * @return Status code from DAQProcess::listen
 */
int
main(int argc, char* argv[])
{
  auto args = dunedaq::appfwk::CommandLineInterpreter::ParseCommandLineArguments(argc, argv);

  dunedaq::appfwk::DAQProcess theDAQProcess(args);

  dunedaq::artdaq::board_reader_contructor gc;
  theDAQProcess.register_modules(gc);

  return theDAQProcess.listen();
}
