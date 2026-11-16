#!/usr/bin/env bash
set -euo pipefail
export LC_ALL=C
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"

python3 - <<'PY'
from pathlib import Path
from struct import pack

# One 8-bit grayscale pixel in a baseline uncompressed TIFF.
tags = [
    (256, 3, 1),  # ImageWidth
    (257, 3, 1),  # ImageLength
    (258, 3, 8),  # BitsPerSample
    (259, 3, 1),  # Compression: none
    (262, 3, 1),  # PhotometricInterpretation: black is zero
    (273, 4, 122),  # StripOffsets: byte after the directory
    (277, 3, 1),  # SamplesPerPixel
    (278, 4, 1),  # RowsPerStrip
    (279, 4, 1),  # StripByteCounts
]
directory = pack("<H", len(tags))
directory += b"".join(pack("<HHII", tag, tag_type, 1, value) for tag, tag_type, value in tags)
Path("input.tif").write_bytes(b"II" + pack("<HI", 42, 8) + directory + pack("<I", 0) + b"\x80")
PY

tiffcp -c lzw input.tif output.tif
tiffinfo -D output.tif > info
grep -Eq 'Image Width: 1[[:space:]]+Image Length: 1' info
grep -Eq 'Compression Scheme: LZW' info
