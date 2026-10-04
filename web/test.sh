#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
build="$(mktemp -d)"
trap 'rm -rf "$build"' EXIT
flags=(-std=c++20 -DPF_HEADLESS_TEST -Iweb/compat -Wall -Wextra -Werror -g -fsanitize=address,undefined)
g++ "${flags[@]}" web/tests/rules.cpp -o "$build/rules"
for test in ghost rotation bomb rows negative; do UBSAN_OPTIONS=halt_on_error=1 "$build/rules" "$test"; done
g++ "${flags[@]}" web/tests/lifecycle.cpp -o "$build/lifecycle"
UBSAN_OPTIONS=halt_on_error=1 "$build/lifecycle"
node --check web/shell.js
bash -n web/build-wasm.sh
g++ -std=c++20 -DPF_HEADLESS_TEST -Iweb/compat -Iweb/tests/stubs -Wall -Wextra -Werror -fsyntax-only web/main.cpp
