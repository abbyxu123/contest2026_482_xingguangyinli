#!/usr/bin/env python3
"""Verify that the recorded Gemini-S1 SPI LCD patch keeps its key guards."""

from __future__ import annotations

import argparse
from pathlib import Path


def require(text: str, token: str, description: str) -> None:
    if token not in text:
        raise AssertionError(f"missing {description}: {token}")


def verify_patch(path: Path) -> None:
    patch = path.read_text(encoding="utf-8")

    require(
        patch,
        "ifneq ($(CONFIG_LCD_ILI9341_HARDWARE_SPI),y)",
        "Make guard excluding the DISP2 framebuffer for SPI ILI9341",
    )
    require(
        patch,
        "if(CONFIG_VIDEO_FB AND NOT CONFIG_LCD_ILI9341_HARDWARE_SPI)",
        "CMake guard excluding the DISP2 framebuffer for SPI ILI9341",
    )
    require(
        patch,
        "-CONFIG_LCD_EXTERNINIT=y",
        "removal of external LCD initialization mode",
    )
    require(
        patch,
        "+# CONFIG_LCD_EXTERNINIT is not set",
        "generic LCD framebuffer board initialization mode",
    )
    require(
        patch,
        "void board_lcd_uninitialize(void)",
        "board LCD teardown entry point",
    )
    require(
        patch,
        "g_lcddev->setpower(g_lcddev, 0)",
        "display power-down during teardown",
    )
    require(
        patch,
        "g_ili9341_lcd_buffer = NULL",
        "transfer buffer reset during teardown",
    )
    require(
        patch,
        "return -ENODEV;",
        "negative board initialization failure status",
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("patch", type=Path)
    args = parser.parse_args()

    try:
        verify_patch(args.patch)
    except (AssertionError, OSError) as error:
        print(f"FAIL: {error}")
        return 1

    print("PASS: Gemini-S1 SPI LCD uses the generic LCD framebuffer path")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
