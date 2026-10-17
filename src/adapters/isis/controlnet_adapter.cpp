#include "pcnet/adapters/isis/controlnet_adapter.hpp"
#include <isis/ControlNet.h>
#include <isis/ControlPoint.h>
#include <isis/ControlMeasure.h>
#include <QList>
#include <QString>
#include <QDateTime>
#include <fstream>
#include <map>
#include <stdexcept>
namespace pcnet {
namespace {
std::uint32_t image_index(std::vector<ImageRecord> &images, const QString &serial) {
  const std::string value = serial.toStdString();
  for (const auto &image : images) if (image.serial_number == value) return image.image_index;
  const auto index = static_cast<std::uint32_t>(images.size()); images.push_back({index, value, value, 0, 0}); return index;
}
}
NetworkView ControlNetAdapter::read(const std::filesystem::path &path, TimingPolicy policy) {
  (void)policy;
  Isis::ControlNet net(QString::fromStdString(path.string())); NetworkView view;
  const auto points = net.GetPoints();
  for (int pi = 0; pi < points.size(); ++pi) {
    const auto *point = points.at(pi); PointRecord record; record.id = point->GetId().toStdString(); record.point_index = static_cast<std::uint32_t>(view.points.size()); record.state_flags = point->IsIgnored() ? 1 : 0; record.measure_begin = view.measures.size();
    const auto measures = point->getMeasures(false);
    for (const auto *measure : measures) { const auto ii = image_index(view.images, measure->GetCubeSerialNumber()); TemporalSupport timing;
      TimingSource source = TimingSource::Unavailable;
      bool degraded = false;
      if (measure->HasDateTime()) {
        const auto date_time = QDateTime::fromString(measure->GetDateTime(), Qt::ISODate);
        if (date_time.isValid()) { const double midpoint = static_cast<double>(date_time.toSecsSinceEpoch()); timing.intervals.push_back({midpoint, midpoint, 1.0}); source = TimingSource::EphemerisMidpoint; degraded = true; }
      }
      view.measures.push_back({ii, record.point_index, measure->GetSample(), measure->GetLine(), timing, 1.0, static_cast<std::uint8_t>(measure->IsIgnored() ? 1 : 0), ReasonCode::None, source, degraded}); }
    record.measure_count = static_cast<std::uint32_t>(view.measures.size() - record.measure_begin); view.points.push_back(record);
  }
  if (auto error = view.validate(); !error.empty()) throw std::runtime_error(error); return view;
}
void ControlNetAdapter::write(const std::filesystem::path &path, const NetworkView &view, WriteMode mode, const std::filesystem::path &audit_sidecar) {
  if (auto error = view.validate(); !error.empty()) throw std::invalid_argument(error);
  Isis::ControlNet net;
  std::ofstream audit(audit_sidecar); if (!audit) throw std::runtime_error("cannot open audit sidecar"); audit << "{\"disabled_measures\":["; bool first = true;
  for (const auto &point : view.points) {
    auto *cp = new Isis::ControlPoint(QString::fromStdString(point.id.empty() ? ("P" + std::to_string(point.point_index)) : point.id));
    if (point.state_flags) cp->SetIgnored(true);
    for (const auto &measure : view.measures) if (measure.point_index == point.point_index) {
      if (mode == WriteMode::Compact && measure.state_flags) continue;
      auto *cm = new Isis::ControlMeasure(); cm->SetCubeSerialNumber(QString::fromStdString(view.images.at(measure.image_index).serial_number)); cm->SetCoordinate(measure.sample, measure.line); if (measure.state_flags) cm->SetIgnored(true); cp->Add(cm);
      if (measure.state_flags) { if (!first) audit << ','; first = false; audit << "{\"point_index\":" << measure.point_index << ",\"image_index\":" << measure.image_index << ",\"reason_code\":" << static_cast<unsigned>(measure.reason_code) << "}"; }
    }
    if (cp->GetNumMeasures() > 0) net.AddPoint(cp); else delete cp;
  }
  audit << "]}\n"; audit.close(); const auto temporary = path.string() + ".tmp"; net.Write(QString::fromStdString(temporary), true); std::error_code ec; std::filesystem::rename(temporary, path, ec); if (ec) { std::filesystem::remove(path, ec); std::filesystem::rename(temporary, path, ec); } if (ec) throw std::runtime_error("cannot atomically replace ControlNet");
}
}
