#include <SDL.h>

#include "input.h"

float* actions[(SDLK_SLEEP&0xFF)+256];

void input_initialize()
{
  memset(actions,0,sizeof(actions));
}

int input_map_to_code(char* keyname)
{
  int code = -1;

  if( (code==-1) && (strcmp(keyname,"Key_Arrow_Up")==0) )   code = SDLK_UP;
  if( (code==-1) && (strcmp(keyname,"Key_Arrow_Down")==0) ) code = SDLK_DOWN;
  if( (code==-1) && (strcmp(keyname,"Key_Arrow_Left")==0) ) code = SDLK_LEFT;
  if( (code==-1) && (strcmp(keyname,"Key_Arrow_Right")==0) ) code = SDLK_RIGHT;

  if( (code==-1) && (strcmp(keyname,"Key_Enter")==0) )  code = SDLK_RETURN;
  if( (code==-1) && (strcmp(keyname,"Key_Sleep")==0) )  code = SDLK_SLEEP;
  if( (code==-1) && (strcmp(keyname,"Key_Backspace")==0) ) code = SDLK_BACKSPACE;
  if( (code==-1) && (strcmp(keyname,"Key_Delete")==0) ) code = SDLK_DELETE;
  if( (code==-1) && (strcmp(keyname,"Key_Escape")==0) ) code = SDLK_ESCAPE;
  if( (code==-1) && (strcmp(keyname,"Key_Space")==0) )  code = SDLK_SPACE;

  if( (code==-1) && (strcmp(keyname,"Key_A")==0) ) code = SDLK_a;
  if( (code==-1) && (strcmp(keyname,"Key_B")==0) ) code = SDLK_b;
  if( (code==-1) && (strcmp(keyname,"Key_C")==0) ) code = SDLK_c;
  if( (code==-1) && (strcmp(keyname,"Key_D")==0) ) code = SDLK_d;
  if( (code==-1) && (strcmp(keyname,"Key_E")==0) ) code = SDLK_e;
  if( (code==-1) && (strcmp(keyname,"Key_F")==0) ) code = SDLK_f;
  if( (code==-1) && (strcmp(keyname,"Key_G")==0) ) code = SDLK_g;
  if( (code==-1) && (strcmp(keyname,"Key_H")==0) ) code = SDLK_h;
  if( (code==-1) && (strcmp(keyname,"Key_I")==0) ) code = SDLK_i;
  if( (code==-1) && (strcmp(keyname,"Key_J")==0) ) code = SDLK_j;
  if( (code==-1) && (strcmp(keyname,"Key_K")==0) ) code = SDLK_k;
  if( (code==-1) && (strcmp(keyname,"Key_L")==0) ) code = SDLK_l;
  if( (code==-1) && (strcmp(keyname,"Key_M")==0) ) code = SDLK_m;
  if( (code==-1) && (strcmp(keyname,"Key_N")==0) ) code = SDLK_n;
  if( (code==-1) && (strcmp(keyname,"Key_O")==0) ) code = SDLK_o;
  if( (code==-1) && (strcmp(keyname,"Key_P")==0) ) code = SDLK_p;
  if( (code==-1) && (strcmp(keyname,"Key_Q")==0) ) code = SDLK_q;
  if( (code==-1) && (strcmp(keyname,"Key_R")==0) ) code = SDLK_r;
  if( (code==-1) && (strcmp(keyname,"Key_S")==0) ) code = SDLK_s;
  if( (code==-1) && (strcmp(keyname,"Key_T")==0) ) code = SDLK_t;
  if( (code==-1) && (strcmp(keyname,"Key_U")==0) ) code = SDLK_u;
  if( (code==-1) && (strcmp(keyname,"Key_V")==0) ) code = SDLK_v;
  if( (code==-1) && (strcmp(keyname,"Key_W")==0) ) code = SDLK_w;
  if( (code==-1) && (strcmp(keyname,"Key_X")==0) ) code = SDLK_x;
  if( (code==-1) && (strcmp(keyname,"Key_Y")==0) ) code = SDLK_y;
  if( (code==-1) && (strcmp(keyname,"Key_Z")==0) ) code = SDLK_z;

  return code;
}

void input_register(char* keyname, float* action)
{
  int code = input_map_to_code(keyname);

  if( (code >= SDLK_BACKSPACE) && (code < SDLK_SLEEP) )
  {
    if(code > SDLK_DELETE)
    {
      code = (code&0xFF)+256;
    }
    actions[code] = action;
  }
}

void input_deregister(char* keyname)
{
  int code = input_map_to_code(keyname);

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
