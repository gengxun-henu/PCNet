#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace pcnet {

class TemporalSupport {
 public:
  struct Segment {
    double start;
    double end;
    double weight;
  };

  void add_segment(double start, double end, double weight = 1.0) {
    if (!std::isfinite(start) || !std::isfinite(end) || !std::isfinite(weight) ||
        end <= start || weight < 0.0) {
      throw std::invalid_argument("TemporalSupport segment must be finite, non-empty, and non-negative");
    }
    segments_.push_back({start, end, weight});
  }

  std::size_t segment_count() const { return segments_.size(); }

  double total_duration() const {
    double result = 0.0;
    for (const Segment &segment : segments_) {
      result += segment.weight * (segment.end - segment.start);
    }
    return result;
  }

  double first_moment() const {
    double result = 0.0;
    for (const Segment &segment : segments_) {
      result += segment.weight *
                (segment.end * segment.end - segment.start * segment.start) / 2.0;
    }
    return result;
  }

  double second_moment() const {
    double result = 0.0;
    for (const Segment &segment : segments_) {
      result += segment.weight *
                (segment.end * segment.end * segment.end -
                 segment.start * segment.start * segment.start) / 3.0;
    }
    return result;
  }

 private:
  std::vector<Segment> segments_;
};

}  // namespace pcnet
