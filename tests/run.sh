#!/bin/sh
# Build and run INSTEAD unit tests, then smoke tests.
# Usage: tests/run.sh   (also available as: make test)
#
# The engine binary for smoke tests is taken from $INSTEAD_BIN,
# ./sdl-instead or ./src/sdl-instead (first found).
set -e

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
cd "$ROOT"

CC=${CC:-cc}
LUA_CFLAGS=$(pkg-config --cflags luajit 2>/dev/null || true)
[ -n "$LUA_CFLAGS" ] || LUA_CFLAGS=$(pkg-config --cflags lua5.1 2>/dev/null || true)
[ -n "$LUA_CFLAGS" ] || LUA_CFLAGS=$(pkg-config --cflags lua 2>/dev/null || true)

echo "unit: build"
$CC -Wall -Wextra -Wno-unused-parameter -g -O2 $LUA_CFLAGS -I src/instead -I src \
	-o tests/unit/instead_unit_tests \
	tests/unit/main.c tests/unit/test_util.c tests/unit/test_list.c \
	tests/unit/test_tinymt.c tests/unit/test_cache.c tests/unit/test_utils.c \
	src/instead/util.c src/instead/list.c src/instead/tinymt32.c \
	src/instead/cache.c src/utils.c -lm

echo "unit: run"
tests/unit/instead_unit_tests

BIN=${INSTEAD_BIN:-}
if [ -z "$BIN" ]; then
	for c in ./sdl-instead ./src/sdl-instead; do
		if [ -x "$c" ]; then
			BIN=$c
			break
		fi
	done
fi
if [ -z "$BIN" ]; then
	echo "smoke: skipped (no sdl-instead binary; run 'make' first)"
	exit 0
fi
tests/smoke.sh "$BIN"
