#include "interfaces.h"

#include "config.h"

namespace spray {

SectionManager::SectionManager() : sections_{} {
  for (uint8_t i = 0U; i < SECTION_COUNT; ++i) {
    sections_[i] = false;
  }
}

void SectionManager::setSection(uint8_t index, bool state) {
  if (index >= SECTION_COUNT) {
    return;
  }
  sections_[index] = state;
}

bool SectionManager::getSection(uint8_t index) const {
  if (index >= SECTION_COUNT) {
    return false;
  }
  return sections_[index];
}

uint8_t SectionManager::getActiveCount() const {
  uint8_t count = 0U;
  for (uint8_t i = 0U; i < SECTION_COUNT; ++i) {
    if (sections_[i]) {
      ++count;
    }
  }
  return count;
}

float SectionManager::getActiveWidth() const {
  return static_cast<float>(getActiveCount()) * SECTION_WIDTH_M;
}

}  // namespace spray
