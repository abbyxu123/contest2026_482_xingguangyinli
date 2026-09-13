#!/usr/bin/env bash
set -euo pipefail

app_dir="${APP_DIR:-$(cd "$(dirname "$0")/../.." && pwd)}"

grep -q 'src/lc_ui.c' "$app_dir/Makefile"
grep -q 'src/lc_ui.c' "$app_dir/CMakeLists.txt"

echo 'PASS: lc_ui build metadata'
