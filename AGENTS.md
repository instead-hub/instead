# AGENTS.md — INSTEAD

Adventure game interpreter (C + Lua/LuaJIT, SDL3 rendering). The active branch is `master`: it already contains the SDL3 port (the old `SDL3` branch is its ancestor and is no longer developed, the SDL2 version lives in the `SDL2` branch). On `master` `CI.yml` and `appimage.yml` build SDL3 (SDL3/SDL3_ttf/image/mixer are pulled from source at release tags — apt Ubuntu does not have them), `windows.yml` builds SDL3 with mingw (official SDL3/SDL3_image/SDL3_mixer mingw packages + SDL3_ttf from the VC package — its mingw build imports `ScriptFreeCache`/`ScriptPlace` from gdi32.dll and does not start on Windows 7; plus LuaJIT/libxmp from source), `macos.yml` builds SDL3 for macOS (x86_64/arm64, SDL3 from source, `.app` bundle + `.dmg`), `emscripten.yml` builds SDL3 for wasm (emscripten ports `sdl3`/`sdl3_ttf`/`zlib`, SDL3_image/SDL3_mixer/libxmp from source via `emcmake`; `_USE_BROWSE` is disabled on wasm), `docs.yml` builds the PDF manuals (`make -C doc pdf`: xelatex + MultiMarkdown). `INSTALL` describes the SDL3 build (cmake/configure.sh); the source of truth is `config.make` + `Makefile`.

Map: `src/` — engine and rendering (`src/instead/` — Lua VM and engine), `stead/` — the Lua game library, `games/tutorial` — smoke test game, `tests/games/integration` — test game for integration tests, `themes/` — theme data.

## Build

Dependencies: a C toolchain, `pkg-config`, `cmake`/`ninja` (optional), and the development
packages of SDL3, SDL3_ttf, SDL3_image, SDL3_mixer, LuaJIT and zlib (plus the usual
image/audio libraries pulled in by SDL3_image/SDL3_mixer). Install them with the distro
package manager; where `sdl3_image`/`sdl3_mixer` packages are missing, build them from
source against the distro SDL3.

- Dev build: `make -j$(nproc)` from the root. `Makefile` includes `Rules.make` (symlink to `Rules.make.standalone`) and the local untracked `config.make` (SDL3/LuaJIT/zlib flags; ASan optional). The binary is `src/sdl-instead`; run it from the repository root (paths `./stead`, `./games`).
- The CMake path works too: `cmake -S . -B build -G Ninja -DSTANDALONE=1` + `ninja -C build`.
- `./configure.sh` completely overwrites `config.make` — do not run it unnecessarily.
- Do not commit: `config.make`, `Rules.make`, `*.o`, `src/sdl-instead`, `desktop/instead.desktop` (untracked by design).
- If git complains about dubious ownership: `git config --global --add safe.directory /work`.

## Verification

Verification: `make test` (unit + smoke + integration) or `ctest --test-dir build --output-on-failure` for a CMake build. Unit tests live in `tests/unit` (pure functions util/list/xoshiro/cache/utils/bits; xoshiro128** is checked against reference vectors), smoke — `tests/smoke.sh`, integration (Lua game `tests/games/integration`) — `tests/integration.sh`. Manual smoke (run from the root only):

```
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 8 ./sdl-instead -nosound -software games/tutorial
```

Success: the line `Video mode: ... (software)`; the game does not exit by itself, `tests/smoke.sh` terminates it via SIGTERM (SDL handles it and exits with 0). The binary is built with ASan/LeakSanitizer: a leak on a clean exit prints a report and changes the exit code to 1. X11 path via `Xvfb :99 -screen 0 1024x768x24` (no GLX — the window is expected to fall back to software).

The `_USE_SWROTATE` branch does not compile with a plain `make` — when changing `graphics.c`/`input.c`, check:

```
cc -fsyntax-only src/graphics.c -Wall -Wextra -Wno-unused-parameter -Dunix -D_USE_SDL \
  -D_USE_UNPACK -D_LOCAL_APPDATA -D_HAVE_ICONV -D_USE_SWROTATE -DVERSION='"3.6.0"' \
  -DLANG_PATH='"./lang/"' -DSTEAD_PATH='"./stead/"' -DGAMES_PATH='"./games/"' \
  -DTHEMES_PATH='"./themes/"' -DICON_PATH='"./icon/"' $(pkg-config --cflags sdl3 sdl3-image sdl3-ttf luajit)

cppcheck --enable=warning,performance,portability --inconclusive --std=c11 \
  --suppress=missingIncludeSystem -D_USE_SDL -Dunix -D_USE_UNPACK -D_LOCAL_APPDATA \
  -D_HAVE_ICONV -D_USE_SWROTATE -I src src/graphics.c
```

cppcheck reports one expected `intToPointerCast` — `(SDL_Rect **)-1` is an intentional "any mode" sentinel.

## SDL3 port gotchas

- The canonical format is the `PIXEL_FORMAT` macro (`SDL_PIXELFORMAT_RGBA32`, graphics.c). All surfaces are normalized to it; address pixels via `img->pitch` and `SDL_BYTESPERPIXEL()`, not `w*4` (SDL_ttf returns a padded pitch and the `ARGB8888` format).
- `SDL_ConvertSurface` in SDL3 does not carry the color key over — `gfx_dup` restores it manually; keep this in mind for new conversions.
- In SDL3 bool functions return `true` on success (`SDL_Init`, `SDL_LockSurface`, `TTF_Init`, `SDL_GetFullscreenDisplayModes` together with the malloc'ed array) — conditions were inverted during the port relative to SDL2.
- Rendering: the software renderer draws dirty rects (`SDL_GetRendererName == SDL_SOFTWARE_RENDERER`), the hardware one draws the full frame with clearing.

## Commits

Commit only on an explicit user request, as `opencode <opencode@localhost>`: `git -c user.name=opencode -c user.email=opencode@localhost commit -m "..."`. Message style: a single short lowercase line (as in `git log`), only intended files, no build artifacts or `config.make`.
