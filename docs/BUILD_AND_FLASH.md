# Gemini-S1 Build and Flash Baseline

Last verified: 2026-09-14 (Asia/Shanghai)

## Source provenance

- Manifest branch: `dev-ai-contest-2026`
- Team manifest revision: `224850b197da7837e60b45573af26a479d366dd9`
- Official docs revision: `cb0389919ea4fc86774702fb36c08ee9b1366e6c`
- Allwinner board support revision: `1676386193f0e710121e710935f1757c0f34b662`
- Pinned manifest: `/home/abby/openvela-workspace/pinned-manifest.xml`
- `repo sync -c -j2`: completed successfully
- `repo status` immediately after the verified build: clean

## Verified paths

Gemini-S1 target configuration:

```text
vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/nsh_minidisplay/defconfig
```

AI Agent defconfig source, reserved for the later AI-enabled build:

```text
packages/ai_agent/defconfigs/gemini-s1/gemini-s1_defconfig
```

Team application mapping:

```text
contest2026_482_xingguangyinli/app/hello_app/
packages/demos/contest2026_482_hello_app/
```

## Clean baseline result

Status: `BASELINE_BUILD_PASSED`

The clean board baseline used the unmodified `nsh_minidisplay` configuration. It did **not** copy the AI Agent defconfig and did **not** run `fix_gemini_s1.sh`; those are separate, source-changing integration steps that require their own before/after status and build evidence.

Verified build command in Ubuntu:

```bash
cd /home/abby/openvela-workspace

QEMU_LD_PREFIX=/usr/x86_64-linux-gnu \
QEMU_SET_ENV=LD_LIBRARY_PATH=/home/abby/.local/share/openvela-x86_64-jammy/root/lib/x86_64-linux-gnu \
./build.sh vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/nsh_minidisplay/ -j2
```

Result:

- Exit code: `0`
- Completed: `2026-09-14 10:24:19 UTC`
- Linker memory report: `13,639,036 B` used in the SDRAM region (`1.42%`)
- Remaining guest root storage after build: about `72 GiB`
- Remaining available guest memory after build: about `7.3 GiB`

Verified outputs:

| Output | Bytes | SHA-256 |
| --- | ---: | --- |
| `nuttx/nuttx` | 7,345,972 | `05566f786bda3b9820adb5f21f6ae6c6dbf6b3796a54c9d2b6bda1eafe4f9996` |
| `nuttx/nuttx.elf` | 155,426,276 | `5fcdf464591975cb59b7bbb9101c7ee0f2fd785a0a4b33adefca04b9c7be4904` |
| `nuttx/nuttx.bin` | 113,143,844 | `c45d7273d17e9de92f469cb63b43c6acd7962ca2abec4e6cb361e46211047f73` |
| `nuttx/vela.bin` | 7,340,976 | `73ad6568dc3c903f0492c339df7d1feca739087fadc442ffc27b6845bc7aca6f` |
| `vendor/allwinnertech/lichee/board/r528s3/gemini-s1_nand/configs/nsh.fex` | 7,340,976 | `73ad6568dc3c903f0492c339df7d1feca739087fadc442ffc27b6845bc7aca6f` |

`nuttx` and `nuttx.elf` were identified as statically linked ARM EABI5 executables. `vela.bin` and the copied board `nsh.fex` have identical size and SHA-256. A local, ignored backup contains `nsh.fex`, the successful build log, and the pinned manifest under `local-setup/backups/gemini-s1-baseline-20260914/`.

## ARM64 compatibility record

Two failures were retained rather than overwritten:

1. The native AArch64 GCC prebuilt initially lacked host `libc++.so.1`; Ubuntu `libc++1` resolved it.
2. The pinned tree invoked x86-64 `jidl_gen_cpp` and later x86-64 strip/objcopy binaries. Ubuntu's `qemu-user-static` binfmt handler plus the amd64 cross-runtime resolved execution. The strip tool also required amd64 `libz.so.1`.

To avoid adding the `amd64` architecture or changing APT sources, Ubuntu Jammy's official `zlib1g_1.2.11.dfsg-2ubuntu9.2_amd64.deb` was checksum-verified and extracted to a user-owned directory. Package SHA-256:

```text
9dc17e51a1be2d9ed63b7b84ef0e4e29c5abe6f1bc62cb03e7181483cce8a2f2
```

`QEMU_SET_ENV` limits the extra `LD_LIBRARY_PATH` to emulated x86-64 programs; native AArch64 build tools do not inherit an incompatible runtime search path.

## Warnings

The successful build contains upstream LTO/type and NAND `memcpy` warnings plus an empty-loadable-segment warning from strip. They did not produce a nonzero exit or prevent image generation. They remain recorded for later upstream review and must not be rewritten as errors or silently removed from evidence.

## Flash gate

Status: `BLOCKED_FOR_FLASH`

Building an image does not authorize flashing it. No flash, erase, repartition, OTA, recovery-mode entry, or device write has been performed. The first flash remains blocked until the physical board/storage revision, authoritative factory recovery image, package hash, supported flashing tool, exact recovery sequence, console/rollback route, stable power, and user-present confirmation are all verified. See `docs/RECOVERY.md`.
