/****************************************************************************
 * Living Canvas - openvela Contest 2026 team 482
 ****************************************************************************/

#include "lc_state.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
  lc_state_machine_t machine;

  (void)argc;
  (void)argv;

  lc_state_init(&machine, 30000u);
  printf("Living Canvas core ready (state=%d)\n", (int)machine.state);
  return 0;
}
