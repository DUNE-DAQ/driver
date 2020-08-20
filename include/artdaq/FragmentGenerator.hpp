/**
 * @file FragmentGenerator.hpp
 * *
 * This is part of the DUNE DAQ Application Framework, copyright 2020. Licensing/copyright details are in the COPYING
 * file that you should have received with this code.
 */

#include "appfwk/DAQModule.hpp"
#include "appfwk/ThreadHelper.hpp"
#include "appfwk/DAQSink.hpp"
#include "artdaq-core/Data/Fragment.hh"
#include "artdaq/Generators/CommandableFragmentGenerator.hh"

namespace dunedaq {
namespace artdaq {
class FragmentGenerator : public appfwk::DAQModule
{
public:
  FragmentGenerator(std::string name);
  void init() final;

private:
	// Command Handlers
  void do_configure(const std::vector<std::string>& args);
  void do_start(const std::vector<std::string>& args);
  void do_pause(const std::vector<std::string>& args);
  void do_resume(const std::vector<std::string>& args);
  void do_stop(const std::vector<std::string>& args);
  void do_unconfigure(const std::vector<std::string>& args);

  // Threading
  void do_work(std::atomic<bool>& running_flag);
  appfwk::ThreadHelper thread_;

  // Members
  std::unique_ptr<appfwk::DAQSink<artdaq::FragmentPtrs>> output_queue_;
  std::chrono::milliseconds queue_timeout_;
  int run_id_;
  uint64_t timeout_;
  std::atomic<bool> paused_;
  std::unique_ptr<artdaq::CommandableFragmentGenerator> generator_ptr_;
};
}
}
