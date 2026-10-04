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
