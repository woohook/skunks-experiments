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
{char numefis[MAXWLG];

int i,
    t0frame; /*t0frame - moment when image starts to be displayed*/

struct _surface* pSurface = NULL;

sgob** objs; /*objects*/
struct _list* parts = 0;
struct _matrix camera;
int nob; /*number of objects and of object types*/

vhc* car = vehicle_create();

float action_quit = 0;

REALN tframe=0;  // tframe-time necessary for display

matrix_identity(&camera);

if(argc<=2){printf("Error: Input files not specified\r\nExample: ./skunks cars/car1 tracks/track1\r\n");exit(1);}
if(argc>=4){printf("Error: Too many arguments\r\n");exit(1);}


physics_init();

strcpy(numefis,argv[2]);
objs=readtrack(numefis,&nob); /*read objects from file*/

strcpy(numefis,argv[1]);
parts = readvehicle(numefis,car); /*read vehicle from file*/

printf("\r\n");


/*Initialize SDL*/
if(SDL_Init(SDL_INIT_VIDEO)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}
// Initialize display
pSurface = surface_create(SCREENWIDTH,SCREENHEIGHT);

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
for(i=1;i<=nob;i++)
{
  free(objs[i]);
}
free(objs);
list_release(parts,1);

/* printf("Press ENTER: ");getchar();printf("\r\n"); */

physics_release();

odis(0,0); /*freed static variables from odis() in "camera.h"*/

return 0;}
