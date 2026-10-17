#pragma once
#include "pcnet/core/network_view.hpp"
#include <cstdint>
#include <string>
#include <vector>

namespace pcnet {
struct ObservationEvidence { bool has_geometry = false; double geometry_score = 0.0; bool has_residual = false; double residual_magnitude = 0.0; bool has_dom = false; double dom_distance = 0.0; };
using FilterEvidence = std::vector<ObservationEvidence>;
struct FilterConfig { double geometry_threshold = 0.0; double residual_threshold = 0.0; double dom_support_threshold = 0.0; std::uint32_t max_iterations = 3; };
struct FilterAudit { std::uint32_t measure_index = 0; ReasonCode reason_code = ReasonCode::None; std::string stage; double value = 0.0; std::uint32_t iteration = 0; };
struct FilterResult { NetworkView output; std::vector<FilterAudit> audit; std::vector<std::uint32_t> unresolved; };
FilterResult filter_outliers(const NetworkView &, const FilterEvidence &, const FilterConfig &);
}
