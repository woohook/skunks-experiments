#include <SDL.h>

#include "input.h"

float* actions[(SDLK_SLEEP&0xFF)+256];

void input_initialize()
{
  memset(actions,0,sizeof(actions));
}

void input_register(int code, float* action)
{
  if( (code >= SDLK_BACKSPACE) && (code < SDLK_SLEEP) )
  {
    if(code > SDLK_DELETE)
    {
      code = (code&0xFF)+256;
    }
    actions[code] = action;
  }
}

void input_deregister(int code)
{
  if( (code >= SDLK_BACKSPACE) && (code < SDLK_SLEEP) )
  {
    if(code > SDLK_DELETE)
    {
      code = (code&0xFF)+256;
    }
    actions[code] = 0;
  }
}

void input_process()
{
  SDL_Event event;

  while(SDL_PollEvent(&event))
  {
    SDL_Keycode key = event.key.keysym.sym;
    if(key > SDLK_DELETE)
    {
      key = (key&0xFF)+256;
    }

    switch(event.type)
    {
      case SDL_KEYDOWN:
        if(actions[key] != 0)
        {
          *actions[key] = 1.0f;
        }
        break;
      case SDL_KEYUP:
        if(actions[key] != 0)
        {
          *actions[key] = 0.0f;
        }
        break;
      default:
        break;
    }
  }
}

void input_release()
{
}
