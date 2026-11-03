# GetPatched recipe: TIFF

GetPatched maintains selected, tested public Ubuntu and Debian package updates. This repository contains recipe metadata, patches, and regression inputs; it does not duplicate the full upstream source tree.

## Candidate targets

| Target | Pinned source | GetPatched version | CVE | Current status |
|---|---|---|---|---|
| `ubuntu-focal-amd64` | Ubuntu 20.04 · `4.1.0+git191117-2ubuntu0.20.04.14` | `4.1.0+git191117-2ubuntu0.20.04.14+getpatched2` | CVE-2025-61144 | Development candidate |
| `debian-bookworm-amd64` | Debian 12 · `4.5.0-6+deb12u4` | `4.5.0-6+deb12u4+getpatched4` | CVE-2025-61144, CVE-2026-52490, CVE-2026-12912 | Candidate; fresh hosted build and regressions pending |
| `debian-trixie-amd64` | Debian 13 · `4.7.0-3+deb13u3` | `4.7.0-3+deb13u3+getpatched3` | CVE-2025-61144, CVE-2026-52490 | Hosted development verification passed; release blocked |

These are unreleased development candidates, not production release claims. Focal's `+getpatched2` candidate has hosted verification evidence for CVE-2025-61144. Bookworm `+getpatched4` adds a focused PixarLog fix for CVE-2026-12912; it is not verified or counted until the exact installed baseline reproduces the out-of-bounds write, the patched package rejects the undersized output buffer cleanly, package tests and consumers pass, and the package-aware before/after scan is retained. The Bookworm and Trixie `+getpatched3` candidates passed exact installed baseline/patched regressions, package-aware before/after scans, `tiffinfo` compatibility checks, and isolated signed APT client tests in runs [41553176585](https://github.com/getpatched/platform/actions/runs/41553176585) and [41554198692](https://github.com/getpatched/platform/actions/runs/41554198692). Their package-aware scans changed both target findings (CVE-2025-61144 and CVE-2026-52490) to zero; both reports set `candidateVerified=true`. Raw Trivy 0.70 used database SHA-256 `05a1fa5372e54059b6a7c1fc41afb812fafd9bd56a8e0ab6aa20d7f3f154ff31` and still reports both CVEs on candidate `libtiff-tools` and unchanged `libtiff6` without fixed versions. That scanner discrepancy remains unresolved and blocks production release; do not describe these images as Trivy-clean. No CVE-2026-52490 fix claim is made for Ubuntu or arm64. Debian marks the new issue minor/no-DSA on Trixie, and Canonical rates it Medium but lists no maintained Ubuntu release affected. This is a `tiffcrop` CLI input-validation fix, not a remote library compromise claim.

The Focal build also produces `libtiff5` so `libtiff-tools` can install with its matching, versioned runtime dependency. The advisory scan scope remains `libtiff-tools`, where the patched `tiffcrop` executable is shipped.

CVE-2025-61143 is not selected for these pinned targets. The exact hosted baselines failed `loadImage` and exited before the debug cleanup marker, with zero Memcheck errors. Upstream's v4.5.0 and v4.7.0 `tiffcrop` sources call `exit(EXIT_FAILURE)` at that failure site before cleanup. Ubuntu and Debian trackers still list the source versions as affected, so this is an applicability conflict, not a claim that the CVE is fixed or not affected. The reference patch remains in `patches/` for review, but no target applies it; re-enable it only after an exact package baseline reproduces the CVE path.

## Repository structure

- `package-manifest.json` indexes separate target recipes.
- `targets/<distribution>/<codename>-<architecture>.json` pins the distro source descriptor and archives, exact source and output versions, changelog, binaries and patch selection.
- `patches/` holds DEP-3 backports. `patches/metadata.json` records target applicability, upstream commit, CVE, patch checksum and regression evidence. The active recipes apply the CVE-2025-61144 fix that bounds the debug sample dump loop. The CVE-2025-61143 guard remains an unselected reference candidate because its failure path has not reproduced on these exact package baselines.
- `tests/fixtures/` contains a one-pixel, nine-sample separate-planar TIFF used for the CVE-2025-61144 sample-bound check. The malformed-input probe for CVE-2025-61143 is retained as an applicability investigation, not as passing regression evidence.
- `LICENSES/` preserves upstream and distro copyright and license notices.

Each target has an independent pinned build container, archive snapshot and scanner ecosystem (`Debian:12` or `Debian:13` for Debian) in `getpatched/platform/packages.json`. Debian profiles are currently build/scan candidates only; platform repository publication remains off. The descriptor and source payload hashes were checked against signed distro source indexes, but individual `.dsc` uploader signatures have not been separately verified.

## Patch and version policy

The active recipe backports upstream [commit 88cf9db](https://gitlab.com/libtiff/libtiff/-/commit/88cf9dbb48f6e172629795ecffae35d5052f68aa), which caps a level-2 debug dump loop at `MAX_SAMPLES`, matching the fixed capacity of its sample-buffer array (CVE-2025-61144). Bookworm and Trixie carry target-specific, focused backports of the `tiffcrop` subdivision-count guards from upstream security-fix [commit b04e935](https://gitlab.com/libtiff/libtiff/-/commit/b04e935cb6242f22cc8b63c99a372cf3ea825e4e), covering command-line parsing, output-offset calculation, and section writing (CVE-2026-52490). The hosted installed regression `tiffcrop -S 134217729:32 -h` returned 0 and displayed usage on each exact baseline, then returned 1 with the `MAX_SECTIONS` limit message on each patched package. The CVE-2025-61143 reference patch guards `TIFFFileName(out)` in a later cleanup path, but is not selected for these source versions pending an exact reproducer.

The output preserves the complete distro source version and appends `+getpatchedN`, where `N` is the target's release serial. Focal remains at `+getpatched2` for CVE-2025-61144; the Debian candidates use `+getpatched3` for CVE-2025-61144 and CVE-2026-52490. Bookworm `+getpatched4` adds the four-commit upstream PixarLog fix series for CVE-2026-12912, limited to Debian Bookworm amd64. The issue is reachable when an application explicitly selects `PIXARLOGDATAFMT_8BITABGR` for RGB decoding; no Ubuntu, Trixie or arm64 claim is made. This candidate remains unverified pending its hosted baseline/patched installed regression, package tests, consumer checks and package-aware scans. Each further patch on the same base increments that target's serial; a changed distro source requires a separately pinned target and fresh evidence. Version ranges alone never establish applicability.

## Build and verification

The platform build runner downloads the exact `.dsc` and source archives, applies the selected backports to distro packaging, and builds target-specific Debian source and binary packages. Installed baseline checks reproduce CVE-2025-61144's level-2 debug sample-buffer issue and CVE-2026-52490's subdivision overflow on both Debian targets. Patched checks pass and the `tiffinfo` consumer check succeeds; unrelated `tiffcrop` Memcheck warnings remain in the reports. The package-aware scan clears both selected CVEs on each candidate. Raw Trivy still reports both CVEs on `libtiff-tools` and unchanged `libtiff6`, with no fixed version in database `05a1fa5372e54059b6a7c1fc41afb812fafd9bd56a8e0ab6aa20d7f3f154ff31`; this mismatch is documented and blocks production release until reconciled or explicitly accepted. CVE-2025-61143 remains an unresolved tracker/source-path discrepancy and is not reported as fixed. No production binaries or APT indexes were published.

Local recipe validation (from a checkout with the platform repository alongside it):

```sh
python3 platform/scripts/build-package.py --source packaging-tiff --target ubuntu-focal-amd64 --output build/tiff-focal
python3 platform/scripts/build-package.py --source packaging-tiff --target debian-bookworm-amd64 --output build/tiff-bookworm
python3 platform/scripts/build-package.py --source packaging-tiff --target debian-trixie-amd64 --output build/tiff-trixie
```

The output is a local candidate only; this recipe does not enable or perform publication.

## Licensing

GetPatched-authored recipe metadata, patches, tests and documentation are MIT-licensed. Upstream TIFF and distribution packaging retain their own copyrights and licenses; preserve the notices in `LICENSES/` in every source and binary distribution.
