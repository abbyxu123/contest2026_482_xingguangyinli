# Repository and Submission Map

Last verified: 2026-09-14 (Asia/Shanghai)

## Repository roles

| Role | Repository | Branch / revision | Status |
| --- | --- | --- | --- |
| Official team manifest and submission target | `https://github.com/open-vela/contest2026_482_xingguangyinli.git` | `dev-ai-contest-2026` at `224850b197da7837e60b45573af26a479d366dd9` | Read access verified |
| Personal planning and backup repository | `https://github.com/abbyxu123/xiaomi-Living-Canvas.git` | `main` at `83d6560af8b7043bcf9b8132f2af2264168270db` | Read access verified |
| Official docs | `https://github.com/open-vela/docs.git` | `dev-ai-contest-2026` at `cb0389919ea4fc86774702fb36c08ee9b1366e6c` | Read access verified |
| Official Allwinner board support | `https://github.com/open-vela/vendor_allwinnertech.git` | `dev-ai-contest-2026` at `1676386193f0e710121e710935f1757c0f34b662` | Read access verified |

The personal repository and the official contest repository have different purposes. The personal repository is not a substitute for the contest submission target.

## Current local integration state

- The original desktop `main` remains unchanged.
- Work is isolated on local branch `codex/environment-bootstrap` in a Git worktree.
- Local planning history and personal `origin/main` were merged only in that isolated branch.
- No branch has been pushed by this setup process.
- The official contest repository has been inspected read-only in a temporary shallow clone.

## Official manifest mapping

The team manifest maps only the team's repository into the openvela source tree:

| Team source path | Build-tree destination |
| --- | --- |
| `contest2026_482_xingguangyinli/app/hello_app` | `packages/demos/contest2026_482_hello_app` |
| `contest2026_482_xingguangyinli/quickapp/hello_quickapp` | `packages/apps/contest2026_482_hello_quickapp` |
| `contest2026_482_xingguangyinli/board/contest_board` | `vendor/openvela/boards/contest2026_482_board` |

Product code should be authored in the team repository paths, not copied into shared upstream repositories.

## Submission path still requiring user presence

The official guide requires the registration GitHub account to accept the collaborator invitation, fork the official team repository, push a development branch, open a pull request back to the official team repository, satisfy the CLA check, and merge the reviewed pull request.

The public fork `https://github.com/abbyxu123/contest2026_482_xingguangyinli` was not confirmed during this audit. Do not create a fork, accept an invitation, sign a CLA, push, or open a pull request without the user present.

Status: `BLOCKED_FOR_GITHUB_ACCOUNT_ACTION`

## Full openvela workspace gate

The official source workspace has not been synchronized yet. The guest currently has about 46 GiB free, while the official minimum is 40 GB before source and build artifacts. This leaves too little recovery margin. Required Ubuntu packages and Git LFS are also not yet installed.

Before `repo sync`:

1. Increase the Ubuntu virtual disk or attach a dedicated virtual disk with adequate headroom.
2. Install and verify required packages, preferably from Ubuntu repositories where available.
3. Install `repo` in a user-owned bin directory and record its checksum/version.
4. Run `repo init` with the official team manifest and `--git-lfs`.
5. Use `repo sync -c -j2` to limit RAM and network pressure on the 16 GB Mac.

Status: `BLOCKED_FOR_DISK_AND_DEPENDENCIES`
