#include "lc_display.h"

#ifdef __NuttX__

#  include <nuttx/config.h>

#  include <stdbool.h>
#  include <stdint.h>
#  include <stdio.h>
#  include <sys/boardctl.h>
#  include <unistd.h>

#  include <lvgl/lvgl.h>

#  undef NEED_BOARDINIT

#  if defined(CONFIG_BOARDCTL) && !defined(CONFIG_NSH_ARCHINIT)
#    define NEED_BOARDINIT 1
#  endif

static void configure_label(lv_obj_t *label, uint32_t color)
{
  lv_obj_set_width(label, LV_PCT(88));
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
}

static void create_preview(void)
{
  lv_obj_t *screen = lv_screen_active();
  lv_obj_t *panel;
  lv_obj_t *label;

  lv_obj_set_style_bg_color(screen, lv_color_hex(0x160f2f), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

  label = lv_label_create(screen);
  configure_label(label, 0xffffff);
  lv_label_set_text(label, "LIVING CANVAS");
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 12);

  label = lv_label_create(screen);
  configure_label(label, 0xc8b9ff);
  lv_label_set_text(label, "SAFE OFFLINE PREVIEW");
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 36);

  panel = lv_obj_create(screen);
  lv_obj_remove_style_all(panel);
  lv_obj_set_size(panel, LV_PCT(90), LV_PCT(54));
  lv_obj_align(panel, LV_ALIGN_CENTER, 0, 14);
  lv_obj_set_style_bg_color(panel, lv_color_hex(0x2b2150), 0);
  lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
  lv_obj_set_style_border_color(panel, lv_color_hex(0x8c73ff), 0);
  lv_obj_set_style_border_width(panel, 2, 0);
  lv_obj_set_style_radius(panel, 16, 0);

  label = lv_label_create(panel);
  configure_label(label, 0xffc6df);
  lv_label_set_text(label, "( =^.^= )");
  lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 16);

  label = lv_label_create(panel);
  configure_label(label, 0xffffff);
  lv_label_set_text(label, "Dinner, gently decided.");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 5);

  label = lv_label_create(panel);
  configure_label(label, 0xc8b9ff);
  lv_label_set_text(label, "Local safety rules are ready.\nAgent bridge is built in.");
  lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -14);

  label = lv_label_create(screen);
  configure_label(label, 0x8ee6c4);
  lv_label_set_text(label, "DISPLAY OK  |  NO DEVICE ACTIONS");
  lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -12);
}

int lc_display_run_preview(void)
{
  lv_nuttx_dsc_t info;
  lv_nuttx_result_t result;

  if (lv_is_initialized())
    {
      fprintf(stderr, "Living Canvas display is already in use\n");
      return 2;
    }

#  ifdef NEED_BOARDINIT
  boardctl(BOARDIOC_INIT, 0);
#  endif

  lv_init();
  lv_nuttx_dsc_init(&info);

#  ifdef CONFIG_LV_USE_NUTTX_LCD
  info.fb_path = "/dev/lcd0";
#  endif

  lv_nuttx_init(&info, &result);
  if (result.disp == NULL)
    {
      fprintf(stderr, "Living Canvas could not open /dev/lcd0\n");
      lv_nuttx_deinit(&result);
      lv_deinit();
      return 2;
    }

  create_preview();

  for (;;)
    {
      uint32_t idle = lv_timer_handler();

      if (idle == 0u)
        {
          idle = 1u;
        }
      else if (idle > 50u)
        {
          idle = 50u;
        }

      usleep(idle * 1000u);
    }
}

#else

int lc_display_run_preview(void)
{
  return 2;
}

#endif
