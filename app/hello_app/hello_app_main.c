/****************************************************************************
 * Living Canvas - openvela Contest 2026 team 482
 ****************************************************************************/

#include "lc_state.h"
#include "lc_voice.h"

#include <stdio.h>

int main(int argc, char *argv[])
{
  lc_state_machine_t machine;
  lc_voice_session_t voice;

  (void)argc;
  (void)argv;

  lc_state_init(&machine, 30000u);
  if (!lc_voice_init(&voice, 15000u))
    {
      fprintf(stderr, "Living Canvas voice safety init failed\n");
      return 1;
    }

  printf("Living Canvas core ready (state=%d, voice_active=%d)\n",
  return 0;
}
