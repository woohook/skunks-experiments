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

#include "trans.h"
#include "readfile.h"
#include "sound.h"
#include "camera.h"

int main(int argc,char *argv[])
{char numefis[MAXWLG];

int i,quit=0,
    t0frame; /*t0frame - moment when image starts to be displayed*/

SDL_Event event;
struct _surface* pSurface = NULL;


//pixcol backcol; /*culoarea fundalului*/
int background_red, background_green, background_blue;
REALN  zfog,zmax; /*zfog,zmax - distanta de la care incepe ceatza, respectiv de la care nu se mai vede nimic*/

sgob** objs; /*objects*/
struct _matrix camera;
int nob,camflag=2; /*number of objects and of object types*/

vhc car; /*vehicle*/

REALN tframe=0,xan=0,/*tframe-time necessary for display; xan-number of displayed images*/
      timp,dstr; /*total time, distance traveled*/

/*for game*/
REALN vrxmax,vrxmr, /*rot. speed*/
      arx,arxmax,arxmr, /*rot. acceleration*/
      vrot3, /*rot. speed of level 3 objects*/
      vrotc,vrcmax,rotc, /*rot. speed and rotation of camera*/
      realstep, /*real time step (s)*/
      speed,dspeed,rotspeed,acc;
int turn, /*-1: left; 0: no turn; 1: right*/
    dmode, /*1 forward, -1 reverse*/
    nstepsf; /*number of simulation steps/frame*/
/*for game^*/


zfog=80;
zmax=120; /*visibility (m)*/

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
objs=readtrack(numefis,&nob,&background_red,&background_green,&background_blue); /*read objects from file*/
set_background_color(background_red,background_green,background_blue);

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

sound_initialize();

arx=0;
vrxmr=vrxmax=0.36;
arxmr=arxmax=vrxmax/1.5;
turn=0;
dmode=1;
vrot3=0.5;
vrcmax=0.79;
vrotc=0;
rotc=0;

timp=0,dstr=0; /*pornit cronometru*/
tframe=0.5; /*assuming 2 frames/second*/


while(!quit){

/*t0frame=clock();*/
t0frame=SDL_GetTicks();
xan++;

if(speed<10){vrxmr=vrxmax; arxmr=arxmax;}
else{
  vrxmr=vrxmax/(0.1*speed);
  arxmr=arxmax/(0.1*speed);
}

switch(turn){
  case 0: if(car.vrx>0){arx=-arxmr*1.5;}else{if(car.vrx<0){arx=arxmr*1.5;}else{arx=0;}}
          if(fabs(car.vrx)<2.25*tframe*arx){arx=0; car.vrx=0;}
          break;
  case -1: if(car.vrx>-vrxmr){arx=-arxmr; if(car.vrx>0){arx*=1.5;}}else{arx=0;}
           break;
  case 1: if(car.vrx<vrxmr){arx=arxmr; if(car.vrx<0){arx*=1.5;}}else{arx=0;}
          break;
  default: break;
}

car.vrx+=arx*tframe;
if(car.vrx>vrxmr){car.vrx=vrxmr;}
if(car.vrx<-vrxmr){car.vrx=-vrxmr;}


/*simulation*/
nstepsf=(int)(tframe/STIMESTEP)+1; /*number of simulation steps/frame*/
realstep=tframe/nstepsf; /*simulation time step*/

speed=0.1/realstep; /*decrease simulation speed if < 10fps*/
if(nstepsf>(int)speed){nstepsf=(int)speed;}

for(i=1;i<=nstepsf;i++){
  runsim(realstep);
  timp+=realstep;
}


for(i=1;i<=nob;i++){
  if(objs[i]->lev==3){
    rotab(&objs[i]->transform,objs[i]->transform.vx[0],objs[i]->transform.vy[0],objs[i]->transform.vz[0],objs[i]->transform.vx[3],objs[i]->transform.vy[3],objs[i]->transform.vz[3],vrot3*tframe);
  }
}

physics_getLinearBodyVelocity(car.parts[1],&speed,&dspeed);
{
  int motor_wheel_count=0;
  for(i=1;i<=car.nob;i++)
  {
    if((car.ofc[i]==3)||(car.ofc[i]==5))
    {
      motor_wheel_count++;
      physics_getAngularBodyVelocity(car.parts[i],&rotspeed);
    }
  }
  rotspeed/=motor_wheel_count; // average rot. speed of motor wheels
}
acc=dspeed/tframe;

sound_update(rotspeed, acc);

setcamg(&camera,&car,camflag,objs[car.oid[1]]);

rotc+=vrotc*tframe; if(camflag==2){rotc=0; vrotc=0;}
if(rotc){rotatx(&camera,objs[car.oid[1]]->transform.vy[0],objs[car.oid[1]]->transform.vz[0],rotc);}

odis(pSurface,zfog,zmax,&camera); /*display image*/

dstr+=(speed*tframe);


while(SDL_PollEvent(&event)){
switch(event.type){

case SDL_KEYDOWN:
  switch(event.key.keysym.sym){
    case SDLK_q:
    case SDLK_UP:
    case SDLK_t: car.af=car.accel*(float)dmode;
                 break;
    case SDLK_a:
    case SDLK_DOWN:
    case SDLK_f: car.bf=1.01f*car.brake;
                 break;
    case SDLK_o:
    case SDLK_LEFT:
    case SDLK_y: turn=-1;
                 break;
    case SDLK_p:
    case SDLK_RIGHT:
    case SDLK_u: turn=1;
                 break;

    case SDLK_r: dmode=-dmode;
                 break;

    case SDLK_c: camflag++; if(camflag>3){camflag=1;}
                 rotc=0; vrotc=0;
                 break;

    case SDLK_n: vrotc=-vrcmax;
                   break;

    case SDLK_m: vrotc=vrcmax;
                   break;

    case SDLK_ESCAPE: quit=1;

    default: break;
  } break;
case SDL_KEYUP:
  switch(event.key.keysym.sym){
    case SDLK_q:
    case SDLK_UP:
    case SDLK_t: car.af=0;
                 break;
    case SDLK_a:
    case SDLK_DOWN:
    case SDLK_f: car.bf=0.01f*car.brake;
                 break;
    case SDLK_o:
    case SDLK_LEFT:
    case SDLK_y: if(turn==-1){turn=0;}
                 break;
    case SDLK_p:
    case SDLK_RIGHT:
    case SDLK_u: if(turn==1){turn=0;}
                 break;

    case SDLK_r: car.af=0;
                 break;

    case SDLK_n: vrotc=0;
                   break;

    case SDLK_m: vrotc=0;
                   break;

    default: break;
  } break;

case SDL_QUIT: quit=1;

default: break;
}
}
/*tframe=(REALN)(clock()-t0frame)/CLOCKS_PER_SEC;*/
tframe=(REALN)(SDL_GetTicks()-t0frame)/1000;
}


printf("\r\n\r\n\r\n**********************************************\r\n");
printf("\r\nDistance: %1.2f km\r\nTime: %1.2f seconds\r\n",dstr/1000,timp);
printf("Average speed: %1.2f km/h\r\n",3.6*dstr/timp);
printf("Average framerate: %1.2f f/s\r\n\r\n",xan/timp);
printf("**********************************************\r\n\r\n");

sound_terminate();

SDL_Quit();

sound_release();

renderer_release();
for(i=1;i<=nob;i++)
{
  free(objs[i]);
}
free(objs);

/* printf("Press ENTER: ");getchar();printf("\r\n"); */

physics_release();

odis(0,0,0,0); /*freed static variables from odis() in "camera.h"*/

return 0;}
