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
  ModelMetrics result; result.model = model;
  double min_time = std::numeric_limits<double>::infinity(), max_time = -std::numeric_limits<double>::infinity();
  for (const auto &measure : view.measures) for (const auto &interval : measure.timing.intervals) { min_time = std::min(min_time, interval.start); max_time = std::max(max_time, interval.end); }
  const bool high_order = model != ErrorModel::Constant; const double span = max_time - min_time;
  Matrix rows;
  for (const auto &measure : view.measures) {
    const auto moments = temporal_moments(measure.timing);
    if (high_order && moments.total_weight == 0.0) { ++result.unavailable_measures; continue; }
    ++result.available_measures;
    double tau = 0.0, tau2 = 0.0;
    if (moments.total_weight > 0.0 && span > 0.0) { tau = 2.0 * (moments.mean - min_time) / span - 1.0; tau2 = (4.0 * (moments.second - 2.0 * min_time * moments.mean + min_time * min_time) / (span * span)) - 4.0 * (moments.mean - min_time) / span + 1.0; }
    if (model == ErrorModel::Constant) rows.push_back({1.0});
    else if (model == ErrorModel::Velocity) rows.push_back({1.0, tau});
    else rows.push_back({1.0, tau, tau2});
  }
  result.high_order_timing_complete = !high_order || result.unavailable_measures == 0;
  const auto g = gram(rows); result.rank = rank_of(rows.empty() ? Matrix{} : rows);
  if (g.empty() || result.rank == 0) { result.condition_number = std::numeric_limits<double>::infinity(); result.max_prediction_variance = std::numeric_limits<double>::infinity(); return result; }
  auto values = eigenvalues(g); double largest = *std::max_element(values.begin(), values.end()); double smallest = *std::min_element(values.begin(), values.end());
  result.condition_number = smallest > 1e-12 ? largest / smallest : std::numeric_limits<double>::infinity();
  result.max_prediction_variance = result.rank < g.size() ? std::numeric_limits<double>::infinity() : 1.0 / std::max(smallest, 1e-12);
  return result;
}
}
