#!/usr/bin/env bash
set -euo pipefail

SCRIPT_UNDER_TEST="${SCRIPT_UNDER_TEST:-$(cd "$(dirname "$0")/.." && pwd)/build_sanitized_usrdata.sh}"
TEST_ROOT="$(mktemp -d)"
trap 'rm -rf "$TEST_ROOT"' EXIT

mkdir -p \
  "$TEST_ROOT/source/etc/wifi" \
  "$TEST_ROOT/source/assets/audio" \
  "$TEST_ROOT/source/assets/display" \
  "$TEST_ROOT/source/empty-dir"
printf '%s\n' '{"ssid":"vendor-demo","psk":"do-not-package"}' \
  >"$TEST_ROOT/source/etc/wifi/wapi.conf"
printf '%s\n' 'keep-this-config' >"$TEST_ROOT/source/etc/wifi/keep.conf"
printf '%s\n' 'living-canvas-asset' >"$TEST_ROOT/source/assets/project.txt"
printf '%s\n' 'synthetic-audio' >"$TEST_ROOT/source/assets/audio/startup.wav"
printf '%s\n' 'synthetic-logo' >"$TEST_ROOT/source/assets/display/logo1.bin"

(
  cd "$TEST_ROOT/source"
  find . -type f ! -path './etc/wifi/wapi.conf' | LC_ALL=C sort
) >"$TEST_ROOT/expected-files"

cat >"$TEST_ROOT/fake-mkyaffs2image" <<'SH'
#!/usr/bin/env bash
set -euo pipefail
[[ "$1" == "--runtime-root" ]]
[[ "$2" == "synthetic-root" ]]
shift 2
source_dir="$1"
output_file="$2"
chunk_size="$3"

[[ "$chunk_size" == "2048" ]]
[[ ! -e "$source_dir/etc/wifi/wapi.conf" ]]
grep -q 'keep-this-config' "$source_dir/etc/wifi/keep.conf"
grep -q 'living-canvas-asset' "$source_dir/assets/project.txt"
grep -q 'synthetic-audio' "$source_dir/assets/audio/startup.wav"
grep -q 'synthetic-logo' "$source_dir/assets/display/logo1.bin"
[[ -d "$source_dir/empty-dir" ]]
(
  cd "$source_dir"
  find . -type f | LC_ALL=C sort
) >"$PACK_MANIFEST"
diff -u "$EXPECTED_MANIFEST" "$PACK_MANIFEST"
printf '%s\n' "$source_dir" >"$PACK_TRACE"
printf '%s\n' 'sanitized-yaffs-image' >"$output_file"
SH
chmod +x "$TEST_ROOT/fake-mkyaffs2image"

export PACK_TRACE="$TEST_ROOT/packer-source"
export PACK_MANIFEST="$TEST_ROOT/packer-manifest"
export EXPECTED_MANIFEST="$TEST_ROOT/expected-files"
"$SCRIPT_UNDER_TEST" \
  "$TEST_ROOT/source" \
  "$TEST_ROOT/usrdata.fex" \
  "$TEST_ROOT/fake-mkyaffs2image" \
  --runtime-root synthetic-root

grep -q 'sanitized-yaffs-image' "$TEST_ROOT/usrdata.fex"
grep -q 'do-not-package' "$TEST_ROOT/source/etc/wifi/wapi.conf"
staged_source="$(cat "$PACK_TRACE")"
[[ "$staged_source" != "$TEST_ROOT/source" ]]
[[ ! -e "$staged_source" ]]

if "$SCRIPT_UNDER_TEST" \
  "$TEST_ROOT/source" \
  "$TEST_ROOT/usrdata.fex" \
  "$TEST_ROOT/fake-mkyaffs2image" \
  --runtime-root synthetic-root \
  >"$TEST_ROOT/retry.out" 2>"$TEST_ROOT/retry.err"; then
  echo 'FAIL: existing output must not be overwritten' >&2
  exit 1
fi
grep -q 'output already exists' "$TEST_ROOT/retry.err"

PATH="$TEST_ROOT:$PATH" "$SCRIPT_UNDER_TEST" \
  "$TEST_ROOT/source" \
  "$TEST_ROOT/usrdata-from-path.fex" \
  fake-mkyaffs2image \
  --runtime-root synthetic-root
grep -q 'sanitized-yaffs-image' "$TEST_ROOT/usrdata-from-path.fex"

mkdir -p "$TEST_ROOT/bin"
cat >"$TEST_ROOT/bin/rsync" <<'SH'
#!/usr/bin/env bash
echo 'unsafe rsync invocation' >&2
exit 98
SH
chmod +x "$TEST_ROOT/bin/rsync"
if PATH="$TEST_ROOT/bin:$PATH" "$SCRIPT_UNDER_TEST" \
  /tmp/.. \
  "$TEST_ROOT/root-source.fex" \
  "$TEST_ROOT/fake-mkyaffs2image" \
  --runtime-root synthetic-root \
  >"$TEST_ROOT/root.out" 2>"$TEST_ROOT/root.err"; then
  echo 'FAIL: canonical root source must be rejected' >&2
  exit 1
fi
grep -q 'refusing root as source' "$TEST_ROOT/root.err"
! grep -q 'unsafe rsync invocation' "$TEST_ROOT/root.err"

cat >"$TEST_ROOT/failing-packer" <<'SH'
#!/usr/bin/env bash
exit 42
SH
chmod +x "$TEST_ROOT/failing-packer"
if "$SCRIPT_UNDER_TEST" \
  "$TEST_ROOT/source" \
  "$TEST_ROOT/failure.fex" \
  "$TEST_ROOT/failing-packer" \
  >"$TEST_ROOT/failure.out" 2>"$TEST_ROOT/failure.err"; then
  echo 'FAIL: packer failure must propagate' >&2
  exit 1
fi
[[ ! -e "$TEST_ROOT/failure.fex" ]]

cat >"$TEST_ROOT/empty-packer" <<'SH'
#!/usr/bin/env bash
set -euo pipefail
output_file="${@: -2:1}"
: >"$output_file"
SH
chmod +x "$TEST_ROOT/empty-packer"
if "$SCRIPT_UNDER_TEST" \
  "$TEST_ROOT/source" \
  "$TEST_ROOT/empty.fex" \
  "$TEST_ROOT/empty-packer" \
  >"$TEST_ROOT/empty.out" 2>"$TEST_ROOT/empty.err"; then
  echo 'FAIL: empty packer output must be rejected' >&2
  exit 1
fi
grep -q 'packer produced no image' "$TEST_ROOT/empty.err"
[[ ! -e "$TEST_ROOT/empty.fex" ]]

ln -s "$TEST_ROOT/missing-target" "$TEST_ROOT/dangling.fex"
if "$SCRIPT_UNDER_TEST" \
  "$TEST_ROOT/source" \
  "$TEST_ROOT/dangling.fex" \
  "$TEST_ROOT/fake-mkyaffs2image" \
  --runtime-root synthetic-root \
  >"$TEST_ROOT/dangling.out" 2>"$TEST_ROOT/dangling.err"; then
  echo 'FAIL: dangling output symlink must not be replaced' >&2
  exit 1
fi
grep -q 'output already exists' "$TEST_ROOT/dangling.err"
[[ -L "$TEST_ROOT/dangling.fex" ]]

[[ -z "$(find "$TEST_ROOT" -maxdepth 1 -type d -name '.living-canvas-usrdata.*' -print -quit)" ]]

echo 'PASS: sanitized usrdata excludes vendor network profile'
