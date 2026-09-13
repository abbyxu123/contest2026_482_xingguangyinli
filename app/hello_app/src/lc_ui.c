#include "lc_ui.h"

#include <stdio.h>
#include <string.h>

static const char *state_label(lc_state_t state)
{
  switch (state)
    {
      case LC_STATE_IDLE:
        return "IDLE";
      case LC_STATE_GREETING:
        return "GREETING";
      case LC_STATE_LISTENING:
        return "LISTENING";
      case LC_STATE_THINKING:
        return "THINKING";
      case LC_STATE_RECOMMENDATION:
        return "RECOMMENDATION";
      case LC_STATE_CONFIRMED:
        return "CONFIRMED";
      case LC_STATE_ERROR:
        return "ERROR";
      default:
        return NULL;
    }
}

lc_ui_status_t lc_ui_build_model(lc_state_t state,
                                 bool time_synced,
                                 unsigned int hour,
                                 unsigned int minute,
                                 lc_ui_model_t *model)
{
  const char *label;
  lc_time_status_t time_status;

  if (model == NULL || (label = state_label(state)) == NULL)
    {
      return LC_UI_INVALID;
    }

  memset(model, 0, sizeof(*model));
  snprintf(model->state_label, sizeof(model->state_label), "%s", label);
  model->cat_eyes_open = state != LC_STATE_IDLE;
  model->qr_visible = state == LC_STATE_RECOMMENDATION;

  time_status = lc_clock_compute(time_synced, hour, minute,
                                 &model->clock_angles);
  if (time_status == LC_TIME_UNSYNCED)
    {
      snprintf(model->time_label, sizeof(model->time_label), "--:--");
      model->clock_visible = false;
      return LC_UI_TIME_UNSYNCED;
    }

  if (time_status != LC_TIME_OK)
    {
      return LC_UI_INVALID;
    }

  snprintf(model->time_label, sizeof(model->time_label), "%02u:%02u",
           hour, minute);
  model->clock_visible = true;
  return LC_UI_OK;
}
