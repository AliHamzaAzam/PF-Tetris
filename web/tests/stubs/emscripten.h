#pragma once
#define EMSCRIPTEN_KEEPALIVE
#define EM_ASM(...) ((void)0)
inline void emscripten_set_main_loop(void (*)(), int, int) {}
