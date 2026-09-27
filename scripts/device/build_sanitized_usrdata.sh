#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'EOF'
Usage: build_sanitized_usrdata.sh SOURCE_DIR OUTPUT_FILE PACKER [PACKER_ARGS...]

Build a YAFFS usrdata image without the vendor demonstration Wi-Fi profile.
PACKER is invoked as:

  PACKER [PACKER_ARGS...] STAGED_SOURCE OUTPUT_FILE 2048

The output path must not already exist. The source directory is never modified.
EOF
}

if (($# < 3)); then
  usage >&2
  exit 64
fi

source_dir="$1"
output_file="$2"
packer="$3"
shift 3

[[ -d "$source_dir" ]] || { echo "ERROR: source directory is missing" >&2; exit 66; }
if [[ "$packer" == */* ]]; then
  [[ -x "$packer" ]] || { echo "ERROR: packer is not executable" >&2; exit 69; }
else
  packer="$(command -v "$packer" || true)"
  [[ -n "$packer" && -x "$packer" ]] || {
    echo "ERROR: packer is not executable" >&2
    exit 69
  }
fi
command -v rsync >/dev/null || { echo "ERROR: rsync is required" >&2; exit 69; }

source_dir="$(cd "$source_dir" && pwd -P)"
[[ "$source_dir" != "/" ]] || { echo "ERROR: refusing root as source" >&2; exit 64; }
output_parent="$(dirname "$output_file")"
[[ -d "$output_parent" ]] || { echo "ERROR: output directory is missing" >&2; exit 73; }
output_parent="$(cd "$output_parent" && pwd -P)"
output_file="$output_parent/$(basename "$output_file")"
[[ ! -e "$output_file" && ! -L "$output_file" ]] || {
  echo "ERROR: output already exists" >&2
  exit 73
}

case "$output_file" in
  "$source_dir"|"$source_dir"/*)
    echo "ERROR: output must be outside the source directory" >&2
    exit 64
    ;;
esac

work_dir="$(mktemp -d "$output_parent/.living-canvas-usrdata.XXXXXX")"
staging_dir="$work_dir/staging"
partial_output="$work_dir/$(basename "$output_file").partial"
mkdir -p "$staging_dir"
cleanup() {
  case "$work_dir" in
    "$output_parent"/.living-canvas-usrdata.*) rm -rf -- "$work_dir" ;;
  esac
}
trap cleanup EXIT

rsync -a --exclude='/etc/wifi/wapi.conf' -- "$source_dir/" "$staging_dir/"

manifest_delta="$(
  rsync -a --dry-run --itemize-changes --delete \
    --exclude='/etc/wifi/wapi.conf' -- "$source_dir/" "$staging_dir/"
)"
if [[ -n "$manifest_delta" ]]; then
  echo "ERROR: staged usrdata does not match the source manifest" >&2
  exit 65
fi

unexpected_profile="$(find "$staging_dir" -type f -name wapi.conf -print -quit)"
if [[ -n "$unexpected_profile" ]]; then
  echo "ERROR: staged usrdata still contains a Wi-Fi profile" >&2
  exit 65
fi

"$packer" "$@" "$staging_dir" "$partial_output" 2048
[[ -s "$partial_output" ]] || { echo "ERROR: packer produced no image" >&2; exit 74; }
ln -- "$partial_output" "$output_file"

printf 'Sanitized usrdata image: %s\n' "$(basename "$output_file")"
