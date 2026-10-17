#include "pcnet/core/storage.hpp"
#include <fstream>
#include <stdexcept>
namespace pcnet {
namespace {
template <typename T> void put(std::ostream &out, const T &value) { out.write(reinterpret_cast<const char *>(&value), sizeof(T)); }
template <typename T> T get(std::istream &in) { T value{}; in.read(reinterpret_cast<char *>(&value), sizeof(T)); if (!in) throw std::runtime_error("truncated PCNet storage"); return value; }
void put_string(std::ostream &out, const std::string &value) { std::uint32_t n = static_cast<std::uint32_t>(value.size()); put(out, n); out.write(value.data(), n); }
std::string get_string(std::istream &in) { auto n = get<std::uint32_t>(in); std::string value(n, '\0'); in.read(value.data(), n); if (!in) throw std::runtime_error("truncated PCNet string"); return value; }
}
void StorageWriter::write(const std::filesystem::path &path, const NetworkView &view, StorageOptions options) {
  if (auto error = view.validate(); !error.empty()) throw std::invalid_argument(error);
  std::ofstream out(path, std::ios::binary | std::ios::trunc); if (!out) throw std::runtime_error("cannot open PCNet storage");
  out.write("PCNT", 4); put<std::uint16_t>(out, 1); put<std::uint8_t>(out, options.include_quality ? 1 : 0);
  put<std::uint32_t>(out, view.images.size()); put<std::uint32_t>(out, view.points.size()); put<std::uint32_t>(out, view.measures.size());
  for (const auto &image : view.images) { put(out, image.image_index); put_string(out, image.serial_number); put_string(out, image.name); put(out, image.width); put(out, image.height); }
  for (const auto &point : view.points) { put(out, point.point_index); put(out, point.state_flags); put(out, point.measure_begin); put(out, point.measure_count); }
  for (const auto &measure : view.measures) {
    put(out, measure.image_index); put(out, measure.point_index); put(out, measure.sample); put(out, measure.line); put(out, measure.quality_weight); put(out, measure.state_flags);
    put<std::uint16_t>(out, static_cast<std::uint16_t>(measure.reason_code));
    put<std::uint32_t>(out, measure.timing.intervals.size());
    for (const auto &interval : measure.timing.intervals) { put(out, interval.start); put(out, interval.end); put(out, interval.weight); }
  }
  if (options.include_quality) {
    put<std::uint32_t>(out, view.quality_columns.size()); for (const auto &column : view.quality_columns) put_string(out, column);
    for (const auto &row : view.quality_values) for (double value : row) put(out, value);
  }
}
NetworkView StorageReader::read(const std::filesystem::path &path) {
  std::ifstream in(path, std::ios::binary); if (!in) throw std::runtime_error("cannot open PCNet storage");
  char magic[4]; in.read(magic, 4); if (std::string(magic, 4) != "PCNT") throw std::runtime_error("invalid PCNet storage magic");
  if (get<std::uint16_t>(in) != 1) throw std::runtime_error("unsupported PCNet storage version");
  const bool has_quality = get<std::uint8_t>(in) != 0; NetworkView view;
  auto ni = get<std::uint32_t>(in), np = get<std::uint32_t>(in), nm = get<std::uint32_t>(in);
  view.images.resize(ni); view.points.resize(np); view.measures.resize(nm);
  for (auto &image : view.images) { image.image_index = get<std::uint32_t>(in); image.serial_number = get_string(in); image.name = get_string(in); image.width = get<std::uint32_t>(in); image.height = get<std::uint32_t>(in); }
  for (auto &point : view.points) { point.point_index = get<std::uint32_t>(in); point.state_flags = get<std::uint8_t>(in); point.measure_begin = get<std::uint64_t>(in); point.measure_count = get<std::uint32_t>(in); }
  for (auto &measure : view.measures) { measure.image_index = get<std::uint32_t>(in); measure.point_index = get<std::uint32_t>(in); measure.sample = get<double>(in); measure.line = get<double>(in); measure.quality_weight = get<double>(in); measure.state_flags = get<std::uint8_t>(in); measure.reason_code = static_cast<ReasonCode>(get<std::uint16_t>(in)); auto count = get<std::uint32_t>(in); measure.timing.intervals.resize(count); for (auto &interval : measure.timing.intervals) { interval.start = get<double>(in); interval.end = get<double>(in); interval.weight = get<double>(in); } }
  if (has_quality) { auto nc = get<std::uint32_t>(in); view.quality_columns.reserve(nc); for (std::uint32_t i = 0; i < nc; ++i) view.quality_columns.push_back(get_string(in)); view.quality_values.assign(nm, std::vector<double>(nc)); for (auto &row : view.quality_values) for (auto &value : row) value = get<double>(in); }
  if (auto error = view.validate(); !error.empty()) throw std::runtime_error(error);
  return view;
}
}
