#ifndef SPRAY_CONTROLLER_NUMERIC_UTILS_H
#define SPRAY_CONTROLLER_NUMERIC_UTILS_H

#include <math.h>

namespace spray {

inline float sanitizeNonNegativeFinite(float value) {
  if (!isfinite(value) || value < 0.0f) {
    return 0.0f;
  }
  return value;
}

inline float clampMinMax(float value, float min_value, float max_value) {
  if (value < min_value) {
    return min_value;
  }
  if (value > max_value) {
    return max_value;
  }
  return value;
}

inline float clampMax(float value, float max_value) {
  if (value > max_value) {
    return max_value;
  }
  return value;
}

}  // namespace spray

#endif  // SPRAY_CONTROLLER_NUMERIC_UTILS_H
