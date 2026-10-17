#pragma once
#include <cstdint>
#include <string_view>

namespace pcnet {
enum class ReasonCode : std::uint16_t {
  None = 0,
  UserDisabled = 1,
  DuplicateObservation = 2,
  GeometricOutlier = 3,
  ResidualOutlier = 4,
  DomSupportingEvidence = 5,
  ConnectivityProtection = 6,
  ModelRankProtection = 7,
};
std::string_view reason_code_name(ReasonCode code);
}
