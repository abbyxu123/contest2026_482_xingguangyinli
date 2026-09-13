# Gemini-S1 Recovery and Flash Gate

Last reviewed: 2026-09-14 (Asia/Shanghai)

## Current decision

`BLOCKED_FOR_FLASH`

Do not flash, erase, repartition, boot an alternate image, enter an undocumented recovery mode, or run a vendor programming tool against Gemini-S1 serial `1234`.

## What is verified

- Board family: Gemini-S1 / Allwinner R528S3.
- Official board config exists at `vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/nsh_minidisplay/`.
- The official AI Agent guide provides a Gemini-S1 defconfig and build flow.
- The official board README describes compilation and points to generic packaging/deployment documentation.
- USB currently enumerates as NuttX Debug Bridge `18d1:4e11`, serial `1234`.

## Public recovery-source audit

- The pinned BSP `build/` directory exposes a build config, `pre_build.sh`, and `generate_ota_package.sh`; it does not expose a host flashing utility or board recovery procedure.
- The OTA generator blob `c1688e424daf6fbdcbaa0fb2aae081f0d12dac0d` invokes the shared `lichee/pack.sh`, refers to `r528s3/evb4_nand`, and can download an externally supplied `ota_base_url`. It generates full/OTA artifacts but does not establish how to restore this physical Gemini-S1 safely.
- The public tree also contains `lichee/board/r528s3/gemini-s1_nand/configs/` with NAND and `_nor` variants. Repository filenames are not proof of the storage fitted to serial `1234`.
- The vendor README delegates complete packing and deployment instructions to board/vendor documentation. The linked vendor wiki was not readable in this environment, so its contents are not treated as verified.

## Missing recovery prerequisites

- Factory image or board-matched recovery package: **not acquired**.
- SHA-256 and provenance for a recovery package: **unknown**.
- Exact hardware revision and storage layout of this physical unit: **unknown**.
- Documented recovery/FEL key sequence for this unit: **unknown**.
- Supported macOS/Linux flashing utility and exact version: **unknown**.
- Verified rollback test from a disposable or vendor-approved image: **not performed**.
- Stable serial-console access and boot log capture: **not established**.
- Official confirmation that an image built from the contest branch is compatible with this shipped unit: **not recorded**.

Use `docs/VENDOR_RECOVERY_QUESTIONS.md` as the minimum support request; do not replace missing technical answers with assumptions.

## Flash authorization gate

First flash is allowed only after all of the following are true:

1. The physical board revision and storage type are recorded.
2. A factory or vendor-approved recovery image is obtained from an authoritative source.
3. Package hash, flashing tool, cable, power requirements, recovery entry, and rollback steps are documented.
4. The contest image builds successfully from pinned source with a recorded hash.
5. The image target exactly matches the physical board.
6. A stable console or other recovery-visible channel is available.
7. The user is present for the irreversible step and explicitly confirms the resolved target and image.

## Pinned documentation reviewed

- Team manifest branch `dev-ai-contest-2026`: `224850b197da7837e60b45573af26a479d366dd9`
- Official docs branch `dev-ai-contest-2026`: `cb0389919ea4fc86774702fb36c08ee9b1366e6c`
- Allwinner vendor branch `dev-ai-contest-2026`: `1676386193f0e710121e710935f1757c0f34b662`

These revisions establish source provenance only. They do not by themselves authorize flashing.
