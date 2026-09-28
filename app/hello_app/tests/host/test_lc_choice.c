#include "lc_choice.h"

#include <assert.h>
#include <stdio.h>

static void test_default_is_takeout(void)
{
  lc_choice_state_t state;

  lc_choice_init(&state);
  assert(state.selected == LC_CHOICE_TAKEOUT);
  assert(!state.armed);
  assert(!state.confirmed);
}

static void test_first_tap_arms_choice(void)
{
  lc_choice_state_t state;

  lc_choice_init(&state);
  assert(lc_choice_tap(&state, LC_CHOICE_MYSTERY) ==
         LC_CHOICE_TAP_SELECTED);
  assert(state.selected == LC_CHOICE_MYSTERY);
  assert(state.armed);
  assert(!state.confirmed);
}

static void test_second_tap_confirms_choice(void)
{
  lc_choice_state_t state;

  lc_choice_init(&state);
  assert(lc_choice_tap(&state, LC_CHOICE_HOME) == LC_CHOICE_TAP_SELECTED);
  assert(lc_choice_tap(&state, LC_CHOICE_HOME) == LC_CHOICE_TAP_CONFIRMED);
  assert(state.selected == LC_CHOICE_HOME);
  assert(state.armed);
  assert(state.confirmed);

  assert(lc_choice_tap(&state, LC_CHOICE_HOME) == LC_CHOICE_TAP_CONFIRMED);
  assert(state.confirmed);
}

static void test_switching_choice_clears_confirmation(void)
{
  lc_choice_state_t state;

  lc_choice_init(&state);
  (void)lc_choice_tap(&state, LC_CHOICE_TAKEOUT);
  (void)lc_choice_tap(&state, LC_CHOICE_TAKEOUT);
  assert(state.confirmed);

  assert(lc_choice_tap(&state, LC_CHOICE_MYSTERY) ==
         LC_CHOICE_TAP_SELECTED);
  assert(state.selected == LC_CHOICE_MYSTERY);
  assert(state.armed);
  assert(!state.confirmed);
}

static void test_invalid_tap_does_not_mutate_state(void)
{
  lc_choice_state_t state;
  lc_choice_state_t before;

  lc_choice_init(&state);
  before = state;
  assert(lc_choice_tap(&state, LC_CHOICE_COUNT) == LC_CHOICE_TAP_INVALID);
  assert(state.selected == before.selected);
  assert(state.armed == before.armed);
  assert(state.confirmed == before.confirmed);
  assert(lc_choice_tap(NULL, LC_CHOICE_TAKEOUT) == LC_CHOICE_TAP_INVALID);
}

static void test_navigation_wraps(void)
{
  lc_choice_state_t state;

  lc_choice_init(&state);
  lc_choice_next(&state);
  assert(state.selected == LC_CHOICE_MYSTERY);
  assert(state.armed);
  assert(!state.confirmed);
  lc_choice_next(&state);
  assert(state.selected == LC_CHOICE_HOME);
  lc_choice_next(&state);
  assert(state.selected == LC_CHOICE_TAKEOUT);

  lc_choice_previous(&state);
  assert(state.selected == LC_CHOICE_HOME);
  lc_choice_previous(&state);
  assert(state.selected == LC_CHOICE_MYSTERY);
}

static void test_confirm_preserves_selection(void)
{
  lc_choice_state_t state;

  lc_choice_init(&state);
  lc_choice_next(&state);
  assert(lc_choice_confirm(&state) == LC_CHOICE_MYSTERY);
  assert(state.armed);
  assert(state.confirmed);
  assert(state.selected == LC_CHOICE_MYSTERY);
}

int main(void)
{
  test_default_is_takeout();
  test_first_tap_arms_choice();
  test_second_tap_confirms_choice();
  test_switching_choice_clears_confirmation();
  test_invalid_tap_does_not_mutate_state();
  test_navigation_wraps();
  test_confirm_preserves_selection();
  puts("PASS: lc_choice");
  return 0;
}
