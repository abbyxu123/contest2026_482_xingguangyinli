---
name: gemini-s1-firmware-acceptance
description: Use when preparing, reviewing, or validating an openvela firmware image for the Gemini-S1 portrait display, especially before a physical NAND flash or a contest evidence claim.
---

# Gemini-S1 Firmware Acceptance

Use this development Skill to keep source, build, package, and physical acceptance separate. A successful compile or IMAGEWTY inspection does not prove that the new image booted or that portrait touch works.

## Locate the evidence

1. Identify the exact team-repo commit, openvela manifest revisions, board revision, target defconfig, and previous known-booting image. If any is unknown, record it as unknown instead of borrowing a prior result.
2. From the Living Canvas team-repository root, read `docs/GEMINI_S1_ADAPTATION.md` and dated records under `tests/evidence/`. Resolve the image by its SHA-256, not by a reused filename. If those paths are unavailable, locate the team repository before proceeding.
3. Keep private NAND dumps, credentials, raw UART/ADB data, and generated images out of the contest repository. Publish only sanitized commands, checksums, outcomes, and evidence needed to reproduce the claim.

## Verify in increasing order of risk

1. Run host tests, UI-build checks, and board patch validation. Record each exit status. Build the `r528s3-gemini-s1/configs/nsh_minidisplay` target in the pinned openvela workspace; verify `living_canvas_main` is linked and hash `nuttx.elf` and `nsh.fex`.
2. Build and verify sanitized `usrdata.fex` with the repository scripts. Exclude the vendor demo Wi-Fi profile, and verify its absence before packaging. Do not copy a live board's user data into a public image.
3. Package with the vendor layout. When invoking `dragon` directly, supply **both** `image.cfg` and `sys_partition_for_dragon.fex`; calling it with only `image.cfg` can report success while omitting the application partitions. Independently inspect the resulting IMAGEWTY file, its embedded payloads and MBR. For the current factory layout, expect 22 embedded files and 10 MBR partitions; compare the exact layout with the known-booting image rather than relying on size alone.
4. Hash the image, unpack it, and compare embedded `nsh.fex` and `usrdata.fex` byte-for-byte with the standalone candidates. Explain every payload or partition difference from the previously booted image.

## Physical acceptance boundary

Only after offline checks pass, ask an operator to connect the board. Identify exactly one intended FEL device and confirm stable power, recovery image, NAND backup, target image hash, and the flash command. A full erase changes persistent storage; do not run it from an ambiguous device or image state. After verified write and reboot, record NAND boot, 240×320 upright display, and each touch target separately. For the two-tap choice mode, verify first tap selects, second same-card tap confirms, and switching cards clears confirmation. Treat network, audio, QR readability, and `ai_agent` runtime as separate tests.

Record the strongest completed gate only: `HOST_TESTED`, `TARGET_BUILT`, `IMAGE_INSPECTED`, `FLASH_VERIFIED`, `BOOT_VERIFIED`, or `TOUCH_VERIFIED`. Never promote a claim based on an earlier image's result.
