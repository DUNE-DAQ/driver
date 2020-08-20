/**
 * @file FragmentGenerator.cxx FragmentGenerator class implementation.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "driver/FragmentGenerator.hpp"
#include "driver/json_to_fhicl.hpp"
#include "artdaq-core/Utilities/TimeUtils.hh"
#include "artdaq/Generators/makeCommandableFragmentGenerator.hh"

#include <fhiclcpp/make_ParameterSet.h>

DEFINE_DUNE_DAQ_MODULE(dunedaq::driver::FragmentGenerator)

dunedaq::driver::FragmentGenerator::FragmentGenerator(std::string name)
  : DAQModule(name)
  , thread_(std::bind(&FragmentGenerator::do_work, this, std::placeholders::_1))
{
  register_command("configure", &FragmentGenerator::do_configure);
  register_command("start", &FragmentGenerator::do_start);
  register_command("pause", &FragmentGenerator::do_pause);
  register_command("resume", &FragmentGenerator::do_resume);
  register_command("stop", &FragmentGenerator::do_stop);
  register_command("unconfigure", &FragmentGenerator::do_unconfigure);
}

void
dunedaq::driver::FragmentGenerator::init()
{
  output_queue_.reset(new appfwk::DAQSink<artdaq::FragmentPtrs>(get_config()["output"].get<std::string>()));
}

void
dunedaq::driver::FragmentGenerator::do_configure(const std::vector<std::string>& /*args*/)
{
  std::string generator_plugin = get_config()["generator_plugin"].get<std::string>();

  fhicl::ParameterSet gen_ps_tmp = json_to_fhicl(get_config());
  fhicl::ParameterSet gen_ps;

  make_ParameterSet(gen_ps_tmp.to_string(), gen_ps);

  generator_ptr_ = artdaq::makeCommandableFragmentGenerator(generator_plugin,gen_ps);
}

void
dunedaq::driver::FragmentGenerator::do_start(const std::vector<std::string>& /*args*/)
{
  if (generator_ptr_ != nullptr) {
    generator_ptr_->StartCmd(run_id_, timeout_, artdaq::TimeUtils::gettimeofday_us());
  }
  paused_ = false;
  thread_.start_working_thread();
}

void
dunedaq::driver::FragmentGenerator::do_pause(const std::vector<std::string>& /*args*/)
{
  if (generator_ptr_ != nullptr) {
    generator_ptr_->PauseCmd(timeout_, artdaq::TimeUtils::gettimeofday_us());
  }
  paused_ = true;
}

void
dunedaq::driver::FragmentGenerator::do_resume(const std::vector<std::string>& /*args*/)
{
  paused_ = false;
  if (generator_ptr_ != nullptr) {
    generator_ptr_->ResumeCmd(timeout_, artdaq::TimeUtils::gettimeofday_us());
  }
}

void
dunedaq::driver::FragmentGenerator::do_stop(const std::vector<std::string>& /*args*/)
{
  if (generator_ptr_ != nullptr) {
    generator_ptr_->StopCmd(timeout_, artdaq::TimeUtils::gettimeofday_us());
  }
  paused_ = false;
  thread_.stop_working_thread();
}

void
dunedaq::driver::FragmentGenerator::do_unconfigure(const std::vector<std::string>& /*args*/)
{
  generator_ptr_.reset(nullptr);
}

void
dunedaq::driver::FragmentGenerator::do_work(std::atomic<bool>& running_flag)
{
  artdaq::FragmentPtrs frags;

  while (running_flag) {
    if (paused_) {
      TLOG(TLVL_TRACE) << "Generator paused";
      usleep(10000);
      continue;
    }

    while (!output_queue_->can_push()) {
      usleep(10000);
    }

    TLOG(TLVL_TRACE) << "Calling getNext";
    generator_ptr_->getNext(frags);
    TLOG(TLVL_TRACE) << "Done with getNext, frags.size()==" << frags.size();

    if (frags.size() > 0) {
      TLOG(TLVL_TRACE) << "Pushing frags onto queue";
      output_queue_->push(std::move(frags), queue_timeout_);
      TLOG(TLVL_TRACE) << "Done with push";
    }
  }
}
