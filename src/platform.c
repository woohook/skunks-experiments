#include <SDL.h>

#include "platform.h"

void platform_initialize()
{
  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    SDL_Quit();
    exit(-1);
  }
}

void platform_process()
{
}

void platform_release()
{
  SDL_Quit();
}
