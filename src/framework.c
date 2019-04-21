/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <SDL.h>

#include "surface.h"
#include "render32.h"
#include "physics.h"
#include "input.h"
#include "skunks2.h"
#include "vehicle.h"

int main(int argc,char *argv[])
{
  int t0frame; // t0frame - moment when image starts to be displayed

  float action_quit = 0;

  float tframe = 0;  // tframe-time necessary for display

  physics_init();

  // Initialize SDL
  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    SDL_Quit();
    return 0;
  }

  renderer_initialize();

  input_initialize();
  input_register(SDLK_ESCAPE, &action_quit);

  skunks_initialize();

  tframe=0.5; // assuming 2 frames/second

  while(action_quit == 0)
  {
    t0frame=SDL_GetTicks();

    input_process();

    vehicle_process();

    physics_process(tframe);

    skunks_process();

    renderer_process();

    tframe=(float)(SDL_GetTicks()-t0frame)/1000;
  }

  skunks_release();

  SDL_Quit();

  renderer_release();

  physics_release();

  odis(0,0); // freed static variables from odis() in "camera.h"

  return 0;
}
