#!/usr/bin/env bash
set -euo pipefail

app_dir="${APP_DIR:-$(cd "$(dirname "$0")/../.." && pwd)}"

grep -q 'src/lc_ui.c' "$app_dir/Makefile"
grep -q 'src/lc_ui.c' "$app_dir/CMakeLists.txt"
grep -q 'src/generated/lc_choice_assets.c' "$app_dir/Makefile"
grep -q 'src/generated/lc_choice_assets.c' "$app_dir/CMakeLists.txt"
grep -q 'lc_display_run_image_preview' "$app_dir/include/lc_display.h"
grep -q 'lc_display_run_choice_preview' "$app_dir/include/lc_display.h"
grep -q '_Static_assert(sizeof(lc_bg_portrait)' \
  "$app_dir/src/generated/lc_choice_assets.c"

echo 'PASS: lc_ui build metadata'
