#ifndef _artdaq_json_to_fhicl_hpp_
#define _artdaq_json_to_fhicl_hpp_

#include <fhiclcpp/ParameterSet.h>

#include <nlohmann/json.hpp>

namespace dunedaq {
namespace artdaq {

fhicl::ParameterSet
json_to_fhicl(nlohmann::json json)
{
  fhicl::ParameterSet ps;

  for (auto& [key, value] : json.items()) {
    if (value.is_object()) {
      ps.put(key, json_to_fhicl(value));
    } else {
      ps.put(key, value);
    }
  }

  return ps;
}
}
}

#endif // _artdaq_json_to_fhicl_hpp_