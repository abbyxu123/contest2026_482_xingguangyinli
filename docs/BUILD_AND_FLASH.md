# Gemini-S1 Build and Flash Baseline

Last reviewed: 2026-09-14 (Asia/Shanghai)

## Source provenance

- Team manifest: `224850b197da7837e60b45573af26a479d366dd9`
- Official docs: `cb0389919ea4fc86774702fb36c08ee9b1366e6c`
- Allwinner board support: `1676386193f0e710121e710935f1757c0f34b662`

The complete source workspace has not been synchronized, so individual project revisions and build outputs are not yet pinned.

## Verified paths from official sources

Gemini-S1 AI Agent defconfig source:

```text
packages/ai_agent/defconfigs/gemini-s1/gemini-s1_defconfig
```

Gemini-S1 target config:

```text
vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/nsh_minidisplay/defconfig
```

Build-tree mapping for this team's application:

```text
contest2026_482_xingguangyinli/app/hello_app/
packages/demos/contest2026_482_hello_app/
```

The official base manifest lists Linux AArch64 prebuilts for GCC, CMake, build tools, emulator, and QEMU. Their actual file architecture must still be verified with `file` after synchronization; manifest names alone are not proof of compatibility.

## Planned baseline build

These commands are recorded for review and are not yet executed:

```bash
cp packages/ai_agent/defconfigs/gemini-s1/gemini-s1_defconfig \
  vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/nsh_minidisplay/defconfig

bash packages/ai_agent/fix_gemini_s1.sh

./build.sh vendor/allwinnertech/boards/r528/r528s3-gemini-s1/configs/nsh_minidisplay/ \
  -e -Wno-error -j2
```

Before running the fix script, review its exact pinned content, record a clean multi-repository status, and verify each expected patch result. A zero exit code alone is insufficient because the script may warn and skip a patch.

The official guide uses all available cores; this project intentionally uses `-j2` to protect the 16 GB host Mac and 8 GB guest.

## Required build evidence

For each build attempt, preserve:

- pinned manifest and team repository commit;
- exact command and UTC timestamp;
- host architecture and tool versions;
- exit code and relevant warnings/errors;
- output image path, byte size, and SHA-256;
- repository status before and after fix scripts;
- peak disk usage and remaining free space.

Do not claim a successful baseline without a zero build exit code and a real output image.

## Flash gate

Status: `BLOCKED_FOR_FLASH`

No flash command is documented here because the factory recovery image, physical-unit storage layout, recovery key sequence, supported programming tool, and rollback procedure are not verified. See `docs/RECOVERY.md` for the complete gate.

Building an image does not authorize flashing it. The first irreversible device operation must happen only with the user present after target, image hash, recovery path, power, and console access are confirmed.
