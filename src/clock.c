#include <SDL.h>
#include "clock.h"

int last_timestamp = 0;
int time_since_last_frame = 0;

void clock_initialize()
{
  last_timestamp = SDL_GetTicks();
}

void clock_process()
{
  int timestamp = SDL_GetTicks();
  time_since_last_frame = timestamp - last_timestamp;
  last_timestamp = timestamp;
}

void clock_release()
{
}

int clock_get_frametime()
{
  return time_since_last_frame;
}
