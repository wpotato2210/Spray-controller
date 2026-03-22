# Arduino include-path audit

Scope audited:
- `Spray-controller.ino`
- `src/*.cpp`

| File in code | Expected path | Exists? | Suggested fix |
|---|---|---|---|
| `#include "config.h"` (`Spray-controller.ino`, `src/*.cpp`) | `config.h` (repo root) | Yes | No change needed. |
| `#include "include/interfaces.h"` (`Spray-controller.ino`, `src/*.cpp`) | `include/interfaces.h` | Yes | No change needed. |
| `#include "include/pins.h"` (`Spray-controller.ino`, `src/main.cpp`) | `include/pins.h` | Yes | No change needed. |
| `#include "include/protocol.h"` (`Spray-controller.ino`, `src/main.cpp`) | `include/protocol.h` | Yes | No change needed. |
| `#include <Arduino.h>` (`.ino`/`.cpp`) | Arduino core include path from installed board package | Not in repo (expected) | Install/select correct Arduino board core in IDE/CLI. |

## Result
- No missing local headers were found in main `.ino`/`.cpp` files.
- No local include-path mismatches remain after normalizing `src/*.cpp` includes to `include/<header>.h`.
