#include "pcnet/core/network_view.hpp"
#include <sstream>
namespace pcnet {
std::string_view reason_code_name(ReasonCode code) {
  switch (code) {
    case ReasonCode::UserDisabled: return "USER_DISABLED";
    case ReasonCode::DuplicateObservation: return "DUPLICATE_OBSERVATION";
    case ReasonCode::GeometricOutlier: return "GEOMETRIC_OUTLIER";
    case ReasonCode::ResidualOutlier: return "RESIDUAL_OUTLIER";
    case ReasonCode::DomSupportingEvidence: return "DOM_SUPPORTING_EVIDENCE";
    case ReasonCode::ConnectivityProtection: return "CONNECTIVITY_PROTECTION";
    case ReasonCode::ModelRankProtection: return "MODEL_RANK_PROTECTION";
    case ReasonCode::None: return "NONE";
  }
  return "RESERVED";
}
std::string NetworkView::validate() const {
  for (std::size_t i = 0; i < images.size(); ++i)
    if (images[i].image_index != i) return "image indices must be contiguous";
  for (std::size_t i = 0; i < points.size(); ++i)
    if (points[i].point_index != i) return "point indices must be contiguous";
  for (const auto &m : measures) {
    if (m.image_index >= images.size()) return "measure image index out of range";
    if (m.point_index >= points.size()) return "measure point index out of range";
    if (m.sample != m.sample || m.line != m.line) return "measure coordinate is NaN";
    for (const auto &interval : m.timing.intervals)
      if (interval.end < interval.start) return "temporal interval is reversed";
  }
  if (!quality_values.empty() && quality_values.size() != measures.size())
    return "quality row count does not match measures";
  for (const auto &row : quality_values)
    if (row.size() != quality_columns.size()) return "quality column count mismatch";
  return {};
}
}
