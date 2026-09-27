#!/usr/bin/env python3
"""Verify the vendor mkyaffs2image data-only 2048-byte-page format."""

from __future__ import annotations

import argparse
import hashlib
import os
from pathlib import Path, PurePosixPath
import struct
import sys
from typing import Optional, Tuple


PAGE_SIZE = 2048
INBAND_TAG_SIZE = 16
DATA_BYTES_PER_PAGE = PAGE_SIZE - INBAND_TAG_SIZE
# The pinned Allwinner mkyaffs2image assigns IDs in header order from 257.
FIRST_OBJECT_ID = 257
NAME_OFFSET = 10
NAME_LENGTH = 256
FILE_SIZE_OFFSET = 292
OBJECT_TYPES = {
    1: "file",
    2: "symlink",
    3: "directory",
    4: "hardlink",
    5: "special",
}


def normalized_relative_path(value: str) -> str:
    path = PurePosixPath(value)
    if path.is_absolute() or ".." in path.parts:
        raise ValueError(f"path must be relative: {value}")
    normalized = path.as_posix()
    if not normalized or normalized == ".":
        raise ValueError("path must identify an entry below the source root")
    return normalized


ManifestEntry = Tuple[str, Optional[int], Optional[str]]


def digest_file(path: Path, size: int | None = None, offset: int = 0) -> str:
    digest = hashlib.sha256()
    remaining = size
    with path.open("rb") as stream:
        stream.seek(offset)
        while remaining is None or remaining > 0:
            read_size = 1024 * 1024 if remaining is None else min(1024 * 1024, remaining)
            chunk = stream.read(read_size)
            if not chunk:
                break
            digest.update(chunk)
            if remaining is not None:
                remaining -= len(chunk)
    if remaining not in (None, 0):
        raise ValueError(f"image ended before {size} bytes could be read")
    return digest.hexdigest()


def digest_yaffs_file(image: Path, size: int, first_page: int) -> str:
    digest = hashlib.sha256()
    remaining = size
    with image.open("rb") as stream:
        stream.seek(first_page * PAGE_SIZE)
        while remaining > 0:
            page = stream.read(PAGE_SIZE)
            if len(page) != PAGE_SIZE:
                raise ValueError(f"image ended before {size} bytes could be read")
            data_size = min(DATA_BYTES_PER_PAGE, remaining)
            digest.update(page[:data_size])
            remaining -= data_size
    return digest.hexdigest()


def source_manifest(source: Path, excluded: set[str]) -> dict[str, ManifestEntry]:
    manifest: dict[str, ManifestEntry] = {}
    for current, directories, files in os.walk(source, followlinks=False):
        current_path = Path(current)
        for name in directories + files:
            entry = current_path / name
            relative = entry.relative_to(source).as_posix()
            if relative in excluded:
                continue
            if entry.is_symlink():
                manifest[relative] = ("symlink", None, None)
            elif entry.is_dir():
                manifest[relative] = ("directory", None, None)
            elif entry.is_file():
                size = entry.stat().st_size
                manifest[relative] = ("file", size, digest_file(entry))
            else:
                manifest[relative] = ("special", None, None)
    return manifest


def object_header(page: bytes) -> tuple[int, int, str, int | None] | None:
    object_type, parent_id = struct.unpack_from("<II", page, 0)
    if object_type not in OBJECT_TYPES or page[8:10] != b"\xff\xff":
        return None

    name_field = page[NAME_OFFSET : NAME_OFFSET + NAME_LENGTH]
    terminator = name_field.find(b"\x00")
    if terminator <= 0:
        return None
    raw_name = name_field[:terminator]
    try:
        name = raw_name.decode("utf-8")
    except UnicodeDecodeError:
        return None
    if name in {".", ".."} or "/" in name or "\x00" in name:
        return None
    if any(ord(character) < 32 for character in name):
        return None

    size = None
    if object_type == 1:
        size = struct.unpack_from("<I", page, FILE_SIZE_OFFSET)[0]
    return object_type, parent_id, name, size


def image_manifest(image: Path) -> dict[str, ManifestEntry]:
    image_size = image.stat().st_size
    if image_size == 0 or image_size % PAGE_SIZE != 0:
        raise ValueError(f"image size must be a non-zero multiple of {PAGE_SIZE}")

    objects: dict[int, tuple[int, str, int | None, int, int]] = {}
    with image.open("rb") as stream:
        page_index = 0
        while page := stream.read(PAGE_SIZE):
            header = object_header(page)
            if header is None:
                page_index += 1
                continue
            object_type, parent_id, name, size = header
            object_id = FIRST_OBJECT_ID + len(objects)
            objects[object_id] = (
                parent_id,
                name,
                size if object_type == 1 else None,
                object_type,
                page_index,
            )
            page_index += 1

    if not objects:
        raise ValueError("no YAFFS2 object headers found")

    resolved: dict[int, str] = {}

    def resolve(object_id: int, active: set[int]) -> str:
        if object_id in resolved:
            return resolved[object_id]
        if object_id in active:
            raise ValueError(f"parent cycle detected for object {object_id}")
        parent_id, name, _size, _object_type, _header_page = objects[object_id]
        if parent_id == 1:
            path = name
        else:
            if parent_id not in objects:
                raise ValueError(f"missing parent object {parent_id} for {name}")
            parent_type = objects[parent_id][3]
            if parent_type != 3:
                raise ValueError(f"parent object {parent_id} is not a directory")
            path = f"{resolve(parent_id, active | {object_id})}/{name}"
        resolved[object_id] = path
        return path

    manifest: dict[str, ManifestEntry] = {}
    for object_id, (_parent_id, _name, size, object_type, header_page) in objects.items():
        path = resolve(object_id, set())
        if path in manifest:
            raise ValueError(f"duplicate image path: {path}")
        digest = None
        if object_type == 1 and size is not None:
            digest = digest_yaffs_file(image, size, header_page + 1)
        manifest[path] = (OBJECT_TYPES[object_type], size, digest)
    return manifest


def describe_difference(
    expected: dict[str, ManifestEntry],
    actual: dict[str, ManifestEntry],
) -> list[str]:
    lines: list[str] = []
    for path in sorted(expected.keys() - actual.keys()):
        lines.append(f"missing: {path}")
    for path in sorted(actual.keys() - expected.keys()):
        lines.append(f"unexpected: {path}")
    for path in sorted(expected.keys() & actual.keys()):
        if expected[path] != actual[path]:
            lines.append(f"changed: {path} metadata or content differs")
    return lines


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Compare a mkyaffs2image manifest with its source directory."
    )
    parser.add_argument("source_dir", type=Path)
    parser.add_argument("image_file", type=Path)
    parser.add_argument("--exclude", action="append", default=[])
    args = parser.parse_args()

    source = args.source_dir.resolve(strict=True)
    image = args.image_file.resolve(strict=True)
    if not source.is_dir():
        parser.error("source_dir must be a directory")
    if not image.is_file():
        parser.error("image_file must be a file")

    try:
        excluded = {normalized_relative_path(path) for path in args.exclude}
        expected = source_manifest(source, excluded)
        actual = image_manifest(image)
    except (OSError, ValueError) as error:
        print(f"ERROR: {error}", file=sys.stderr)
        return 2

    differences = describe_difference(expected, actual)
    if differences:
        print("ERROR: manifest mismatch", file=sys.stderr)
        for difference in differences:
            print(f"  {difference}", file=sys.stderr)
        return 1

    file_count = sum(kind == "file" for kind, _size, _digest in actual.values())
    directory_count = sum(
        kind == "directory" for kind, _size, _digest in actual.values()
    )
    print(
        "PASS: YAFFS2 manifest matches source "
        f"(files={file_count} directories={directory_count} excluded={len(excluded)})"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
