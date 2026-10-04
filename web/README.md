# Browser build

This browser entry reuses the native game rules through a small SDL2 implementation of the SFML graphics calls used by the game. Native CMake and release workflows continue to use SFML. Browser menus, pause, restart, hard drop and game over are updated once per animation frame by `emscripten_set_main_loop`; the browser never enters a native blocking menu loop.

The source is ready for an external Emscripten SDK. **The checked-in browser build is currently incomplete: `dist-wasm/tetris.js`, `tetris.wasm` and `tetris.data` have not been built.** The landing page reports this explicitly instead of making requests for missing binaries.

## Rebuild

Use an approved Emscripten SDK outside this repository. No SDK is downloaded by this script. SDL2, SDL_image and SDL_ttf Emscripten ports need to be available in the SDK cache, or the approved build environment needs network access to populate that cache. The repository contains PNG textures and an OTF font, but no audio files. The web page generates a short optional drop tone after the user clicks Start game.

```sh
export EMSDK=/path/to/external/emsdk
./web/build-wasm.sh
./web/test.sh
python3 -m http.server 8000 --directory web
```

Open `http://localhost:8000`. Do not open the HTML using a file URL. On a successful build, commit all three generated files in `web/dist-wasm/` and the generated `web/build-info.js`. The build preloads `GameResources` at the same virtual path used by the native game. The build requires Emscripten 3.1.64. That pinned SDK has not yet been validated in this environment.

`./web/test.sh` runs AddressSanitizer and UndefinedBehaviorSanitizer regression checks for landing distance, blocked rotations, bomb-floor impact, line compaction and negative row bounds. It also covers start, movement, soft and hard drop, pause, blur, resume, menu, restart, game over and large frame-delta clamping. It checks shell/JavaScript syntax and compiles the browser entry against test stubs. This does not verify SDL ports, font rasterization, WebGL or WASM output. Run `node web/tests/browser.cjs` against a served `web/` directory to check the landing page in installed Chromium.

## Controls and browser differences

Arrow keys move, rotate and soft drop. Space locks the piece immediately. P pauses or resumes, Escape returns to the menu, and Enter starts, resumes or restarts. Moving keyboard focus away from the canvas or hiding the page pauses the game. Scores are per game and are not persisted. The native high-score/name-entry screen and menu Help screen are not ported; browser controls are documented below the canvas. The page links to the native GitHub releases.

## Static Cloudflare Pages deployment

After a verified local WASM build, Cloudflare Pages can serve `web/` as a static output directory, with no server, database or production API. Keep the generated JS, WASM and data files together under `dist-wasm/`. Serve `.wasm` with `application/wasm`; no threads or SharedArrayBuffer are used, so cross-origin isolation headers are unnecessary. A Cloudflare build can run `web/build-wasm.sh` only if an approved external SDK is already available. Otherwise deploy the prebuilt `web/` directory. No deployment has been performed.

For the exact independent-attempt comparison, validation results and remaining blockers, see [VALIDATION.md](VALIDATION.md). `PF_WEB_URL=http://localhost:8000 node web/tests/shell.cjs` additionally tests shell interactions with a deliberately mocked runtime; it is not evidence of WASM gameplay.
