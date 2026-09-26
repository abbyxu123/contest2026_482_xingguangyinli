# Gemini-S1 / openvela 适配状态

最后核验：2026-09-26（Asia/Shanghai）

## 平台定位

Gemini-S1（Allwinner R528S3）是 Living Canvas 的正式目标主控，目标软件栈为 openvela、LVGL 与 ai_agent。仓库中的应用入口、manifest 映射、Agent 安全桥接、自定义 Skill 和目标构建均围绕这条路线维护。

本页把“源码与构建已经完成”和“实体板端运行已经验证”分开记录。构建成功不等同于已经刷入设备；辅助交互原型也不等同于 openvela 板端运行证据。

## 已验证事项

| 层级 | 结果 | 可复核证据 |
| --- | --- | --- |
| 应用逻辑 | 12 个严格 C 主机测试通过 | `app/hello_app/tests/host/` |
| openvela 映射 | 应用映射到 `packages/demos/contest2026_482_hello_app/` | `contest2026_482_xingguangyinli.xml`、`app/hello_app/` |
| 目标构建 | Gemini-S1 产品配置构建退出状态为 0，`nuttx.elf` 含 `living_canvas_main` | `tests/evidence/build/gemini-s1-product-20260918.txt` |
| 镜像封装 | 目标应用分区和 128 MB NAND LiveSuit 整包生成并校验 | `tests/evidence/build/gemini-s1-product-20260918.txt` |
| 设备基线 | USB/ADB 身份和板载麦克风非内容信号已验证 | `tests/evidence/device/` |
| 恢复通道 | FEL 身份、R528/T113 芯片 ID、Winbond 256 MiB SPI NAND 已识别 | `tests/evidence/device/gemini-s1-fel-spinand-20260920.txt` |
| 写入前保护 | 完整 SPI NAND 只读备份已生成并校验 | `tests/evidence/device/gemini-s1-fel-spinand-20260920.txt` |
| macOS 部署复核 | Apple Silicon macOS 上已完成镜像检查、FEL、完整 NAND 写入、逐阶段校验、自动重启与 openvela 启动 | `tests/evidence/device/gemini-s1-macos-retest-20260925.txt`、`tests/evidence/device/gemini-s1-full-flash-runtime-20260926.txt` |
| 完整 NAND 烧录 | 128 MiB 整包完成全擦写、分阶段校验、Boot0/Boot1 写入与自动重启 | `tests/evidence/device/gemini-s1-full-flash-runtime-20260926.txt` |
| openvela 真机启动 | 烧录后从 NAND 启动并进入 UART2 NSH；目标符号 `living_canvas_main` 已链接 | `tests/evidence/device/gemini-s1-full-flash-runtime-20260926.txt` |
| LVGL 与输入设备 | SPI LCD framebuffer 报告 RGB565 320×240；`/dev/fb0` 与 `/dev/input0` 打开成功，官方 widgets 与触摸事件均在实体屏验证 | `tests/evidence/device/gemini-s1-spi-display-runtime-20260927.txt` |
| Living Canvas 实体屏 | 作品画面、决策覆盖层与二维码组件已在 Gemini-S1 实体屏显示并循环；最终竖屏与触摸坐标继续校准 | `tests/evidence/device/gemini-s1-spi-display-runtime-20260927.txt` |

## 2026-09-27 SPI 屏与作品真机验证

Gemini-S1 已完成第二轮完整 NAND 烧录与逐阶段校验，并在自动重启后从 NAND
进入 openvela NSH。链接映射确认 `/dev/fb0` 已由 NuttX `lcd_framebuffer`
提供，板级初始化由 `ili9341_lcd_spi` 提供，不再由未连接到 SPI 面板的 DISP2
framebuffer 提供。

新运行时报告 RGB565、320×240、16 bpp、153,600 字节 framebuffer 和 640
字节 stride。官方 LVGL widgets 已在实体屏显示；触摸日期控件后，日期选择器
在实体屏弹出，完成从输入事件到画面刷新的端到端验证。随后在干净重启后启动
Living Canvas 比赛循环，作品背景、决策覆盖层和二维码组件均已在 Gemini-S1
实体屏显示。

当前验证配置仍是 320×240 横屏，最终画框需要 240×320 竖屏与触摸坐标校准；
工程验证二维码也不是最终公开手机交接页。两项均保留为独立产品校准门禁，不影响
本次 SPI 显示、触摸与作品真机运行结论。完整脱敏记录见
`tests/evidence/device/gemini-s1-spi-display-runtime-20260927.txt`。

## 历史适配边界（2026-09-25）

首次分区写入流程曾在 FES DRAM 初始化阶段超时，流程在进入存储、MBR 和分区写入阶段之前自动停止。停止后 FEL 芯片身份仍可读取，未发生持久化 NAND 写入。

2026 年 9 月 25 日在 Apple Silicon macOS 上再次执行同范围验证，结果复现：镜像 SHA-256 与结构检查通过，FEL 与 SPI NAND 只读链路稳定；写入工具加载 FES 后连续 60 次未能完成 DRAM 就绪检查。操作前后的三个 NAND 采样点逐字节一致，因此该次测试仍不记为持久化写入或板端运行成功。

同日重新枚举 USB 后继续执行受控验证，FES DRAM 初始化连续两次在首次状态读取时通过，返回 `0x4d415244`（`DRAM`）成功标志和参数更新标志。随后 U-Boot、DTB 占位项与系统配置完成内存传输，U-Boot 执行请求成功；设备离开 FEL，但在 45 秒门限内没有枚举成 FES/SRV 设备。工具侧同时补充了 FES 返回参数向 U-Boot 的传递及回归测试；完整可执行测试集 197 项通过。应用该修正后的实板结果仍停在 FES 重枚举门禁，因此没有进入存储查询、MBR、擦除或分区下载阶段。

截至 2026-09-25 可以准确陈述：

- Living Canvas 的 Gemini-S1/openvela 目标源码、主机逻辑、目标构建和镜像封装已经完成并留下证据；
- Gemini-S1 的 USB/ADB、麦克风、FEL 和 SPI NAND 基线已经实板验证；
- Gemini-S1 的首次持久化写入和启动当时仍在适配验证中；该门禁已由 2026-09-26 的完整烧录与启动证据更新；
- 当前不把尚未完成的板端运行描述为已完成，也不把其他主控的演示结果替代为 openvela 运行结果。

该阶段的问题曾从 DRAM 初始化收敛到 U-Boot/FES 启动与 USB 重枚举链路。2026-09-26 已通过完整烧录、校验和 NAND 启动越过此门禁；历史记录继续保留用于复核。

## 代码完成度与运行门禁

应用侧已经具备：

- 存在事件触发的主动问候和冷却控制；
- 晚餐约束收集、最多三个候选和信息不足时追问；
- 模型输出到本地动作之间的白名单与二次校验；
- 用户明确确认后的手机交接、灯光意图和可删除偏好记忆；
- 网络、超时、取消、迟到回复和时钟未同步的确定性回退；
- LVGL 状态、角色反馈、选择与二维码界面；
- 按 ai_agent 官方 Markdown 格式编写、目标部署到
  `/data/agent/skills/dinner-assistant.md` 的 Dinner Assistant Skill。

板端验收继续按以下门禁逐项完成：

1. 核对板卡版本、烧录专用 U-Boot/FES 组合、恢复工具和可回滚镜像（已完成）；
2. 在可恢复条件下完成受控写入并校验分区（已完成）；
3. 通过串口确认 openvela 启动（已完成）；
4. 分别验证 LVGL 显示与触摸（已完成），继续验证音频播放、网络和 ai_agent；
5. 完成从存在事件到确认、执行和手机交接的实体板端演示；
6. 将脱敏结果补入 `tests/evidence/`。

## 辅助交互原型

仓库保留一个 ESP32-S3 辅助交互原型，用来复核竖屏布局、触摸选择和产品流程。它有独立的源码与自动化测试，但不属于 Gemini-S1/openvela 运行证据，不改变正式目标平台和上述验收门禁。
