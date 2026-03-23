#include "interfaces.h"

namespace spray {

RunHoldSwitch::RunHoldSwitch(DigitalInputAdapter& input) : input_(input) {}

void RunHoldSwitch::begin() { input_.beginPullup(); }

bool RunHoldSwitch::readRunHold() const { return input_.isActive(); }

}  // namespace spray
