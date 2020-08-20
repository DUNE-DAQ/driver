/**
 * @file HDF5Output.cxx HDF5Output class implementation.
 *
 * This is part of the DUNE DAQ Application Framework, copyright 2020.
 * Licensing/copyright details are in the COPYING file that you should have
 * received with this code.
 */

#include "artdaq/HDF5Output.hpp"
#include "artdaq/json_to_fhicl.hpp"
#include "artdaq-demo-hdf5/HDF5/MakeDatasetPlugin.hh"

#include <fhiclcpp/make_ParameterSet.h>

DEFINE_DUNE_DAQ_MODULE(dunedaq::artdaq::HDF5Output)

inline dunedaq::artdaq::HDF5Output::HDF5Output(std::string name)
  : DAQModule(name)
  , thread_(std::bind(&HDF5Output::do_work, this, std::placeholders::_1))
{
  register_command("configure", &HDF5Output::do_configure);
  register_command("start", &HDF5Output::do_start);
  register_command("stop", &HDF5Output::do_stop);
  register_command("unconfigure", &HDF5Output::do_unconfigure);
}

inline void
dunedaq::artdaq::HDF5Output::init()
{
  input_queue_.reset(new appfwk::DAQSource<artdaq::FragmentPtrs>(get_config()["input"].get<std::string>()));
}

void
dunedaq::artdaq::HDF5Output::do_configure(const std::vector<std::string>& /*args*/)
{
  queue_timeout_ = std::chrono::milliseconds(get_config().value<int>("queue_timeout_ms", 100));

  fhicl::ParameterSet dataset_ps, dataset_ps_tmp, pset;
  dataset_ps_tmp = json_to_fhicl(get_config());
  dataset_ps_tmp.put<std::string>("mode", "write");
  if (!dataset_ps_tmp.has_key("datasetPluginType")) {
    dataset_ps_tmp.put<std::string>("datasetPluginType", "highFiveGroupedDataset");
  }

  // Re-run through the parser
  fhicl::make_ParameterSet(dataset_ps_tmp.to_string(), dataset_ps);

  pset.put<fhicl::ParameterSet>("dataset", dataset_ps);

  TLOG(TLVL_TRACE) << "ParameterSet for Dataset Plugin: " << pset.to_string();

  hdf_dataset_ = artdaq::hdf5::MakeDatasetPlugin(pset, "dataset");
}

void
dunedaq::artdaq::HDF5Output::do_start(const std::vector<std::string>& /*args*/)
{
  thread_.start_working_thread();
}

void
dunedaq::artdaq::HDF5Output::do_stop(const std::vector<std::string>& /*args*/)
{
  thread_.stop_working_thread();
}

void
dunedaq::artdaq::HDF5Output::do_unconfigure(const std::vector<std::string>& /*args*/)
{
  hdf_dataset_.reset(nullptr);
}

void
dunedaq::artdaq::HDF5Output::do_work(std::atomic<bool>& running_flag)
{
  artdaq::FragmentPtrs frags;
  while (running_flag) {
    if (input_queue_->can_pop()) {

      try {
        input_queue_->pop(frags, queue_timeout_);
      } catch (const dunedaq::appfwk::QueueTimeoutExpired& excpt) {
        continue;
      }

      if (hdf_dataset_ != nullptr) {
        for (auto& frag : frags) {
          hdf_dataset_->insertOne(*frag);
        }
      }
    }
  }
}
