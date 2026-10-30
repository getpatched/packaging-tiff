# GetPatched recipe: TIFF

GetPatched maintains selected, tested public Ubuntu and Debian package updates. This repository contains recipe metadata, patches, and regression inputs; it does not duplicate the full upstream source tree.

## Candidate targets

| Target | Pinned source | GetPatched version | CVE | Current status |
|---|---|---|---|---|
| `ubuntu-focal-amd64` | Ubuntu 20.04 · `4.1.0+git191117-2ubuntu0.20.04.14` | `4.1.0+git191117-2ubuntu0.20.04.14+getpatched1` | CVE-2025-61144 | Development candidate |
| `debian-bookworm-amd64` | Debian 12 · `4.5.0-6+deb12u4` | `4.5.0-6+deb12u4+getpatched1` | CVE-2025-61144 | Development candidate |
| `debian-trixie-amd64` | Debian 13 · `4.7.0-3+deb13u3` | `4.7.0-3+deb13u3+getpatched1` | CVE-2025-61144 | Development candidate |

These entries are not release claims. All targets use the zero-hour public policy, but publication remains disabled until each exact source builds, the installed regression passes, the package-aware scan is clear, and a clean client installation is verified. Debian describes CVE-2025-61144 as a command-line tool crash with no security impact; Ubuntu rates it Low. The scope is bounded `tiffcrop` behavior, not a remote library compromise or a general security guarantee.

CVE-2025-61143 is excluded from these exact target builds. The original malformed-input probe and a second crop-validation probe both produced zero Valgrind errors on the pinned Ubuntu Focal, Debian Bookworm, and Debian Trixie sources; the latter also exited before the `TIFFFileName(out)` cleanup call. We do not claim the upstream 61143 patch fixes these target versions. The hosted baseline reports are retained in [`getpatched/platform`](https://github.com/getpatched/platform/tree/debian-snapshot-tls/evidence/tiff-hosted-baselines-2026-10-30.md).

## Repository structure

- `package-manifest.json` indexes separate target recipes.
- `targets/<distribution>/<codename>-<architecture>.json` pins the distro source descriptor and archives, exact source and output versions, changelog, binaries and patch selection.
- `patches/` holds DEP-3 backports. `patches/metadata.json` records source-version applicability, upstream commit, CVE, patch checksum and regression evidence inputs. The selected backport addresses the reproduced CVE-2025-61144 debug sample-bound bug.
- `tests/fixtures/` contains a valid one-pixel, nine-sample separate-planar TIFF for the CVE-2025-61144 debug sample-bound regression.
- `LICENSES/` preserves upstream and distro copyright and license notices.

Each target has an independent pinned build container, archive snapshot and scanner ecosystem (`Debian:12` or `Debian:13` for Debian) in `getpatched/platform/packages.json`. Debian profiles are currently build/scan candidates only; platform repository publication remains off. The descriptor and source payload hashes were checked against signed distro source indexes, but individual `.dsc` uploader signatures have not been separately verified.

## Patch and version policy

The recipe backports upstream [commit 88cf9db](https://gitlab.com/libtiff/libtiff/-/commit/88cf9dbb48f6e172629795ecffae35d5052f68aa), which caps a level-2 debug dump loop at `MAX_SAMPLES`, matching the fixed capacity of its sample-buffer array (CVE-2025-61144). The upstream CVE-2025-61143 guard is not selected because its cleanup fault has not reproduced on these exact sources.

The output preserves the complete distro source version and appends `+getpatched1`. A subsequent patch on the same base increments the serial, such as `+getpatched2`; a changed distro source requires a separately pinned target and fresh evidence. Version ranges alone never establish applicability.

## Build and verification

The platform build runner downloads the exact `.dsc` and source archives, applies the selected backport to distro packaging, and builds target-specific Debian source and binary packages. Installed baseline checks use Valgrind to reproduce the debug-dump bounds issue. Patched checks require the expected error handling without memory errors and include a `tiffinfo` consumer check. The release candidate remains ineligible if any check is missing or fails.

Local recipe validation (from a checkout with the platform repository alongside it):

```sh
python3 platform/scripts/build-package.py --source packaging-tiff --target ubuntu-focal-amd64 --output build/tiff-focal
python3 platform/scripts/build-package.py --source packaging-tiff --target debian-bookworm-amd64 --output build/tiff-bookworm
python3 platform/scripts/build-package.py --source packaging-tiff --target debian-trixie-amd64 --output build/tiff-trixie
```

The output is a local candidate only; this recipe does not enable or perform publication.

## Licensing

GetPatched-authored recipe metadata, patches, tests and documentation are MIT-licensed. Upstream TIFF and distribution packaging retain their own copyrights and licenses; preserve the notices in `LICENSES/` in every source and binary distribution.
