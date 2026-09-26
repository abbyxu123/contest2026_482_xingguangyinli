# Gemini-S1 开发体验与工程建议

最后核验：2026-09-27（Asia/Shanghai）

本文只记录可复核的 Gemini-S1/openvela 开发观察与改进建议，不包含账号、
凭据、私人环境路径或未经验证的平台结论。

## 已复核的工程观察

1. 目标固件已在 Apple Silicon macOS 主机完成完整 NAND 写入、逐阶段校验、
   自动重启和 openvela 启动；因此本次后期问题不属于“镜像无法写入”。
2. 原配置同时提供 DISP2 framebuffer 与 NuttX LCD framebuffer，而 Gemini-S1
   的 ILI9341 面板实际使用硬件 SPI。链接器优先采用 DISP2 `dev_fb` 后，应用
   能向内存 framebuffer 绘制，但像素不能通过未接出的并口/TCON 数据线到达
   面板。
3. 将 SPI ILI9341 配置切换到 NuttX `lcd_framebuffer + ili9341_lcd_spi` 后，
   `/dev/fb0` 报告 RGB565 320×240，官方 LVGL widgets、触摸事件和 Living
   Canvas 均已在实体屏显示。
4. 当前验证配置为横屏，最终产品需要继续完成 240×320 竖屏与触摸坐标校准。

## 建议一：提供单一、明确的 Gemini-S1 显示配置

- 当 `CONFIG_LCD_ILI9341_HARDWARE_SPI=y` 时，避免同时让 DISP2 `dev_fb` 和
  NuttX LCD framebuffer 竞争 `up_fbinitialize` 等符号。
- 在官方 Gemini-S1 配置中明确选择 SPI LCD 数据路径，并把竖屏/横屏作为一组
  可核验的预设，而不是仅由多个分散的 Kconfig 项共同决定。
- 移除或显式标注启动阶段的面板测试图，避免它被误认为应用已经显示或屏幕损坏。

## 建议二：提供最小化显示与触摸验收程序

建议随板提供一个不依赖网络和模型的 smoke test，依次显示纯色、坐标网格、
RGB565 色块和触摸落点，并输出：

- framebuffer 提供者与像素格式；
- 分辨率、stride、buffer 长度；
- ILI9341 方向/MADCTL 配置；
- 触摸原始坐标与映射坐标；
- 每次刷新返回值和 SPI 传输错误。

这样可以把“镜像写入、系统启动、显示初始化、像素刷新、触摸映射”五个阶段明确
分开，降低跨主机问题排查成本。

## 建议三：完善跨平台烧录与恢复说明

- 为 Windows x64 与 Apple Silicon macOS 提供签名或可校验的官方 CLI 包；
- 在同一页明确镜像格式、FEL 进入/退出方式、UART 端口与 1,500,000 波特率；
- 烧录工具输出分阶段状态，并在写入前支持只读芯片/NAND/布局检查；
- 提供可验证的恢复镜像、完整哈希和故障矩阵，区分 FEL、FES、U-Boot、存储、
  openvela 与应用层问题。

Windows 侧本项目保留了准备包和操作方案，但当前记录不足以形成与 macOS 同等级的
完整实机结论，因此本文不把 Windows 现象写成已确认的平台缺陷。

## 建议四：把可复现信息纳入标准诊断输出

建议增加一条一键诊断命令，输出板卡版本、芯片/存储身份、构建 ID、关键 Kconfig、
framebuffer 符号提供者、面板方向、SPI 端口与触摸设备。输出默认脱敏，便于开发者
直接附到 issue 或比赛技术材料中。
