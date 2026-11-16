#!/usr/bin/env bash
set -euo pipefail
export LC_ALL=C
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"

python3 - <<'PY'
from pathlib import Path
from struct import pack

for name, width, height in (("landscape", 3, 2), ("portrait", 2, 3)):
    pixels = bytes(range(width * height))
    tags = [
        (256, 3, width), (257, 3, height), (258, 3, 8),
        (259, 3, 1), (262, 3, 1), (273, 4, 122),
        (277, 3, 1), (278, 4, height), (279, 4, len(pixels)),
    ]
    directory = pack("<H", len(tags))
    directory += b"".join(pack("<HHII", tag, kind, 1, value) for tag, kind, value in tags)
    Path(f"{name}.tif").write_bytes(b"II" + pack("<HI", 42, 8) + directory + pack("<I", 0) + pixels)
PY

tiffcp -c lzw landscape.tif portrait.tif combined.tif
tiffsplit combined.tif page-
pages=(page-*.tif)
test "${#pages[@]}" -eq 2
tiffinfo -D "${pages[0]}" > first.info
tiffinfo -D "${pages[1]}" > second.info
grep -Eq 'Image Width: 3[[:space:]]+Image Length: 2' first.info
grep -Eq 'Image Width: 2[[:space:]]+Image Length: 3' second.info
