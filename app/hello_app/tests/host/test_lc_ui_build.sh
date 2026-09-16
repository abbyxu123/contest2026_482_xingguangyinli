#!/usr/bin/env bash
set -euo pipefail

app_dir="${APP_DIR:-$(cd "$(dirname "$0")/../.." && pwd)}"

grep -q 'src/lc_ui.c' "$app_dir/Makefile"
grep -q 'src/lc_ui.c' "$app_dir/CMakeLists.txt"
grep -q 'src/generated/lc_choice_assets.c' "$app_dir/Makefile"
grep -q 'src/generated/lc_choice_assets.c' "$app_dir/CMakeLists.txt"
grep -q 'lc_display_run_image_preview' "$app_dir/include/lc_display.h"
grep -q 'lc_display_run_choice_preview' "$app_dir/include/lc_display.h"
grep -q 'lc_display_run_competition_demo' "$app_dir/include/lc_display.h"
grep -q 'LC_COMPETITION_REQUESTING' "$app_dir/src/lc_display.c"
grep -q 'LC_COMPETITION_RECOMMENDATION' "$app_dir/src/lc_display.c"
grep -q 'LC_COMPETITION_CONFIRMING' "$app_dir/src/lc_display.c"
grep -q 'LC_COMPETITION_QR' "$app_dir/src/lc_display.c"
grep -q 'LC_COMPETITION_ERROR' "$app_dir/src/lc_display.c"
grep -q '_Static_assert(sizeof(lc_bg_portrait)' \
  "$app_dir/src/generated/lc_choice_assets.c"

echo 'PASS: lc_ui build metadata'
