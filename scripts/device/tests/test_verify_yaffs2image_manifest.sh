#!/usr/bin/env bash
set -euo pipefail

VERIFIER="${VERIFIER:-$(cd "$(dirname "$0")/.." && pwd)/verify_yaffs2image_manifest.py}"
TEST_ROOT="$(mktemp -d)"
trap 'rm -rf "$TEST_ROOT"' EXIT

mkdir -p "$TEST_ROOT/source/etc/wifi" "$TEST_ROOT/source/assets"
printf '%s' 'startup-audio' >"$TEST_ROOT/source/startup.wav"
printf '%s' 'project-asset' >"$TEST_ROOT/source/assets/project.bin"
printf '%s' 'vendor-network-profile' >"$TEST_ROOT/source/etc/wifi/wapi.conf"

python3 - "$TEST_ROOT" <<'PY'
from pathlib import Path
import struct
import sys

root = Path(sys.argv[1])
large = (b"0123456789abcdef" * 157)[:2500]
(root / "source" / "large.bin").write_bytes(large)

def header(object_type, parent_id, name, size=0):
    page = bytearray(b"\xff" * 2048)
    struct.pack_into("<II", page, 0, object_type, parent_id)
    page[8:10] = b"\xff\xff"
    encoded = name.encode("utf-8")
    page[10:10 + len(encoded)] = encoded
    page[10 + len(encoded)] = 0
    struct.pack_into("<I", page, 292, size)
    return bytes(page)

def data_pages(payload):
    pages = []
    for offset in range(0, len(payload), 2032):
        chunk = payload[offset:offset + 2032]
        pages.append(chunk + b"\xff" * (2032 - len(chunk)) + b"\xff" * 16)
    return b"".join(pages)

startup = b"startup-audio"
project = b"project-asset"
profile = b"vendor-network-profile"

# Object IDs are assigned by header order starting at 257, matching
# mkyaffs2image. Root has object ID 1.
valid = b"".join([
    header(1, 1, "startup.wav", len(startup)), data_pages(startup),
    header(3, 1, "assets"),
    header(1, 258, "project.bin", len(project)), data_pages(project),
    header(3, 1, "etc"),
    header(3, 260, "wifi"),
    header(1, 1, "large.bin", len(large)), data_pages(large),
])
(root / "valid.fex").write_bytes(valid)

missing = b"".join([
    header(1, 1, "startup.wav", len(startup)), data_pages(startup),
    header(3, 1, "assets"),
    header(3, 1, "etc"),
    header(3, 259, "wifi"),
    header(1, 1, "large.bin", len(large)), data_pages(large),
])
(root / "missing.fex").write_bytes(missing)

extra = valid + header(1, 261, "wapi.conf", len(profile)) + data_pages(profile)
(root / "extra.fex").write_bytes(extra)

wrong_size = valid.replace(
    header(1, 258, "project.bin", len(project)),
    header(1, 258, "project.bin", len(project) + 1),
)
(root / "wrong-size.fex").write_bytes(wrong_size)

wrong_content = valid.replace(
    data_pages(project),
    data_pages(b"x" * len(project)),
    1,
)
(root / "wrong-content.fex").write_bytes(wrong_content)
PY

python3 "$VERIFIER" \
  "$TEST_ROOT/source" \
  "$TEST_ROOT/valid.fex" \
  --exclude etc/wifi/wapi.conf \
  >"$TEST_ROOT/valid.out"
grep -q 'PASS: YAFFS2 manifest matches source' "$TEST_ROOT/valid.out"
grep -q 'files=3 directories=3' "$TEST_ROOT/valid.out"

for invalid in missing extra wrong-size wrong-content; do
  if python3 "$VERIFIER" \
    "$TEST_ROOT/source" \
    "$TEST_ROOT/$invalid.fex" \
    --exclude etc/wifi/wapi.conf \
    >"$TEST_ROOT/$invalid.out" 2>"$TEST_ROOT/$invalid.err"; then
    echo "FAIL: $invalid image must be rejected" >&2
    exit 1
  fi
  grep -q 'manifest mismatch' "$TEST_ROOT/$invalid.err"
done

echo 'PASS: YAFFS2 verifier accepts only a complete sanitized manifest'
