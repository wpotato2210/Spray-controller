# Arduino Include/Path Audit

Scope audited: `Spray-controller.ino` and all `src/*.cpp` files.

| File in code | Expected path | Exists? | Suggested fix |
| --- | --- | --- | --- |
| `Arduino.h` | Provided by Arduino core/toolchain include paths | ⚠️ External dependency (not in repo) | Ensure target board core is installed in Arduino IDE/CLI. |
| `config.h` | `./config.h` (repo root) | ✅ Yes | No change needed. |
| `interfaces.h` | `./include/interfaces.h` | ✅ Yes | Compile with `-Iinclude` in non-Arduino toolchains. |
| `pins.h` | `./include/pins.h` | ✅ Yes | Compile with `-Iinclude` in non-Arduino toolchains. |
| `protocol.h` | `./include/protocol.h` | ✅ Yes | Compile with `-Iinclude` in non-Arduino toolchains. |

## Findings

- Project-local includes are now normalized to plain header names: `interfaces.h`, `pins.h`, `protocol.h`.
- The include directory must be on the compiler search path (`-Iinclude`) for non-Arduino toolchains.
- CI compile checks run through `ci/compile-sketch.sh`, which injects `-I$GITHUB_WORKSPACE/include` for both C and C++ compilation.

## Portability check

- GitHub Actions `Arduino CI` compiles the same source for both:
  - `arduino:avr:nano`
  - `arduino:avr:uno`

This verifies include portability without relying on `include/...` path hacks.
