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
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL.h>
#include <time.h>
#include "config.h"

#include "defstr.h"

#include "surface.h"
#include "render32.h"
#include "physics.h"
#include "input.h"

#include "trans.h"
#include "readfile.h"
#include "camera.h"

int main(int argc,char *argv[])
{char numefis[MAXWLG];

int i,
    t0frame; /*t0frame - moment when image starts to be displayed*/

struct _surface* pSurface = NULL;

sgob** objs; /*objects*/
struct _matrix camera;
int nob; /*number of objects and of object types*/

vhc car; /*vehicle*/

float action_accelerate = 0;
float action_brake = 0;
float action_left = 0;
float action_right = 0;
float action_reverse = 0;
float action_quit = 0;

REALN tframe=0,xan=0,/*tframe-time necessary for display; xan-number of displayed images*/
      timp; /*total time*/

/*for game*/
REALN realstep, /*real time step (s)*/
      sim_speed;
int nstepsf; /*number of simulation steps/frame*/
/*for game^*/


camera.vx[0]=0; camera.vy[0]=0; camera.vz[0]=0;
camera.vx[1]=1; camera.vy[1]=0; camera.vz[1]=0;
camera.vx[2]=0; camera.vy[2]=1; camera.vz[2]=0;
camera.vx[3]=0; camera.vy[3]=0; camera.vz[3]=1; /*set camera parameters*/

car.af = 0;
car.bf = 0;
car.vrx = 0;

if(argc<=2){printf("Error: Input files not specified\r\nExample: ./skunks cars/car1 tracks/track1\r\n");exit(1);}
if(argc>=4){printf("Error: Too many arguments\r\n");exit(1);}


physics_init();
physics_setERP(0.2);
physics_setCFM(1e-5);
physics_setGravity(GRAVITY);

strcpy(numefis,argv[2]);
objs=readtrack(numefis,&nob); /*read objects from file*/

strcpy(numefis,argv[1]);
objs=readvehicle(numefis,objs,&nob,&car); /*read vehicle from file*/

printf("\r\n");


/*Initialize SDL*/
if(SDL_Init(SDL_INIT_VIDEO)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}
// Initialize display
pSurface = surface_create(SCREENWIDTH,SCREENHEIGHT);

set_view_angle(FOV);
set_double_pixel(DOUBLEPIX);
#if ASPCOR==1
set_width_factor(WIDTHFACTOR);
#endif

car.turn=0;
car.dmode=1;

input_initialize();
input_register(SDLK_UP, &action_accelerate);
input_register(SDLK_DOWN, &action_brake);
input_register(SDLK_LEFT, &action_left);
input_register(SDLK_RIGHT, &action_right);
input_register(SDLK_r, &action_reverse);
input_register(SDLK_ESCAPE, &action_quit);

timp=0; /*pornit cronometru*/
tframe=0.5; /*assuming 2 frames/second*/


while(action_quit == 0){

/*t0frame=clock();*/
t0frame=SDL_GetTicks();
xan++;

car.vrx = ((float)car.turn)*0.36;

/*simulation*/
nstepsf=(int)(tframe/STIMESTEP)+1; /*number of simulation steps/frame*/
realstep=tframe/nstepsf; /*simulation time step*/

sim_speed=0.1/realstep; /*decrease simulation speed if < 10fps*/
if(nstepsf>(int)sim_speed){nstepsf=(int)sim_speed;}

for(i=1;i<=nstepsf;i++){
  runsim(realstep);
  timp+=realstep;
}


setcamg(&camera,objs[car.oid[1]]);

odis(pSurface,&camera); /*display image*/

input_process();

car.af=action_accelerate*car.accel*(float)car.dmode;
car.bf=(action_brake+0.01f)*car.brake;
car.turn=action_right-action_left;
if(action_reverse>0.0f)
{
  car.dmode=-car.dmode;
  car.af=0;
}

/*tframe=(REALN)(clock()-t0frame)/CLOCKS_PER_SEC;*/
tframe=(REALN)(SDL_GetTicks()-t0frame)/1000;
}


printf("\r\n\r\n\r\n**********************************************\r\n");
printf("\r\nTime: %1.2f seconds\r\n",timp);
printf("Average framerate: %1.2f f/s\r\n\r\n",xan/timp);
printf("**********************************************\r\n\r\n");

SDL_Quit();

renderer_release();
for(i=1;i<=nob;i++)
{
  free(objs[i]);
}
free(objs);

/* printf("Press ENTER: ");getchar();printf("\r\n"); */

physics_release();

odis(0,0); /*freed static variables from odis() in "camera.h"*/

return 0;}
