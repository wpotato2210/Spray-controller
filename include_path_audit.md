# Arduino Include/Path Audit

Scope audited: `Spray-controller.ino` and all `src/*.cpp` files.

| File in code | Expected path | Exists? | Suggested fix |
| --- | --- | --- | --- |
| `Arduino.h` | Provided by Arduino core/toolchain include paths | ⚠️ External dependency (not in repo) | Ensure target board core is installed in Arduino IDE/CLI. |
| `config.h` | `./config.h` (repo root) | ✅ Yes | No change needed. |
| `include/interfaces.h` | `./include/interfaces.h` | ✅ Yes | No change needed. |
| `include/pins.h` | `./include/pins.h` | ✅ Yes | No change needed. |
| `include/protocol.h` | `./include/protocol.h` | ✅ Yes | No change needed. |

## Findings

- No missing project-local headers were found in the audited `.ino` / `.cpp` sources.
- `config.h` is present at the repository root as expected.
- Include paths currently compile as long as the sketch root is on the compiler include path (standard Arduino behavior).

## Optional hardening (not required)

- To reduce toolchain/path sensitivity across non-Arduino build systems, you may normalize includes to either:
  - `#include "interfaces.h"`, `#include "pins.h"`, `#include "protocol.h"` and pass `-Iinclude`, or
  - keep current includes and explicitly ensure project root is always included.
