#include "lc_display.h"
#include "lc_choice.h"
#include "generated/lc_choice_assets.h"

#ifdef __NuttX__

#  include <nuttx/config.h>

#  include <stdbool.h>
#  include <stdint.h>
#  include <stdio.h>
#  include <string.h>
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

static lv_obj_t *g_choice_cards[LC_CHOICE_COUNT];
static bool g_pulse_bright;

static void init_image_descriptor(lv_image_dsc_t *descriptor,
                                  const uint8_t *data,
                                  uint32_t data_size,
                                  uint32_t width,
                                  uint32_t height)
{
  memset(descriptor, 0, sizeof(*descriptor));
  descriptor->header.magic = LV_IMAGE_HEADER_MAGIC;
  descriptor->header.cf = LV_COLOR_FORMAT_RGB565;
  descriptor->header.w = width;
  descriptor->header.h = height;
  descriptor->header.stride = width * 2u;
  descriptor->data_size = data_size;
  descriptor->data = data;
}

static bool create_background(lv_display_t *display, lv_obj_t *screen)
{
  static lv_image_dsc_t background;
  int32_t width = lv_display_get_horizontal_resolution(display);
  int32_t height = lv_display_get_vertical_resolution(display);
  lv_obj_t *image;

  if (width == (int32_t)LC_BG_PORTRAIT_WIDTH &&
      height == (int32_t)LC_BG_PORTRAIT_HEIGHT)
    {
      init_image_descriptor(&background, lc_bg_portrait,
                            LC_BG_PORTRAIT_BYTES,
                            LC_BG_PORTRAIT_WIDTH,
                            LC_BG_PORTRAIT_HEIGHT);
    }
  else if (width == (int32_t)LC_BG_LANDSCAPE_WIDTH &&
           height == (int32_t)LC_BG_LANDSCAPE_HEIGHT)
    {
      init_image_descriptor(&background, lc_bg_landscape,
                            LC_BG_LANDSCAPE_BYTES,
                            LC_BG_LANDSCAPE_WIDTH,
                            LC_BG_LANDSCAPE_HEIGHT);
    }
  else
    {
      fprintf(stderr, "Living Canvas unsupported display: %ldx%ld\n",
              (long)width, (long)height);
      return false;
    }

  lv_obj_remove_style_all(screen);
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x1b1024), 0);
  lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);

  image = lv_image_create(screen);
  lv_image_set_src(image, &background);
  lv_obj_center(image);
  return true;
}

static void style_choice_card(lv_obj_t *card, bool selected)
{
  lv_obj_set_style_border_width(card, selected ? 4 : 2, 0);
  lv_obj_set_style_border_color(
    card, lv_color_hex(selected ? 0xffd66b : 0x7d604e), 0);
  lv_obj_set_style_border_opa(card,
                              selected ? LV_OPA_COVER : LV_OPA_70, 0);
}

static lv_obj_t *create_choice_card(lv_obj_t *screen,
                                    const lv_image_dsc_t *source,
                                    int32_t x_offset,
                                    bool selected)
{
  lv_obj_t *card = lv_obj_create(screen);
  lv_obj_t *image;

  lv_obj_remove_style_all(card);
  lv_obj_set_size(card, 70, 70);
  lv_obj_align(card, LV_ALIGN_BOTTOM_MID, x_offset, -5);
  lv_obj_set_style_bg_color(card, lv_color_hex(0xf6e1c2), 0);
  lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
  lv_obj_set_style_radius(card, 10, 0);
  style_choice_card(card, selected);

  image = lv_image_create(card);
  lv_image_set_src(image, source);
  lv_obj_center(image);
  return card;
}

static void pulse_selected_card(lv_timer_t *timer)
{
  (void)timer;
  g_pulse_bright = !g_pulse_bright;
  lv_obj_set_style_border_opa(g_choice_cards[LC_CHOICE_TAKEOUT],
                              g_pulse_bright ? LV_OPA_COVER : LV_OPA_70,
                              0);
}

static void create_choice_overlay(lv_obj_t *screen)
{
  static lv_image_dsc_t card_images[LC_CHOICE_COUNT];
  lc_choice_state_t choice;

  lc_choice_init(&choice);
  init_image_descriptor(&card_images[LC_CHOICE_TAKEOUT], lc_card_takeout,
                        LC_CARD_BYTES, LC_CARD_WIDTH, LC_CARD_HEIGHT);
  init_image_descriptor(&card_images[LC_CHOICE_MYSTERY], lc_card_mystery,
                        LC_CARD_BYTES, LC_CARD_WIDTH, LC_CARD_HEIGHT);
  init_image_descriptor(&card_images[LC_CHOICE_HOME], lc_card_home,
                        LC_CARD_BYTES, LC_CARD_WIDTH, LC_CARD_HEIGHT);

  g_choice_cards[LC_CHOICE_TAKEOUT] =
    create_choice_card(screen, &card_images[LC_CHOICE_TAKEOUT], -76,
                       choice.selected == LC_CHOICE_TAKEOUT);
  g_choice_cards[LC_CHOICE_MYSTERY] =
    create_choice_card(screen, &card_images[LC_CHOICE_MYSTERY], 0,
                       choice.selected == LC_CHOICE_MYSTERY);
  g_choice_cards[LC_CHOICE_HOME] =
    create_choice_card(screen, &card_images[LC_CHOICE_HOME], 76,
                       choice.selected == LC_CHOICE_HOME);
  g_pulse_bright = true;
  (void)lv_timer_create(pulse_selected_card, 450u, NULL);
}

static int run_artwork_preview(bool with_choices)
{
  lv_nuttx_dsc_t info;
  lv_nuttx_result_t result;
  lv_obj_t *screen;

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

  screen = lv_screen_active();
  if (!create_background(result.disp, screen))
    {
      lv_nuttx_deinit(&result);
      lv_deinit();
      return 2;
    }

  if (with_choices)
    {
      create_choice_overlay(screen);
    }

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

int lc_display_run_image_preview(void)
{
  return run_artwork_preview(false);
}

int lc_display_run_choice_preview(void)
{
  return run_artwork_preview(true);
}

#else

int lc_display_run_preview(void)
{
  return 2;
}

int lc_display_run_image_preview(void)
{
  return 2;
}

int lc_display_run_choice_preview(void)
{
  return 2;
}

#endif
