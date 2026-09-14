# 画间 Living Canvas

**一幅会生活、也会在需要时帮你做决定的交互画。**

画间 Living Canvas 由星光引力 Starlight Gravity 发起。项目把实体绘画、局部动态屏幕、原创角色与设备端 AI Agent 组合成家居艺术装置。第一条端到端体验聚焦晚餐：收集预算和明确限制，给出不超过三个可执行候选，等待用户确认，再把确认结果用于画面、灯光和可删除的偏好记忆。

当前开发节点为 2026 首届 openvela AI 硬件开发者大赛。主控目标是 Gemini-S1（Allwinner R528S3），软件路线为 openvela、LVGL 与 ai_agent。

> 状态更新：2026-09-14。完整 openvela 工作区已同步；未修改基线与 AI Agent + Living Canvas 产品配置均已在 Ubuntu ARM64 上构建成功。构建日志确认六个 Living Canvas 源文件参与编译，ELF 含 `living_canvas_main`。纯 C 状态机、指针钟数学、晚餐约束、确认记忆和 UI 视图模型已通过 macOS 主机测试及 AddressSanitizer/UBSan。真机画面、语音运行时、外设联动与首次刷机仍需逐项验证；本文不会把这些规划写成已实现结果。

## 一、作品简介

画中的猫和朋友平时安静生活；用户需要时，可以通过触摸或语音开始晚餐协商。系统将当天意愿与长期偏好分开处理：临时的“不吃辣”不会自动变成永久记忆，只有用户确认后，明确选择保存的稳定偏好才会写入本地存储。

核心安全边界：

- 模型只返回受限结构化结果，不能直接开灯、写记忆、下单、运行设备命令或处理付款。
- 过敏、预算、是否做饭和临时口味在本地再次校验。
- 推荐最多三个候选；信息不足时追问，不编造价格、库存、配料或配送时间。
- 未同步时间显示 `--:--`，不伪造时刻。
- 网络错误、取消和重复存在事件都有确定的退出/冷却路径。
- 凭据不进入源码、文档、测试证据或 AI Coding 日志。

## 二、选题方向

**AI 硬件产品创新。**

项目重点不是把聊天界面搬到屏幕，而是让 AI 的结果经过本地约束与用户确认后，安全地驱动画面、声音和低压实体交互。Gemini-S1 始终作为主控；毫米波、触摸和摄像头等外设逐项验证，未验证的能力不会进入演示承诺。

## 三、目录结构

```text
app/hello_app/
  include/                 固定边界的核心接口
  src/                     纯 C 状态、时钟、晚餐、记忆和 UI 模型
  skills/dinner_assistant/ 晚餐运行时合同
  tests/host/              可在 Mac/Linux 重复运行的主机测试
docs/                      环境、设备基线、恢复与构建门禁
scripts/device/            只读 USB/ADB 身份与证据脚本
tests/evidence/            脱敏的真实环境和设备证据
logs/                      经人工审查后提交的赛事 AI Coding 日志
```

赛事 manifest 将 `app/hello_app/` 映射到：

```text
packages/demos/contest2026_482_hello_app/
```

## 四、运行方式

### 1. 当前可重复的主机测试

在本仓根目录执行：

```bash
make -C app/hello_app/tests/host clean test
bash app/hello_app/tests/host/test_build_metadata.sh
bash app/hello_app/tests/host/test_lc_ui_build.sh
```

测试覆盖：

- `IDLE → GREETING → LISTENING → THINKING → RECOMMENDATION → CONFIRMED`；
- 取消、网络错误恢复、确认写入保护和存在事件冷却；
- 00:00、03:00、06:30、12:00、23:59 的时针/分针角度；
- 未同步时间不显示伪造时刻；
- 忌口、预算、临时不辣和“不想做饭”的过滤；
- 未知或高权限模型动作被拒绝；
- 只有确认后的稳定偏好写入，支持清除和版本化文件往返；
- UI 状态、猫咪睁闭眼和二维码可见性。

### 2. 设备身份检查

macOS 上只读执行：

```bash
bash scripts/device/verify_usb_inventory.sh
```

Gemini-S1 必须唯一匹配序列号 `1234` 与 USB 身份 `18d1:4e11 / NuttX / Debug Bridge`。外设断开时会明确显示 `MISSING`；`--require-all` 仅用于完整硬件盘点。

### 3. openvela 构建与真机

官方未修改基线以及 AI Agent + Living Canvas 产品固件均已构建成功；最新固件已包含受限录音会话状态机和经过主入口编译执行测试的 `living_canvas_main`，真机刷写尚未执行。准确的环境状态、官方配置路径和构建证据见：

- `docs/ENVIRONMENT_SETUP.md`
- `docs/BUILD_AND_FLASH.md`
- `docs/DEVICE_BASELINE.md`
- `docs/RECOVERY.md`

当前状态为 `PRODUCT_BUILD_PASSED / BLOCKED_FOR_FLASH`。在取得厂商/官方恢复包、确认硬件版本和回滚流程之前，不执行首次刷机。

## 五、AI Coding 使用说明

AI 协作目前用于需求拆解、风险边界、官方资料核对、测试先行实现、失败证据保留、隐私扫描和文档整理。每个核心模块先观察预期失败，再写最小实现，并同时运行严格编译警告与 sanitizer。

赛事日志采集器尚未安装。完整 `.repo/` 工作区已经建立，但官方采集字段可能包含对话、思考、工具输入与输出，因此仍须先审阅全局安装影响，并使用一个新的项目专用会话验证。任何含密码、API Key、私人路径、个人文件或无关对话的会话都不得提交。详情见 `docs/LOGGING_SETUP.md`。

## 六、当前验证状态

| 能力 | 状态 | 证据 |
| --- | --- | --- |
| Gemini-S1 USB/ADB 身份 | 已验证（只读） | `tests/evidence/device/` |
| Ubuntu 22.04 ARM64、4 核、8 GB | 已验证 | `tests/evidence/build/` |
| 核心纯 C 逻辑 | 主机测试通过 | `app/hello_app/tests/host/` |
| openvela 全量同步与 ARM64/兼容主机工具 | 已验证 | `docs/ENVIRONMENT_SETUP.md` |
| AI Agent + Living Canvas 固件构建 | 已通过（含受限录音入口） | `tests/evidence/build/gemini-s1-living-canvas-voice-20260914.txt` |
| LVGL 真机画面、录放音、ai_agent 运行时 | 未上板验证 | 后续真实证据 |
| 毫米波、MPR121、灯光、摄像头 | 未接入 | 外设保持断开 |
| 未修改 Gemini-S1 基线构建 | 已通过 | `docs/BUILD_AND_FLASH.md` |
| 首次刷机 | 阻塞 | `docs/BUILD_AND_FLASH.md`、`docs/RECOVERY.md` |

## 七、隐私与许可证

- 不提交 Wi-Fi 密码、模型 Token、Ubuntu 密码、SSH 私钥、家庭原始影像或付款信息。
- 用户记忆须可查看、纠正和删除；临时意愿与长期偏好分开保存。
- 宠物快报只有在真实图像和时间证据存在时才描述事件。
- 第三方字体、图片、音频和代码在进入最终演示前逐项记录来源与授权。

## 参考

- [openvela 官方文档](https://github.com/open-vela/docs)
- [AI 硬件赛道教程](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/ai_hardware/ai_hardware_guide_index.md)
- [Gemini-S1 板级说明](https://github.com/open-vela/vendor_allwinnertech/blob/dev-ai-contest-2026/boards/r528/r528s3-gemini-s1/README_zh-cn.md)
- [本队官方赛事仓](https://github.com/open-vela/contest2026_482_xingguangyinli)
