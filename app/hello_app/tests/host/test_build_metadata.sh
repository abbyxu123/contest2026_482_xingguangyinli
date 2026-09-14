#!/usr/bin/env bash
set -euo pipefail

app_dir="${APP_DIR:-$(cd "$(dirname "$0")/../.." && pwd)}"
symbol='LVX_USE_DEMO_CONTEST2026_482_LIVING_CANVAS'

grep -q "config $symbol" "$app_dir/Kconfig"
grep -q "CONFIG_$symbol" "$app_dir/Make.defs"
grep -q 'packages/demos/contest2026_482_hello_app' "$app_dir/Make.defs"
grep -q "CONFIG_$symbol" "$app_dir/Makefile"
grep -q "CONFIG_$symbol" "$app_dir/CMakeLists.txt"

for source in lc_state.c lc_clock.c lc_dinner.c lc_memory.c lc_voice.c; do
  grep -q "src/$source" "$app_dir/Makefile"
  grep -q "src/$source" "$app_dir/CMakeLists.txt"
done

grep -q 'include' "$app_dir/Makefile"
grep -q 'INCLUDE_DIRECTORIES' "$app_dir/CMakeLists.txt"
grep -q '#include "lc_voice.h"' "$app_dir/hello_app_main.c"
grep -q 'lc_voice_init' "$app_dir/hello_app_main.c"


if grep -Eq 'CONTEST2026_000|team 000' \
  "$app_dir/Kconfig" "$app_dir/Make.defs" "$app_dir/Makefile" \
  "$app_dir/CMakeLists.txt" "$app_dir/hello_app_main.c"; then
  echo 'FAIL: template team 000 metadata remains' >&2
  exit 1
fi

echo 'PASS: openvela build metadata'
