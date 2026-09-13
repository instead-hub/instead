#!/bin/sh
# INSTEAD smoke tests. Usage: tests/smoke.sh [path-to-binary]
#
# Must be run from a runtime root: the repository root for a standalone
# (make) build, or the staged standalone directory for a CMake build.
set -e

BIN=${1:-${INSTEAD_BIN:-./sdl-instead}}
if [ ! -x "$BIN" ]; then
	echo "smoke: binary '$BIN' not found or not executable" >&2
	exit 1
fi

echo "smoke: $BIN -version"
OUT=$(SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy "$BIN" -version)
printf '  %s\n' "$OUT"
case "$OUT" in
	*[0-9].[0-9].[0-9]*) ;;
	*) echo "smoke: unexpected -version output" >&2; exit 1 ;;
esac

echo "smoke: games/tutorial (headless)"
TMP=$(mktemp)
PID=
cleanup() {
	rm -f "$TMP"
	if [ -n "$PID" ]; then
		kill -KILL "$PID" 2>/dev/null || true
	fi
}
trap cleanup EXIT

SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
	"$BIN" -nosound -software games/tutorial >"$TMP" 2>&1 &
PID=$!

i=0
while [ $i -lt 20 ]; do
	if grep -q "Video mode:" "$TMP"; then
		break
	fi
	if ! kill -0 "$PID" 2>/dev/null; then
		cat "$TMP" >&2
		echo "smoke: tutorial exited before video init" >&2
		exit 1
	fi
	sleep 1
	i=$((i + 1))
done
if ! grep -q "Video mode:" "$TMP"; then
	cat "$TMP" >&2
	echo "smoke: no 'Video mode' in output" >&2
	exit 1
fi

# SDL handles SIGTERM: the game must quit cleanly (and pass leak checks)
kill -TERM "$PID" 2>/dev/null || true
RC=0
wait "$PID" || RC=$?
PID=
if [ "$RC" -ne 0 ]; then
	cat "$TMP" >&2
	echo "smoke: tutorial exited with code $RC" >&2
	exit 1
fi

echo "smoke: ok"
