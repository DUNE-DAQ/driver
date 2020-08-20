/**
 * @file HDF5Output.hpp
 * *
 * This is part of the DUNE DAQ Application Framework, copyright 2020. Licensing/copyright details are in the COPYING
 * file that you should have received with this code.
 */

#include "appfwk/DAQModule.hpp"
#include "appfwk/DAQSource.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "artdaq-demo-hdf5/HDF5/FragmentDataset.hh"

#include <memory>

namespace dunedaq {
namespace artdaq {
class HDF5Output : public appfwk::DAQModule
{
public:
  HDF5Output(std::string name);
  void init() final;

private:
	// Commands
  void do_configure(const std::vector<std::string>& args);
  void do_start(const std::vector<std::string>& args);
  void do_stop(const std::vector<std::string>& args);
  void do_unconfigure(const std::vector<std::string>& args);

  // Threading
  void do_work(std::atomic<bool>& running_flag);
  appfwk::ThreadHelper thread_;

  // Members
  std::unique_ptr<appfwk::DAQSource<artdaq::FragmentPtrs>> input_queue_;
  std::chrono::milliseconds queue_timeout_;
  std::unique_ptr<artdaq::hdf5::FragmentDataset> hdf_dataset_;
};
}
}