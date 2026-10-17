#include "pcnet/metrics/quality.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <sstream>
#include <unordered_map>
namespace pcnet {
namespace {
struct ImageAccum { std::set<std::uint32_t> points; std::vector<const MeasureRecord *> measures; std::set<std::uint32_t> neighbors; };
std::string quote(const std::string &value) { std::string out = "\""; for (char c : value) { if (c == '"') out += "\\\""; else out += c; } return out + "\""; }
double measure_span(std::vector<double> values) { if (values.size() < 2) return 0.0; auto [lo, hi] = std::minmax_element(values.begin(), values.end()); return *hi - *lo; }
double time_coverage(const std::vector<const MeasureRecord *> &measures, double *gap) {
  std::vector<std::pair<double,double>> intervals; for (auto *m : measures) for (const auto &i : m->timing.intervals) intervals.emplace_back(i.start, i.end);
  if (intervals.empty()) { *gap = 0.0; return 0.0; } std::sort(intervals.begin(), intervals.end()); double total = 0.0, cursor = intervals.front().first, max_gap = 0.0, lo = cursor, hi = cursor;
  for (const auto &[start,end] : intervals) { hi = std::max(hi, end); if (start > cursor) max_gap = std::max(max_gap, start - cursor); if (end > cursor) { total += end - std::max(cursor, start); cursor = std::max(cursor, end); } }
  *gap = max_gap; return hi > lo ? total / (hi - lo) : 0.0;
}
}
QualityReport evaluate_quality(const NetworkView &view, const QualityConfig &config) {
  if (!validate_time_bins(config.time_bins)) throw std::invalid_argument("time_bins must be between 4 and 32");
  QualityReport report; report.metadata.time_bins = config.time_bins; report.metadata.denominator = "valid exposure interval union; valid independent observations"; report.metadata.coordinate_rule = "sample range per atomic image; ground overlap unavailable in core";
  std::vector<ImageAccum> accum(view.images.size());
  for (const auto &m : view.measures) if (m.image_index < accum.size()) { accum[m.image_index].points.insert(m.point_index); accum[m.image_index].measures.push_back(&m); }
  for (const auto &point : view.points) { std::set<std::uint32_t> images; for (const auto &m : view.measures) if (m.point_index == point.point_index && m.image_index < view.images.size()) images.insert(m.image_index); for (auto a : images) for (auto b : images) if (a != b) accum[a].neighbors.insert(b); }
  for (std::uint32_t i = 0; i < view.images.size(); ++i) { auto &a = accum[i]; ImageQuality q; q.image_index = i; q.n_points = a.points.size(); q.n_measures = a.measures.size(); q.n_neighbors = a.neighbors.size(); std::vector<double> samples; for (auto *m : a.measures) samples.push_back(m->sample); q.cross_track_coverage = samples.empty() ? 0.0 : (samples.size() > 1 ? 1.0 : 0.0); q.time_coverage = time_coverage(a.measures, &q.max_time_gap); q.sparse = q.n_points < config.sparse_point_threshold; if (q.sparse) q.warning_flags.push_back("sparse"); NetworkView unit = view; unit.measures.clear(); for (auto *m : a.measures) unit.measures.push_back(*m); auto model = analyze_model(unit, config.model, ModelConfig{config.time_bins}); q.design_rank = model.rank; q.condition_number = model.condition_number; if (model.unavailable_measures) q.warning_flags.push_back("time_unavailable"); report.images.push_back(std::move(q)); }
  std::map<std::pair<std::uint32_t,std::uint32_t>, std::set<std::uint32_t>> shared;
  for (const auto &point : view.points) { std::set<std::uint32_t> images; for (const auto &m : view.measures) if (m.point_index == point.point_index) images.insert(m.image_index); for (auto a = images.begin(); a != images.end(); ++a) for (auto b = std::next(a); b != images.end(); ++b) shared[{*a,*b}].insert(point.point_index); }
  for (const auto &[key,points] : shared) { PairQuality q; q.image_a = key.first; q.image_b = key.second; q.n_valid_tracks = points.size(); q.n_independent_tracks = points.size(); q.overlap_fallback = true; q.warning_flags.push_back("overlap_fallback"); report.pairs.push_back(std::move(q)); }
  report.network.n_images = view.images.size(); report.network.n_points = view.points.size(); report.network.n_measures = view.measures.size(); report.network.n_components = view.images.empty() ? 0 : 1; for (const auto &q : report.images) if (q.n_points < config.sparse_point_threshold) report.network.weak_images.push_back(q.image_index); for (const auto &point : view.points) { std::uint64_t count = 0; for (const auto &m : view.measures) if (m.point_index == point.point_index) ++count; report.network.point_count_quantiles.push_back(count); } std::sort(report.network.point_count_quantiles.begin(), report.network.point_count_quantiles.end()); return report;
}
void write_quality_csv_json(const QualityReport &report, const std::filesystem::path &directory) {
  std::filesystem::create_directories(directory); std::ofstream image_csv(directory / "image_quality.csv"); image_csv << "image_index,n_points,n_measures,n_neighbors,time_coverage,cross_track_coverage,max_time_gap,design_rank,condition_number,sparse\n"; for (const auto &q : report.images) image_csv << q.image_index << ',' << q.n_points << ',' << q.n_measures << ',' << q.n_neighbors << ',' << q.time_coverage << ',' << q.cross_track_coverage << ',' << q.max_time_gap << ',' << q.design_rank << ',' << q.condition_number << ',' << (q.sparse ? 1 : 0) << '\n';
  std::ofstream json(directory / "network_quality.json"); json << "{\n  \"metadata\": {\"time_bins\": " << report.metadata.time_bins << ", \"denominator\": " << quote(report.metadata.denominator) << "},\n  \"ImageQuality\": [\n"; for (std::size_t i = 0; i < report.images.size(); ++i) { const auto &q = report.images[i]; json << "    {\"image_index\": " << q.image_index << ", \"n_points\": " << q.n_points << ", \"time_coverage\": " << q.time_coverage << "}" << (i + 1 == report.images.size() ? "\n" : ",\n"); } json << "  ],\n  \"PairQuality\": [\n"; for (std::size_t i = 0; i < report.pairs.size(); ++i) { const auto &q = report.pairs[i]; json << "    {\"image_a\": " << q.image_a << ", \"image_b\": " << q.image_b << ", \"overlap_fallback\": " << (q.overlap_fallback ? "true" : "false") << "}" << (i + 1 == report.pairs.size() ? "\n" : ",\n"); } json << "  ],\n  \"NetworkQuality\": {\"n_images\": " << report.network.n_images << ", \"n_points\": " << report.network.n_points << ", \"n_measures\": " << report.network.n_measures << "}\n}\n";
}
}
