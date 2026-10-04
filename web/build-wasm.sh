#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
if [[ -n "${EMSDK:-}" && -f "$EMSDK/emsdk_env.sh" ]]; then
  case "$(cd -- "$EMSDK" && pwd)" in "$root"|"$root"/*) echo 'Keep emsdk outside the repository.' >&2; exit 1;; esac
  source "$EMSDK/emsdk_env.sh"
fi
printf '%s\n' 'window.PF_WASM_BUILT = false;' > "$root/web/build-info.js"
if ! command -v em++ >/dev/null; then
  echo 'Build blocked: em++ missing. Activate an approved external emsdk installation.' >&2
  exit 1
fi
if ! em++ --version | head -n 1 | grep -F '3.1.64' >/dev/null; then
  echo 'Build requires the pinned Emscripten 3.1.64 SDK outside this repository.' >&2
  exit 1
fi
cd "$root"
mkdir -p web/dist-wasm
em++ web/main.cpp -std=c++20 -O2 -Iweb/compat \
  -sUSE_SDL=2 -sUSE_SDL_IMAGE=2 -sSDL2_IMAGE_FORMATS='["png"]' -sUSE_SDL_TTF=2 \
  -sALLOW_MEMORY_GROWTH=1 -sENVIRONMENT=web -sASSERTIONS=1 \
  -sEXPORTED_FUNCTIONS='["_main","_start_game","_pause_game"]' \
  --preload-file GameResources@GameResources -o web/dist-wasm/tetris.js
printf '%s\n' 'window.PF_WASM_BUILT = true;' > web/build-info.js
