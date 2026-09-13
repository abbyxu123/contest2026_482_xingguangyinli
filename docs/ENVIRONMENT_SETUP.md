# Living Canvas Development Environment

Last verified: 2026-09-14 (Asia/Shanghai)

This document records observed state. It does not claim that openvela has been synchronized, built, run, or flashed.

## Host Mac

- Computer: 2021 16-inch MacBook Pro
- SoC: Apple M1 Pro
- Memory: 16 GB
- macOS: Ventura 13.2.1
- Free host storage observed during setup: about 262 GiB
- Project path: `/Users/<USER>/Desktop/xiaomi-openvela-ibbie`

The project directory was renamed to ASCII-only `xiaomi-openvela-ibbie` to reduce quoting and tool-path risk. Local installers, VM data, platform-tools, source workspaces, and backups remain below ignored `local-setup/` or other non-repository locations.

## UTM guest

- VM name: `LivingCanvas-Ubuntu`
- UTM application: 4.7.5, code signature previously verified
- Guest OS: Ubuntu Server 22.04.5 LTS
- Architecture: `aarch64`
- Kernel: `5.15.0-191-generic`
- CPU: 4 virtual cores
- RAM: 8 GB configured; guest reports 7.7 GiB
- Swap: 3.8 GiB
- Root filesystem: 55 GiB total, 46 GiB available at last check
- Virtual disk: 64 GiB (`vda`); LVM partition about 60.9 GiB and root logical volume 56 GiB
- Estimated unused LVM capacity: about 4.9 GiB; exact extent data requires user-present sudo access
- Network: UTM shared network; guest address was `192.168.64.2` at last check and may change after restart
- Access: SSH public-key authentication for the development session; passwordless sudo is not configured

The installer ISO is ejected. The full Mac project is not shared into the VM. Source must live on the Ubuntu virtual disk or a dedicated virtual disk, not in a macOS shared folder.

Ubuntu Server's text console is expected and does not indicate failure. A desktop environment is unnecessary for openvela builds and would consume limited RAM and disk.

## Verified guest tools

| Tool | State |
| --- | --- |
| Git | 2.34.1 |
| Python | 3.10.12 |
| curl | 7.81.0 |
| OpenSSH server | installed and reachable |
| CMake | missing |
| GCC / build-essential | missing |
| Git LFS | missing |
| repo | missing |

Evidence: `tests/evidence/build/ubuntu-host-baseline-20260913T185829Z.txt`.

## Official requirements and local constraints

The pinned official Ubuntu guide supports Ubuntu 22.04 on arm64 or x86_64 and requests at least 40 GB free disk and 16 GB RAM. The host Mac itself has only 16 GB, so assigning 16 GB to the guest is not safe. The chosen guest allocation is 8 GB plus swap, and build/sync concurrency will be limited to `-j2`.

A read-only `lsblk` check confirmed that the current 64 GiB virtual disk is already fully partitioned. Extending the existing logical volume alone would add only about 4.9 GiB, which does not resolve the workspace headroom risk. Any UTM disk resize must therefore happen with the VM shut down and the user present, followed by separately verified partition, PV, LV, and filesystem growth; no unattended storage mutation is authorized.

The current 46 GiB free root filesystem exceeds the stated 40 GB minimum by only about 6 GiB. That is not enough safety margin for source, LFS objects, build artifacts, retries, and logs. Full `repo sync` is blocked until the virtual storage plan is expanded and verified.

Status: `BLOCKED_FOR_DISK_AND_DEPENDENCIES`

## Next user-present system step

Run inside Ubuntu after checking the command on screen. Do not paste the password into chat or a project file:

```bash
sudo apt update
sudo apt install -y git curl cmake python3 libc++abi-dev build-essential git-lfs
git lfs install
```

Using Ubuntu's `git-lfs` package is preferred initially over piping a remote repository-setup script into `sudo`. After installation, record versions before syncing source.

Install the `repo` launcher into a user-owned directory such as `~/.local/bin`, verify its source and version, and avoid a system-wide move unless necessary.

## Mac-side device tooling

- Android platform-tools 37.0.1 is isolated below `local-setup/platform-tools/`.
- Archive SHA-256: `ee39ad5967e95c2a07f04dbcbde96b1a0c916ba376096db5d2f498b7727a5d1d`.
- Gemini-S1 serial `1234` is visible to ADB.
- ADB shell command transport is not yet verified and is not used for baseline collection.

See `docs/DEVICE_BASELINE.md` and `docs/RECOVERY.md`.

## Acceptance sequence

1. Expand or attach Ubuntu storage and verify the resulting filesystem capacity.
2. Install missing packages with the user present and record versions.
3. Install and verify a user-local `repo` launcher.
4. Initialize the official team manifest with Git LFS enabled.
5. Sync with `repo sync -c -j2` and record the pinned manifest.
6. Verify that Linux ARM64 prebuilts are actual `aarch64` executables.
7. Build an official baseline at `-j2`.
8. Keep flashing blocked until `docs/RECOVERY.md` is fully resolved.

## References

- [Official Ubuntu quick start](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/quickstart/openvela_ubuntu_quick_start.md)
- [Official team manifest](https://github.com/open-vela/contest2026_482_xingguangyinli/blob/dev-ai-contest-2026/contest2026_482_xingguangyinli.xml)
- [Gemini-S1 board README](https://github.com/open-vela/vendor_allwinnertech/blob/dev-ai-contest-2026/boards/r528/r528s3-gemini-s1/README_zh-cn.md)
- [AI Agent quick start](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/ai_hardware/ai_agent_quickstart.md)
