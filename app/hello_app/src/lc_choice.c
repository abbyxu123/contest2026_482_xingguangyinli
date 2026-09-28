#include "lc_choice.h"

#include <stddef.h>

void lc_choice_init(lc_choice_state_t *state)
{
  if (state != NULL)
    {
      state->selected = LC_CHOICE_TAKEOUT;
      state->armed = false;
      state->confirmed = false;
    }
}

lc_choice_tap_result_t lc_choice_tap(lc_choice_state_t *state,
                                    lc_choice_id_t choice)
{
  if (state == NULL || choice < LC_CHOICE_TAKEOUT ||
      choice >= LC_CHOICE_COUNT)
    {
      return LC_CHOICE_TAP_INVALID;
    }

  if (!state->armed || state->selected != choice)
    {
      state->selected = choice;
      state->armed = true;
      state->confirmed = false;
      return LC_CHOICE_TAP_SELECTED;
    }

  state->confirmed = true;
  return LC_CHOICE_TAP_CONFIRMED;
}

void lc_choice_next(lc_choice_state_t *state)
{
  if (state != NULL)
    {
      state->selected = (lc_choice_id_t)((state->selected + 1) %
                                         LC_CHOICE_COUNT);
      state->armed = true;
      state->confirmed = false;
    }
}

void lc_choice_previous(lc_choice_state_t *state)
{
  if (state != NULL)
    {
      state->selected = state->selected == LC_CHOICE_TAKEOUT
                          ? LC_CHOICE_HOME
                          : (lc_choice_id_t)(state->selected - 1);
      state->armed = true;
      state->confirmed = false;
    }
}

lc_choice_id_t lc_choice_confirm(lc_choice_state_t *state)
{
  if (state == NULL)
    {
      return LC_CHOICE_TAKEOUT;
    }

  state->armed = true;
  state->confirmed = true;
  return state->selected;
}
