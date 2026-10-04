# C08 validation and remaining work

This is an explicitly partial result based on main commit `92f5eb254958556e3e262cdbbae08dc5637ee805`. All work and four independent attempts ran in the cloud environment. No software was installed. Native CMake configuration and `.github/workflows` are unchanged.

## Independent attempts

All four started from the same commit in separate detached worktrees, with internal workers running concurrently. Only attempt 4 was incorporated into `dot/wasm-web`, followed by review fixes.

| Attempt | Approach | Evidence and selection |
| --- | --- | --- |
| 1 | Canvas graphics facade with original C++ rules | `bash web/test.sh` passed. Not selected because Canvas differs from the requested SDL2/WebGL route and menu parity was incomplete. Two regression tests were added after their fixes, contrary to the requested failing-first workflow. |
| 2 | Separate C++ game model and SDL2 renderer | `g++ -std=c++17 -Wall -Wextra -Werror -fsanitize=address,undefined -g web/tests/game_test.cpp -o /tmp/c08-attempt2-tests && /tmp/c08-attempt2-tests` passed, including randomized games. Not selected because it duplicates the native game model. |
| 3 | SDL2 SFML facade and explicit browser states | `g++ -std=c++17 -Wall -Wextra -pedantic web/tests/flow.cpp -o /tmp/c08-attempt3-flow && /tmp/c08-attempt3-flow` passed. Not selected because it retained unsafe native rule paths and lacked a browser shell. |
| 4 | SDL2 SFML facade, existing rule functions, frame-based browser lifecycle | Selected for direct rule reuse, tested safety fixes, browser shell and explicit build failure handling. |

## Review rounds

Five baseline regressions failed before the selected fixes: ghost obstruction, rotation into occupied cells, bomb impact at row 20, top-row compaction, and negative row access. These were independently reproduced against unchanged main with UBSan/bounds; the fixed versions pass.

Review added a failing regression for checking a new spawn before clearing completed rows, then corrected that ordering. Browser hard drop locks and spawns immediately rather than leaving a placed piece movable. Follow-up review fixed a `clock` name collision, guarded failed text texture creation, scoped SDL keyboard input to the canvas, made the build marker false before rebuild attempts, and enabled strict warning checks. SDL keyboard scoping still needs real WASM verification.

## Commands and results

Run from the repository root:

- `bash web/test.sh`: PASS. Five ASan/UBSan rule cases, browser lifecycle tests, strict C++ warning checks, browser entry syntax with a test Emscripten header and headless graphics facade, JavaScript syntax, and build-script syntax.
- `python3 -m http.server 8008 --directory web`: served the checked-in web directory locally.
- `PF_WEB_URL=http://localhost:8008 node web/tests/browser.cjs`: PASS for the missing-artifact page, keyboard focus, and zero console/page errors. This did not render WASM.
- `PF_WEB_URL=http://localhost:8008 node web/tests/shell.cjs`: PASS for start gating, focus, audio key path, blur pause, releases link and load failure, using an explicitly mocked runtime. This did not exercise SDL or WASM.
- `bash web/build-wasm.sh`: BLOCKED, exit 1: `em++` missing.
- `cmake -S . -B /tmp/c08-native-build`: BLOCKED, exit 127: `cmake` missing. Native SFML headers/libraries are also absent.
- `git diff --check`: PASS.

The repository has no separate configured type checker or existing automated test suite. The new test suite above runs in full, but headless C++ checks exclude the SDL rendering path. Native compilation and the actual browser toolchain remain unverified.

## Unverified items

- Actual SDL2, SDL_image and SDL_ttf compilation and Emscripten linking.
- Font rasterization, WebGL rendering, real keyboard events, timing and gameplay in Chrome.
- Reproducibility of the pinned Emscripten 3.1.64 build.
- Generated `web/dist-wasm/tetris.js`, `tetris.wasm`, and `tetris.data` are absent. No placeholder binaries were added.
- Native platform builds after shared rule fixes.
- Native high-score/name-entry and Help menu parity. Browser controls are shown below the canvas; scores last only for the current game.

## Needs the founder

Authorize or provide Emscripten 3.1.64 outside the repository, its SDL2/SDL_image/SDL_ttf ports, and CMake 3.28 or newer plus native SFML build dependencies in this cloud environment. Then rebuild, test real WASM rendering and gameplay, run native builds, and commit the generated artifacts before considering this playable or deployable. No SDK, credentials, deployment, package publication, production API writes, or real notifications were used.

## Expanded SDK discovery and gameplay review

A second path/name-only search covered the readable filesystem from `/`, pruning `/proc`, `/sys` and `/dev`, for files and symlinks named `emcc`, `em++`, `emsdk`, `emsdk_env.sh`, `.emscripten`, `cmake`, and `AGENTS.md`. Targeted directory discovery additionally covered `/workspace`, `/tmp`, `/root`, `/home`, `/opt`, `/usr/local`, `/usr/lib`, and `/usr/share` for SDK/toolchain names. No executable, SDK directory, configuration file, or saved AGENTS setup instruction was found. Permission-denied directories were not inspected. `/workspace/.agents` and `/workspace/.codex` are empty. `EMSDK`, `EM_CONFIG`, `EM_CACHE`, and `CMAKE_PREFIX_PATH` are unset. No credential variables or secret contents were read. `pkg-config --exists sfml-graphics` and `pkg-config --exists sdl2` both returned 1.

Further parity review added failing tests for native top-out across any top-row column and repeated discrete key actions, then fixed both. Left/right/down may repeat; pause, rotate, hard drop, Enter and Escape fire once per press. Expanded ASan/UBSan tests exercise all seven original shapes, lock/spawn behavior, scoring 10/30/60/100, level crossings 4 to 6 and 9 to 11, minimum delay, matching and mismatched bomb effects, floor impacts including color zero, frozen pause timers and 100 randomized games. These C++ tests validate gameplay logic, not browser rendering or the compiled SDL event path.

### Minimal browser toolchain approval bundle

The following official SDK commands have **not** been executed. They install outside the repository and do not require global credentials or system packages:

```sh
mkdir -p /workspace/c08-tools
git clone --depth 1 --branch 3.1.64 https://github.com/emscripten-core/emsdk.git /workspace/c08-tools/emsdk
/workspace/c08-tools/emsdk/emsdk install 3.1.64
/workspace/c08-tools/emsdk/emsdk activate 3.1.64
source /workspace/c08-tools/emsdk/emsdk_env.sh
cd /workspace/PF-Tetris
./web/build-wasm.sh
```

Verified official emsdk tag `3.1.64` resolves to `0b3bcbc3b005cbb811d48e497eabcc6846d43001`; its SDK release is `fd61bacaf40131f74987e649a135f1dd559aff60`. Installation downloads the Emscripten/LLVM/Binaryen toolchain and bundled Node. The first build downloads the SDK-pinned ports: SDL 2.28.4, SDL_image 2.6.0, SDL_ttf 2.20.2, FreeType `version_1`, HarfBuzz 3.2.0, libpng 1.6.39 and zlib 1.2.13. Required network origins are GitHub and its archive/release redirect hosts, plus `storage.googleapis.com` for SDK/Node/libpng archives. Port recipes include SHA-512 checks. No separate native SDL installation is needed for this browser build.

Official references: [SDK installation](https://emscripten.org/docs/getting_started/downloads.html), [pinned SDK manifest](https://github.com/emscripten-core/emsdk/blob/3.1.64/emsdk_manifest.json), and [pinned port recipes](https://github.com/emscripten-core/emscripten/tree/3.1.64/tools/ports).

Native validation separately needs CMake 3.28 or newer and the existing workflow's Linux packages: `libxrandr-dev libxcursor-dev libxi-dev libudev-dev libgl1-mesa-dev libfreetype-dev xvfb`. CMake then fetches SFML 2.6.2 from its official GitHub repository. None were installed.
