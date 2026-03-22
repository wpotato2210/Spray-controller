# Arduino Include/Path Audit

Scope audited: `Spray-controller.ino` and all `src/*.cpp` files.

## Include map

- `Arduino.h`
- Expected path: provided by Arduino core/toolchain include paths
- Exists in repo: no (external dependency)
- Suggested fix: ensure target board core is installed in Arduino

IDE/CLI.

- `config.h`
- Expected path: `./config.h` (repo root)
- Exists in repo: yes
- Suggested fix: no change needed.
- `interfaces.h`
- Expected path: `./include/interfaces.h`
- Exists in repo: yes
- Suggested fix: compile with `-Iinclude` in non-Arduino toolchains.
- `pins.h`
- Expected path: `./include/pins.h`
- Exists in repo: yes
- Suggested fix: compile with `-Iinclude` in non-Arduino toolchains.
- `protocol.h`
- Expected path: `./include/protocol.h`
- Exists in repo: yes
- Suggested fix: compile with `-Iinclude` in non-Arduino toolchains.

## Findings

- Project-local includes are now normalized to plain header names:

`interfaces.h`, `pins.h`, and `protocol.h`.

- The include directory must be on the compiler search path

(`-Iinclude`) for non-Arduino toolchains.

- CI compile checks run through `ci/compile-sketch.sh`, which injects

`-I$GITHUB_WORKSPACE/include` for both C and C++ compilation.

## Portability check

- GitHub Actions `Arduino CI` compiles the same source for both:
- `arduino:avr:nano`
- `arduino:avr:uno`

This verifies include portability without relying on `include/...` path hacks.
