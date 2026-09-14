#define main living_canvas_main
#include "../../hello_app_main.c"
#undef main

#include <assert.h>
#include <stddef.h>
#include <stdio.h>

int main(void)
{
  char *argv[] = {"living_canvas", NULL};

  assert(living_canvas_main(1, argv) == 0);
  puts("PASS: lc_main");
  return 0;
}
