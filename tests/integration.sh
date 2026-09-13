#!/bin/sh
# INSTEAD Lua integration tests. Usage: tests/integration.sh [path-to-binary]
#
# Must be run from a runtime root (repository root for a make build, or the
# staged standalone directory for a CMake build).
set -e

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
GAME=$ROOT/tests/games/integration
BIN=${1:-${INSTEAD_BIN:-./sdl-instead}}
if [ ! -x "$BIN" ]; then
	echo "integration: binary '$BIN' not found or not executable" >&2
	exit 1
fi

TMP=$(mktemp)
PID=
cleanup() {
	rm -f "$TMP"
	if [ -n "$PID" ]; then
		kill -KILL "$PID" 2>/dev/null || true
	fi
}
trap cleanup EXIT

echo "integration: $BIN $GAME"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	"$BIN" -nosound -software "$GAME" >"$TMP" 2>&1 &
PID=$!

i=0
while [ $i -lt 20 ]; do
	if grep -q "INTEGRATION OK" "$TMP"; then
		break
	fi
	if grep -q "INTEGRATION FAILED" "$TMP"; then
		cat "$TMP" >&2
		echo "integration: failed" >&2
		exit 1
	fi
	if ! kill -0 "$PID" 2>/dev/null; then
		cat "$TMP" >&2
		echo "integration: interpreter exited too early" >&2
		exit 1
	fi
	sleep 1
	i=$((i + 1))
done
if ! grep -q "INTEGRATION OK" "$TMP"; then
	cat "$TMP" >&2
	echo "integration: timeout waiting for tests" >&2
	exit 1
fi

grep "INTEGRATION CHECKS" "$TMP"

# SDL handles SIGTERM: the interpreter must quit cleanly
kill -TERM "$PID" 2>/dev/null || true
RC=0
wait "$PID" || RC=$?
PID=
if [ "$RC" -ne 0 ]; then
	cat "$TMP" >&2
	echo "integration: interpreter exited with code $RC" >&2
	exit 1
fi

echo "integration: ok"
