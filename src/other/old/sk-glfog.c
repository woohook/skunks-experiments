/*
Copyright (C) 2007-2011 Victor Matei Petrescu

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

#define dSINGLE
/*for ODE*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <ode/ode.h>
#include <time.h>
#include "config.h"

#include "rendergl.h" /*just 1 file, 16 bit color*/

#include "defstr.h"
#include "trans.h"
#include "camera.h"
#include "readfile.h"
#include "game.h"

#define GLWIDTH 640
#define GLHEIGHT 480

int main(int argc,char *argv[])
{char numefis[MAXWLG];

int i,quit=0,
    t0frame; /*t0frame - moment when image starts to be displayed*/

SDL_Event event;
SDL_Surface *screen;


#if SOUND==1
/* Open the audio device */
SDL_AudioSpec *desired, *obtained;
SDL_AudioSpec *hardware_spec;
REALN volum[6]={0,0,0,0,0,0};
#endif

GLfloat density = 0.3; /*fog density, OpenGL*/
GLfloat fogcolor[4] = {0.0f,0.0f,0.0f,1.0f}; /*will be set to match background*/

pixcol backcol; /*culoarea fundalului*/

GLfloat  zfog,zmax; /*zfog,zmax - distanta de la care incepe ceatza, respectiv de la care nu se mai vede nimic*/
lightpr light;

sgob *objs,camera; /*objects*/
int nob,nto,camflag=2; /*number of objects and of object types*/

vhc car; /*vehicle*/

REALN tframe=0,xan=0,/*tframe-time necessary for display; xan-number of displayed images*/
      timp,dstr; /*total time, distance traveled*/

/*for game*/
REALN vrx,vrxmax,vrxmr, /*rot. speed*/
      arx,arxmax,arxmr, /*rot. acceleration*/
      vrot3, /*rot. speed of level 3 objects*/
      vrotc,vrcmax,rotc, /*rot. speed and rotation of camera*/
      realstep, /*real time step (s)*/
      speed,dspeed,rotspeed,acc,
      af=0,bf=0; /*acceleration and brake factors*/
int turn, /*-1: left; 0: no turn; 1: right*/
    dmode, /*1 forward, -1 reverse*/
    nstepsf; /*number of simulation steps/frame*/
FILE *repf;
/*for game^*/


zfog=80;
zmax=120; /*visibility (m)*/

camera.vx[0]=0; camera.vy[0]=0; camera.vz[0]=0;
camera.vx[1]=1; camera.vy[1]=0; camera.vz[1]=0;
camera.vx[2]=0; camera.vy[2]=1; camera.vz[2]=0;
camera.vx[3]=0; camera.vy[3]=0; camera.vz[3]=1; /*set camera parameters*/

if(argc<=2){printf("Error: Input files not specified\r\nExample: ./skunks cars/car1 tracks/track1\r\n");exit(1);}
if(argc>=4){printf("Error: Too many arguments\r\n");exit(1);}


#if REPLAY==1
if(!(repf=fopen("replays/rep1","w"))){printf("Error: could not open 'replays/rep1' (check the permissions)\r\n"); exit(1);}
fprintf(repf,"%s\r\n%s\r\n",argv[1],argv[2]);
#else
repf=NULL;
#endif


dInitODE();
wglob=dWorldCreate();
dWorldSetERP(wglob,0.2);
dWorldSetCFM(wglob,1e-5);
dWorldSetGravity(wglob,GRAVITY,0,0);

strcpy(numefis,argv[2]);
objs=readtrack(numefis,&nob,&nto,&backcol,&light); /*read objects from file*/

strcpy(numefis,argv[1]);
objs=readvehicle(numefis,objs,&nto,&nob,&car); /*read vehicle from file*/

printf("\r\n");

backcol.glred=(GLfloat)(backcol.red-50)*0.5/255;
backcol.glgreen=(GLfloat)(backcol.green-50)*0.5/255;
backcol.glblue=(GLfloat)(backcol.blue-50)*0.5/255; /*background color for OpenGL*/
fogcolor[0]=backcol.glred;
fogcolor[1]=backcol.glgreen;
fogcolor[2]=backcol.glblue;

#if SOUND==1
/* Allocate a desired SDL_AudioSpec */
desired = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
/* Allocate space for the obtained SDL_AudioSpec */
obtained = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
/* 22050Hz - FM Radio quality */
desired->freq=22050;
/* 16-bit signed audio */
desired->format=AUDIO_U8;
/* Mono */
desired->channels=1;
/* Large audio buffer reduces risk of dropouts but increases response time */
desired->samples=1024; /*increase if using computer faster than 400MHz*/
/* Our callback function */
desired->callback=my_audio_callback;
desired->userdata=volum;
#endif


/*Initialize SDL*/
if(SDL_Init(SDL_INIT_VIDEO)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}

   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,5 );
   SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16 );

/*Initialize display*/
screen=SDL_SetVideoMode(GLWIDTH,GLHEIGHT,CLBITS,SDL_OPENGL);
if(screen == NULL){printf("Couldn't set requested video mode: %s\n",SDL_GetError());SDL_Quit();return 0;}
printf("Set %dx%dx%d\n",(screen->pitch)/(screen->format->BytesPerPixel),GLHEIGHT,screen->format->BitsPerPixel);
/*Set window title*/
SDL_WM_SetCaption("Skunks-4.1.0 - OpenGL", "Skunks-4.1.0 - OpenGL");
/* Enable Unicode translation */
SDL_EnableUNICODE(1);
/*SDL initialized*/

   glViewport(0,0,GLWIDTH,GLHEIGHT);
   glClearColor(backcol.glred,backcol.glgreen,backcol.glblue,0.0f); /*background color*/
   glClearDepth(0.0);
   glDepthFunc(GL_GREATER);
   glEnable(GL_DEPTH_TEST);
   glShadeModel(GL_SMOOTH);
   glMatrixMode(GL_PROJECTION);

   glEnable (GL_FOG);
   glFogi (GL_FOG_MODE,GL_LINEAR);
   glFogfv (GL_FOG_COLOR,fogcolor);
   glFogf (GL_FOG_DENSITY,density);
   glFogf(GL_FOG_START,zfog);
   glFogf(GL_FOG_END,zmax);

   gluLookAt(
   0.0f, 0.0f, 0.0f,
   0.0f, 0.0f, 1.0f,
   0.0f, 1.0f, 0.0f);

#if ASPCOR==0
   gluPerspective(0.82f*FOV,(GLfloat)GLWIDTH/(GLfloat)GLHEIGHT,0.25f,zmax); /*0.25, zmax - clipping planes*/
#else
   gluPerspective(0.82f*FOV,(GLfloat)GLWIDTH/(GLfloat)GLHEIGHT/(GLfloat)WIDTHFACTOR,0.25f,zmax); /*0.25, zmax - clipping planes*/
#endif

   glPolygonOffset(10.0f,0.001f);
   glMatrixMode(GL_MODELVIEW);

#if SOUND==1
/* Open the audio device */
if ( SDL_OpenAudio(desired, obtained) < 0 ){
  fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
  exit(-1);
}
/* desired spec is no longer needed */
free(desired);
hardware_spec=obtained;

volum[2]=hardware_spec->format;
volum[3]=hardware_spec->channels;
volum[4]=hardware_spec->freq; /*pentru trimis la callback()*/

/* Start playing */
SDL_PauseAudio(0);
#endif


vrx=0; arx=0;
vrxmr=vrxmax=0.36;
arxmr=arxmax=vrxmax/1.5;
turn=0;
dmode=1;
vrot3=1.57;
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
  case 0: if(vrx>0){arx=-arxmr*1.5;}else{if(vrx<0){arx=arxmr*1.5;}else{arx=0;}}
          if(fabs(vrx)<2.25*tframe*arx){arx=0; vrx=0;}
          break;
  case -1: if(vrx>-vrxmr){arx=-arxmr; if(vrx>0){arx*=1.5;}}else{arx=0;}
           break;
  case 1: if(vrx<vrxmr){arx=arxmr; if(vrx<0){arx*=1.5;}}else{arx=0;}
          break;
  default: break;
}

vrx+=arx*tframe;
if(vrx>vrxmr){vrx=vrxmr;}
if(vrx<-vrxmr){vrx=-vrxmr;}


/*simulation*/
nstepsf=(int)(tframe/STIMESTEP)+1; /*number of simulation steps/frame*/
realstep=tframe/nstepsf; /*simulation time step*/

for(i=1;i<=nstepsf;i++){
  runsim(objs,nob,&car,realstep,vrx,af,bf,repf,&timp);
}
/*^simulation - last 2 parameters are for saving replay data*/


for(i=1;i<=nob;i++){
  if(objs[i].lev==3){
    rotab(&objs[i],objs[i].vx[0],objs[i].vy[0],objs[i].vz[0],objs[i].vx[3],objs[i].vy[3],objs[i].vz[3],vrot3*tframe);
  }
}

rdspeed(&car,&speed,&rotspeed,&dspeed);
acc=dspeed/tframe;

backcol.red=1+dmode; zfog=speed; /*no text on screen with OpenGL*/
/*
switch(dmode){
  case 1: sprintf(textglob,"%3.0f km/h",speed*3.6);
          break;
  case -1: sprintf(textglob,"%3.0f km/h-R",speed*3.6);
          break;
  default: break;
}
*/

#if SOUND==1
volum[1]=rotspeed; if (volum[1]>200){volum[1]=200;}
volum[5]=acc;
#endif


setcamg(&camera,&car,camflag);

rotc+=vrotc*tframe; if(camflag==2){rotc=0; vrotc=0;}
if(rotc){rotatx(&camera,objs[car.oid[1]].vy[0],objs[car.oid[1]].vz[0],rotc);}

odis(screen,objs,nob,backcol,zfog,zmax,&camera,&light); /*display image*/

dstr+=(speed*tframe);


while(SDL_PollEvent(&event)){
switch(event.type){

case SDL_KEYDOWN:
	switch(event.key.keysym.sym){
		case SDLK_q:
		case SDLK_UP: af=dmode;
		             break;
		case SDLK_a:
		case SDLK_DOWN: bf=1;
		             break;
		case SDLK_o:
		case SDLK_LEFT: turn=-1;
		             break;
		case SDLK_p:
		case SDLK_RIGHT: turn=1;
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
		case SDLK_UP: af=0;
		             break;
		case SDLK_a:
		case SDLK_DOWN: bf=0;
		             break;
		case SDLK_o:
		case SDLK_LEFT: if(turn==-1){turn=0;}
		             break;
		case SDLK_p:
		case SDLK_RIGHT: if(turn==1){turn=0;}
		             break;

		case SDLK_r: af=0;
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

#if SOUND==1
/* Stop playing */
SDL_PauseAudio(1);
SDL_CloseAudio();
#endif

SDL_Quit();

#if SOUND==1
free(obtained);
#endif

for(i=1;i<=nto;i++){free(fceglob[i]);}
free(fceglob); free (refglob); free(objs);

#if REPLAY==1
fclose(repf);
#endif

/* printf("Press ENTER: ");getchar();printf("\r\n"); */

dWorldDestroy(wglob);
dCloseODE();

return 0;}
