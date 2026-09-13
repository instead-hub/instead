# AGENTS.md — INSTEAD

Интерпретатор приключенческих игр (C + Lua/LuaJIT, рендер SDL3). Активная ветка — `SDL3` (порт с SDL2); `master` — старая SDL2-версия. В этой ветке `CI.yml` и `appimage.yml` собирают SDL3 (SDL3/SDL3_ttf/image/mixer тянутся из исходников на release-тегах — в apt Ubuntu их нет), `windows.yml` собирает SDL3 под mingw (официальные mingw-пакеты SDL3 + LuaJIT/libxmp из исходников), `emscripten.yml` собирает SDL3 под wasm (порты emscripten `sdl3`/`sdl3_ttf`/`zlib`, SDL3_image/SDL3_mixer/libxmp — из исходников через `emcmake`; `_USE_BROWSE` на wasm выключен). `INSTALL` описывает сборку SDL3 (cmake/configure.sh); источник правды `config.make` + `Makefile`.

Карта: `src/` — движок и рендер (`src/instead/` — Lua VM и движок), `stead/` — Lua-библиотека игр, `games/tutorial` — игра для smoke, `tests/games/integration` — тест-игра для интеграционных тестов, `themes/` — данные тем.

## Сборка (Alpine Linux)

Пакеты: `apk add git build-base pkgconf cmake ninja zlib-dev luajit-dev sdl3-dev sdl3_ttf-dev libpng-dev libjpeg-turbo-dev libwebp-dev tiff-dev giflib-dev libogg-dev libvorbis-dev flac-dev opusfile-dev libxmp-dev xvfb cppcheck`

- В Alpine нет `pkg-config`, есть `pkgconf`; пакетов `sdl3_image`/`sdl3_mixer` нет — они собраны из исходников и установлены в `/usr` (`/tmp/opencode/SDL_image` release-3.4.6, `/tmp/opencode/SDL_mixer` release-3.2.4). При чистом окружении собрать: `cmake ... -DCMAKE_INSTALL_PREFIX=/usr -DSDLIMAGE_*` / `-DSDLMIXER_*` с `*_VENDORED=OFF`, у mixer обязателен `-DSDLMIXER_FLAC_LIBFLAC=OFF` (сломанный flac-config в Alpine).
- Dev-сборка: `make -j$(nproc)` из корня. `Makefile` включает `Rules.make` (symlink на `Rules.make.standalone`) и локальный untracked `config.make` (флаги SDL3/LuaJIT/zlib; ASan — по желанию). Бинарник — `src/sdl-instead`, запускать из корня репозитория (пути `./stead`, `./games`).
- CMake-путь тоже рабочий: `cmake -S . -B build -G Ninja -DSTANDALONE=1` + `ninja -C build`.
- `./configure.sh` полностью перезаписывает `config.make` — не запускать без необходимости.
- Не коммитить: `config.make`, `Rules.make`, `*.o`, `src/sdl-instead`, `desktop/instead.desktop` (untracked by design).
- Если git ругается на dubious ownership: `git config --global --add safe.directory /work`.

## Проверка

Проверка: `make test` (unit + smoke + integration) или `ctest --test-dir build --output-on-failure` для CMake-сборки. Unit-тесты — `tests/unit` (чистые функции util/list/tinymt/cache/utils/bits; TinyMT сверяется с официальными векторами), smoke — `tests/smoke.sh`, интеграционные (Lua-игра `tests/games/integration`) — `tests/integration.sh`. Ручной smoke (запуск только из корня):

```
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 8 ./sdl-instead -nosound -software games/tutorial
```

Успех: строка `Video mode: ... (software)`; игра сама не выходит, `tests/smoke.sh` завершает её через SIGTERM (SDL обрабатывает его и выходит с 0). Бинарник собран с ASan/LeakSanitizer: утечка при чистом выходе печатает отчёт и меняет код на 1. X11-путь через `Xvfb :99 -screen 0 1024x768x24` (GLX нет — окно ожидаемо падает в software).

Ветка `_USE_SWROTATE` обычным `make` не компилируется — при правках `graphics.c`/`input.c` проверять:

```
cc -fsyntax-only src/graphics.c -Wall -Wextra -Wno-unused-parameter -Dunix -D_USE_SDL \
  -D_USE_UNPACK -D_LOCAL_APPDATA -D_HAVE_ICONV -D_USE_SWROTATE -DVERSION='"3.6.0"' \
  -DLANG_PATH='"./lang/"' -DSTEAD_PATH='"./stead/"' -DGAMES_PATH='"./games/"' \
  -DTHEMES_PATH='"./themes/"' -DICON_PATH='"./icon/"' $(pkg-config --cflags sdl3 sdl3-image sdl3-ttf luajit)

cppcheck --enable=warning,performance,portability --inconclusive --std=c11 \
  --suppress=missingIncludeSystem -D_USE_SDL -Dunix -D_USE_UNPACK -D_LOCAL_APPDATA \
  -D_HAVE_ICONV -D_USE_SWROTATE -I src src/graphics.c
```

cppcheck выдаёт один ожидаемый `intToPointerCast` — `(SDL_Rect **)-1` это намеренный sentinel «любой режим».

## Грабли SDL3-порта

- Канонический формат — макрос `PIXEL_FORMAT` (`SDL_PIXELFORMAT_RGBA32`, graphics.c). Все поверхности нормализуются к нему; адресация пикселей — через `img->pitch` и `SDL_BYTESPERPIXEL()`, не через `w*4` (SDL_ttf отдаёт pitch с выравниванием, формат `ARGB8888`).
- `SDL_ConvertSurface` в SDL3 не переносит color key — `gfx_dup` восстанавливает его вручную; при новых конверсиях учитывать.
- В SDL3 bool-функции возвращают `true` при успехе (`SDL_Init`, `SDL_LockSurface`, `TTF_Init`, `SDL_GetFullscreenDisplayModes` вместе с malloc-массивом) — условия при порте инвертированы относительно SDL2.
- Рендер: программный рендерер рисует dirty-rect (`SDL_GetRendererName == SDL_SOFTWARE_RENDERER`), аппаратный — полный кадр с очисткой.

## Коммиты

Коммиты — только по явной просьбе пользователя, от автора `opencode <opencode@localhost>`: `git -c user.name=opencode -c user.email=opencode@localhost commit -m "..."`. Стиль сообщений: одна короткая строка с маленькой буквы (как в `git log`), только намеренные файлы, без артефактов сборки и `config.make`.
