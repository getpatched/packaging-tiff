# GetPatched recipe: TIFF

GetPatched maintains selected, tested public Ubuntu and Debian package updates. This repository contains recipe metadata, patches, and regression inputs; it does not duplicate the full upstream source tree.

## Candidate targets

| Target | Pinned source | GetPatched version | CVE | Current status |
|---|---|---|---|---|
| `ubuntu-focal-amd64` | Ubuntu 20.04 · `4.1.0+git191117-2ubuntu0.20.04.14` | `4.1.0+git191117-2ubuntu0.20.04.14+getpatched2` | CVE-2025-61144 | Development candidate |
| `debian-bookworm-amd64` | Debian 12 · `4.5.0-6+deb12u4` | `4.5.0-6+deb12u4+getpatched2` | CVE-2025-61144 | Development candidate |
| `debian-trixie-amd64` | Debian 13 · `4.7.0-3+deb13u3` | `4.7.0-3+deb13u3+getpatched2` | CVE-2025-61144 | Development candidate |

These entries are development candidates, not release claims. All three exact targets have completed hosted builds, installed regression checks, package-aware scans, consumer checks, and isolated signed-client checks. The package-aware scan reports CVE-2025-61144 on each baseline and no finding on each candidate. That result does not mean every scanner is clear: raw Trivy 0.70, using the same database snapshot, still reports CVE-2025-61143 and CVE-2025-61144 on both the Bookworm and Trixie baseline and candidate `libtiff-tools`/`libtiff6` packages, without a fixed version; it has no corresponding Focal finding. Those raw reports are retained in the platform evidence and remain unresolved. Public publication is disabled until this scanner discrepancy is reconciled or explicitly accepted through a documented release decision. The targets use a zero-hour public policy, so publication will be public after required release checks pass; there is no sponsor delay. Debian describes CVE-2025-61144 as a command-line tool crash with no security impact; Ubuntu rates it Low. The scope is bounded `tiffcrop` behavior, not a remote library compromise or a general security guarantee.

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

The active recipe backports upstream [commit 88cf9db](https://gitlab.com/libtiff/libtiff/-/commit/88cf9dbb48f6e172629795ecffae35d5052f68aa), which caps a level-2 debug dump loop at `MAX_SAMPLES`, matching the fixed capacity of its sample-buffer array (CVE-2025-61144). The CVE-2025-61143 reference patch guards `TIFFFileName(out)` in a later cleanup path, but is not selected for these source versions pending an exact reproducer.

The output preserves the complete distro source version and appends `+getpatchedN`, where `N` is the target's release serial. These candidates use `+getpatched2` because `+getpatched1` was already assigned to the earlier CVE-2025-61144 candidate. A further patch on the same base increments the serial; a changed distro source requires a separately pinned target and fresh evidence. Version ranges alone never establish applicability.

## Build and verification

The platform build runner downloads the exact `.dsc` and source archives, applies the selected backports to distro packaging, and builds target-specific Debian source and binary packages. Installed baseline checks require CVE-2025-61144 to reproduce the level-2 debug sample-buffer issue. Patched checks require the target-specific regression signature to disappear and a `tiffinfo` consumer check to pass; unrelated `tiffcrop` Memcheck warnings remain in the reports. The package-aware scan must clear the selected CVE on the candidate. Raw scanner results must also be reconciled or covered by a documented release decision before public release; a clear package-aware result does not erase a raw scanner finding. CVE-2025-61143 remains an unresolved tracker/source-path discrepancy and is not reported as fixed. The release candidate remains ineligible if any required check is missing or fails.

Local recipe validation (from a checkout with the platform repository alongside it):

```sh
python3 platform/scripts/build-package.py --source packaging-tiff --target ubuntu-focal-amd64 --output build/tiff-focal
python3 platform/scripts/build-package.py --source packaging-tiff --target debian-bookworm-amd64 --output build/tiff-bookworm
python3 platform/scripts/build-package.py --source packaging-tiff --target debian-trixie-amd64 --output build/tiff-trixie
```

The output is a local candidate only; this recipe does not enable or perform publication.

## Licensing

GetPatched-authored recipe metadata, patches, tests and documentation are MIT-licensed. Upstream TIFF and distribution packaging retain their own copyrights and licenses; preserve the notices in `LICENSES/` in every source and binary distribution.
