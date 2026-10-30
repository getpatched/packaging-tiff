# GetPatched recipe: TIFF

GetPatched maintains selected, tested security backports for public Ubuntu and Debian source packages. This repository contains recipe metadata, patches, and regression inputs; it does not duplicate the full upstream source tree.

## Candidate targets

| Target | Pinned source | GetPatched version | CVE | Current status |
|---|---|---|---|---|
| `ubuntu-focal-amd64` | Ubuntu 20.04 · `4.1.0+git191117-2ubuntu0.20.04.14` | `4.1.0+git191117-2ubuntu0.20.04.14+getpatched1` | CVE-2025-61144 | Development candidate |
| `debian-bookworm-amd64` | Debian 12 · `4.5.0-6+deb12u4` | `4.5.0-6+deb12u4+getpatched1` | CVE-2025-61144 | Development candidate |
| `debian-trixie-amd64` | Debian 13 · `4.7.0-3+deb13u3` | `4.7.0-3+deb13u3+getpatched1` | CVE-2025-61144 | Development candidate |

These entries are not release claims. Debian publication routing is disabled, and no target is supported until the exact source builds, the vulnerable installed package reproduces the issue, the patched installed package passes the regression and package-aware scan, and a clean client installation is verified. CVE-2025-61144 affects `tiffcrop` debug output; Debian's tracker describes this as a CLI crash without security impact. Do not present it as a remote library compromise.

## Repository structure

- `package-manifest.json` indexes separate target recipes.
- `targets/<distribution>/<codename>-<architecture>.json` pins the distro source descriptor and archives, exact source and output versions, changelog, binaries and patch selection.
- `patches/` holds DEP-3 backports. `patches/metadata.json` records source-version applicability, upstream commit, CVE, patch checksum and regression evidence inputs.
- `tests/fixtures/` contains the bounded nine-sample planar TIFF fixture and its generator for the `tiffcrop` debug-dump regression.
- `LICENSES/` preserves upstream and distro copyright and license notices.

Each target has an independent pinned build container, archive snapshot and scanner ecosystem (`Debian:12` or `Debian:13` for Debian) in `getpatched/platform/packages.json`. Debian profiles are currently build/scan candidates only; platform repository publication remains off. The descriptor and source payload hashes were checked against signed distro source indexes, but individual `.dsc` uploader signatures have not been separately verified.

## Patch and version policy

The patch is the upstream fix from [libtiff commit 88cf9db](https://gitlab.com/libtiff/libtiff/-/commit/88cf9dbb48f6e172629795ecffae35d5052f68aa), adapted to each distribution's `tiffcrop.c`. It caps a level-2 debug dump loop at `MAX_SAMPLES`, matching the fixed capacity of its sample-buffer array. It applies only to the exact source versions listed in each target and in patch metadata.

The output preserves the complete distro source version and appends `+getpatched1`. A subsequent patch on the same base increments the serial, such as `+getpatched2`; a changed distro source requires a separately pinned target and fresh evidence. Version ranges alone never establish applicability.

## Build and verification

The platform build runner downloads the exact `.dsc` and source archives, applies the selected backport to distro packaging, and builds the target-specific Debian source and binary packages. On the exact installed baseline, it runs `tiffcrop -D format:txt,level:2,input:...` under Valgrind Memcheck and requires the specific invalid 8-byte read immediately beyond the 64-byte `srcbuffs` stack array in `loadImage`/`combineSeparateSamplesBytes`; a generic SIGSEGV is insufficient. Source builds of the pinned distro sources independently reproduce the stack-buffer overflow with AddressSanitizer on the same fixture, but that source-level result does not replace the installed-package regression. The installed patched package must complete the debug dump under Memcheck with zero errors, followed by a successful `tiffinfo` readback. The release candidate remains ineligible if any check is missing or fails.

Local recipe validation (from a checkout with the platform repository alongside it):

```sh
python3 platform/scripts/build-package.py --source packaging-tiff --target ubuntu-focal-amd64 --output build/tiff-focal
python3 platform/scripts/build-package.py --source packaging-tiff --target debian-bookworm-amd64 --output build/tiff-bookworm
python3 platform/scripts/build-package.py --source packaging-tiff --target debian-trixie-amd64 --output build/tiff-trixie
```

The output is a local candidate only; this recipe does not enable or perform publication.

## Licensing

GetPatched-authored recipe metadata, patches, tests and documentation are MIT-licensed. Upstream TIFF and distribution packaging retain their own copyrights and licenses; preserve the notices in `LICENSES/` in every source and binary distribution.
