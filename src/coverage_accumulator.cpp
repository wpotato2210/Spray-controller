#include "interfaces.h"

#include <math.h>

namespace spray {
namespace {
constexpr float kMetersPerKm = 1000.0f;
constexpr float kSecondsPerHour = 3600.0f;
constexpr float kSquareMetersPerHectare = 10000.0f;
constexpr float kMaxDistanceMeters = 1000000000.0f;
constexpr float kMaxAreaHectares = 1000000.0f;

float sanitizeNonNegativeFinite(float value) {
  if (!isfinite(value) || value < 0.0f) {
    return 0.0f;
  }
  return value;
}

float clamp(float value, float max_value) {
  if (value > max_value) {
    return max_value;
  }
  return value;
}
}  // namespace

CoverageAccumulator::CoverageAccumulator() : distance_m_(0.0f), area_ha_(0.0f) {}

void CoverageAccumulator::update(float speed_kmh, float active_width_m, uint32_t elapsed_ms) {
  speed_kmh = sanitizeNonNegativeFinite(speed_kmh);
  active_width_m = sanitizeNonNegativeFinite(active_width_m);
  if (elapsed_ms == 0U || speed_kmh <= 0.0f) {
    return;
  }

  const float elapsed_s = static_cast<float>(elapsed_ms) / 1000.0f;
  const float speed_mps = (speed_kmh * kMetersPerKm) / kSecondsPerHour;
  const float delta_distance_m = speed_mps * elapsed_s;

  distance_m_ = clamp(distance_m_ + delta_distance_m, kMaxDistanceMeters);

  if (active_width_m <= 0.0f) {
    return;
  }

  const float delta_area_ha = (delta_distance_m * active_width_m) / kSquareMetersPerHectare;
  area_ha_ = clamp(area_ha_ + delta_area_ha, kMaxAreaHectares);
}

void CoverageAccumulator::reset() {
  distance_m_ = 0.0f;
  area_ha_ = 0.0f;
}

float CoverageAccumulator::getDistanceMeters() const { return distance_m_; }

float CoverageAccumulator::getAreaHectares() const { return area_ha_; }

}  // namespace spray
