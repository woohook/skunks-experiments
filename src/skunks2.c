/*
Copyright (C) 2007-2013 Victor Matei Petrescu

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

#include <stdio.h>
#include <SDL.h>
#include "config.h"

#include "defstr.h"

#include "surface.h"
#include "render32.h"
#include "physics.h"
#include "input.h"

#include "trans.h"
#include "list.h"
#include "vehicle.h"
#include "readfile.h"
#include "camera.h"


#define SCREENWIDTH 800   // screen width (pixels)
#define SCREENHEIGHT 600  // screen height (pixels)


int main(int argc,char *argv[])
{

int t0frame; // t0frame - moment when image starts to be displayed

struct _surface* pSurface = NULL;

struct _list* objs = 0;
struct _list* parts = 0;
struct _matrix camera;

vhc* car = vehicle_create();

float action_quit = 0;

REALN tframe=0;  // tframe-time necessary for display

char* vehicleName = 0;
char* worldName = 0;
int width  = SCREENWIDTH;
int height = SCREENHEIGHT;

matrix_identity(&camera);

if(argc<2)
{
  printf("Error: parameters required.\r\nExample: %s vehicle=cars/car1 world=tracks/track1\r\n", argv[0]);
  exit(1);
}

physics_init();

for(int i=1; i<argc; i++)
{
  if(strncmp("vehicle=",argv[i],8)==0)
  {
    vehicleName = &argv[i][8];
  }
  if(strncmp("world=",argv[i],6)==0)
  {
    worldName = &argv[i][6];
  }
  if(strncmp("resolution=",argv[i],11)==0)
  {
    sscanf(argv[i],"resolution=%dx%d",&width,&height);
  }
}

if(vehicleName==0)
{
  printf("Error: No vehicle specified.\r\nExample: %s vehicle=cars/car1 world=tracks/track1\r\n", argv[0]);
  exit(1);
}

if(worldName==0)
{
  printf("Error: No world specified.\r\nExample: %s vehicle=cars/car1 world=tracks/track1\r\n", argv[0]);
  exit(1);
}

objs = readtrack(worldName);

parts = readvehicle(vehicleName,car);

printf("\r\n");


/*Initialize SDL*/
if(SDL_Init(SDL_INIT_VIDEO)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}
// Initialize display
pSurface = surface_create(width,height);

renderer_initialize();

input_initialize();
input_register(SDLK_UP, &car->action_accelerate);
input_register(SDLK_DOWN, &car->action_brake);
input_register(SDLK_LEFT, &car->action_left);
input_register(SDLK_RIGHT, &car->action_right);
input_register(SDLK_r, &car->action_reverse);
input_register(SDLK_ESCAPE, &action_quit);

tframe=0.5; /*assuming 2 frames/second*/


while(action_quit == 0){

/*t0frame=clock();*/
t0frame=SDL_GetTicks();

input_process();

vehicle_process(car);

physics_process(tframe);

setcamg(&camera,list_get_value(parts,0));

odis(pSurface,&camera); /*display image*/

/*tframe=(REALN)(clock()-t0frame)/CLOCKS_PER_SEC;*/
tframe=(REALN)(SDL_GetTicks()-t0frame)/1000;
}

SDL_Quit();

renderer_release();

list_release(objs,1);
list_release(parts,1);

/* printf("Press ENTER: ");getchar();printf("\r\n"); */

physics_release();

odis(0,0); /*freed static variables from odis() in "camera.h"*/

return 0;}
