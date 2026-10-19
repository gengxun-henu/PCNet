#include "pcnet/core/error_model.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
namespace pcnet {
TemporalMoments temporal_moments(const TemporalSupport &support) {
  TemporalMoments result;
  for (const auto &interval : support.intervals) {
    if (interval.end < interval.start || interval.weight < 0.0) throw std::invalid_argument("invalid temporal interval");
    const double length = interval.end - interval.start;
    if (length == 0.0) { result.total_weight += interval.weight; result.mean += interval.start * interval.weight; result.second += interval.start * interval.start * interval.weight; continue; }
    const double weighted_length = length * interval.weight;
    result.total_weight += weighted_length;
    result.mean += 0.5 * (interval.end * interval.end - interval.start * interval.start) * interval.weight;
    result.second += (interval.end * interval.end * interval.end - interval.start * interval.start * interval.start) * interval.weight / 3.0;
  }
  if (result.total_weight > 0.0) { result.mean /= result.total_weight; result.second /= result.total_weight; }
  return result;
}
bool validate_time_bins(std::uint32_t bins) { return bins >= 4 && bins <= 32; }
NormalizedTiming normalize_support(const TemporalSupport &support, double envelope_start, double envelope_end, bool degraded) {
  NormalizedTiming result; result.degraded = degraded; result.envelope_start = envelope_start; result.envelope_end = envelope_end;
  if (support.intervals.empty()) return result;
  const double span = envelope_end - envelope_start;
  for (const auto &interval : support.intervals) {
    if (interval.end < interval.start || interval.weight < 0.0) throw std::invalid_argument("invalid temporal interval");
    if (span > 0.0) result.support.intervals.push_back({2.0 * (interval.start - envelope_start) / span - 1.0, 2.0 * (interval.end - envelope_start) / span - 1.0, interval.weight});
    else result.support.intervals.push_back({0.0, 0.0, interval.weight});
  }
  result.available = true; return result;
}
std::vector<NormalizedTiming> normalize_view_timing(const NetworkView &view, bool exclude_disabled) {
  std::vector<double> starts(view.images.size(), std::numeric_limits<double>::infinity()), ends(view.images.size(), -std::numeric_limits<double>::infinity());
  for (const auto &measure : view.measures) {
    if (exclude_disabled && (measure.state_flags & kStateDisabled)) continue;
    if (measure.image_index >= view.images.size()) continue;
    for (const auto &interval : measure.timing.intervals) { starts[measure.image_index] = std::min(starts[measure.image_index], interval.start); ends[measure.image_index] = std::max(ends[measure.image_index], interval.end); }
  }
  std::vector<NormalizedTiming> result(view.measures.size());
  for (std::size_t i = 0; i < view.measures.size(); ++i) {
    const auto &measure = view.measures[i]; if (exclude_disabled && (measure.state_flags & kStateDisabled)) continue;
    if (measure.image_index >= view.images.size() || !std::isfinite(starts[measure.image_index])) continue;
    result[i] = normalize_support(measure.timing, starts[measure.image_index], ends[measure.image_index], measure.timing_degraded);
  }
  return result;
}

std::string error_model_name(ErrorModel model) { switch (model) { case ErrorModel::Constant: return "constant"; case ErrorModel::Velocity: return "velocity"; case ErrorModel::Acceleration: return "acceleration"; } return "unknown"; }
namespace {
using Matrix = std::vector<std::vector<double>>;
std::size_t rank_of(Matrix a) {
  const std::size_t rows = a.size(), cols = rows ? a[0].size() : 0; std::size_t rank = 0; const double eps = 1e-10;
  for (std::size_t col = 0; col < cols && rank < rows; ++col) {
    std::size_t pivot = rank; for (std::size_t r = rank + 1; r < rows; ++r) if (std::abs(a[r][col]) > std::abs(a[pivot][col])) pivot = r;
    if (std::abs(a[pivot][col]) <= eps) continue; std::swap(a[pivot], a[rank]);
    for (std::size_t r = rank + 1; r < rows; ++r) { const double factor = a[r][col] / a[rank][col]; for (std::size_t c = col; c < cols; ++c) a[r][c] -= factor * a[rank][c]; }
    ++rank;
  }
  return rank;
}
std::vector<double> eigenvalues(Matrix a) {
  const std::size_t n = a.size();
  for (int iteration = 0; iteration < 100 * static_cast<int>(n + 1); ++iteration) {
    std::size_t p = 0, q = 0; double largest = 0.0;
    for (std::size_t i = 0; i < n; ++i) for (std::size_t j = i + 1; j < n; ++j) if (std::abs(a[i][j]) > largest) { largest = std::abs(a[i][j]); p = i; q = j; }
    if (largest < 1e-12) break;
    const double angle = 0.5 * std::atan2(2.0 * a[p][q], a[p][p] - a[q][q]); const double c = std::cos(angle), s = std::sin(angle);
    for (std::size_t k = 0; k < n; ++k) { const double apk = a[p][k], aqk = a[q][k]; a[p][k] = c * apk + s * aqk; a[q][k] = -s * apk + c * aqk; }
    for (std::size_t k = 0; k < n; ++k) { const double akp = a[k][p], akq = a[k][q]; a[k][p] = c * akp + s * akq; a[k][q] = -s * akp + c * akq; }
  }
  std::vector<double> values(n); for (std::size_t i = 0; i < n; ++i) values[i] = a[i][i]; return values;
}
Matrix gram(const Matrix &rows) { const std::size_t n = rows.empty() ? 0 : rows[0].size(); Matrix g(n, std::vector<double>(n)); for (const auto &row : rows) for (std::size_t i = 0; i < n; ++i) for (std::size_t j = 0; j < n; ++j) g[i][j] += row[i] * row[j]; return g; }
}
ModelMetrics analyze_model(const NetworkView &view, ErrorModel model, const ModelConfig &config) {
  if (!validate_time_bins(config.time_bins)) throw std::invalid_argument("time_bins must be between 4 and 32");
  ModelMetrics result; result.model = model; const bool high_order = model != ErrorModel::Constant;
  const auto normalized = normalize_view_timing(view);
  Matrix rows;
  for (std::size_t index = 0; index < view.measures.size(); ++index) {
    const auto &measure = view.measures[index]; if (measure.state_flags & kStateDisabled) continue;
    if (high_order && !normalized[index].available) { ++result.unavailable_measures; continue; }
    ++result.available_measures;
    double tau = 0.0, tau2 = 0.0;
    if (high_order) { const auto moments = temporal_moments(normalized[index].support); tau = moments.mean; tau2 = moments.second; }
    if (model == ErrorModel::Constant) rows.push_back({1.0});
    else if (model == ErrorModel::Velocity) rows.push_back({1.0, tau});
    else rows.push_back({1.0, tau, tau2});
  }
  result.high_order_timing_complete = !high_order || result.unavailable_measures == 0;
  const auto g = gram(rows); result.rank = rank_of(rows);
  if (g.empty() || result.rank == 0) { result.condition_number = std::numeric_limits<double>::infinity(); result.max_prediction_variance = std::numeric_limits<double>::infinity(); return result; }
  auto values = eigenvalues(g); double largest = *std::max_element(values.begin(), values.end()); double smallest = *std::min_element(values.begin(), values.end());
  result.condition_number = smallest > 1e-12 ? largest / smallest : std::numeric_limits<double>::infinity();
  result.max_prediction_variance = result.rank < g.size() ? std::numeric_limits<double>::infinity() : 1.0 / std::max(smallest, 1e-12);
  return result;
}
}
