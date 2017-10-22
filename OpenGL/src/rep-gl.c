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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <time.h>
#include "config.h"

#include "rendergl.h" /*just 1 file, 16 bit color*/

#include "rep_dfst.h"
#include "trans.h"
#include "camera.h"
#include "rep_rdf.h"
#include "rep_game.h"

#define GLWIDTH 1024
#define GLHEIGHT 768

int main(int argc,char *argv[])
{char numefis1[MAXWLG],numefis2[MAXWLG];

int i,quit=0,
    t0frame; /*t0frame - moment when image starts to be displayed*/

SDL_Event event;
SDL_Surface *screen;

pixcol backcol; /*culoarea fundalului*/
REALN  zfog,zmax,
       vfov; /*zfog,zmax - distanta de la care incepe ceatza, respectiv de la care nu se mai vede nimic*/
lightpr light;

sgob *objs,camera; /*objects*/
int nob,nto,camflag=1; /*number of objects and of object types*/

vhc car; /*vehicle*/

REALN tframe=0,xan=0,/*tframe-time necessary for display; xan-number of displayed images*/
      timp,dstr; /*total time, distance traveled*/

/*for game*/
REALN vrot3=1.57, /*rot. speed of level 3 objects*/
      vrotc=0.0,vrcmax=0.79,rotc=0.0, /*rot. speed and rotation of camera*/
      speed;
FILE *repf;
/*for game^*/


zfog=40;
zmax=240; /*visibility (m)*/

camera.vx[0]=0; camera.vy[0]=0; camera.vz[0]=0;
camera.vx[1]=1; camera.vy[1]=0; camera.vz[1]=0;
camera.vx[2]=0; camera.vy[2]=1; camera.vz[2]=0;
camera.vx[3]=0; camera.vy[3]=0; camera.vz[3]=1; /*set camera parameters*/

if(argc<=1){printf("Error: Input files not specified\r\nExample: ./replay replays/rep2\r\n");exit(1);}
if(argc>=3){printf("Error: Too many arguments\r\n");exit(1);}

strcpy(numefis1,argv[1]);
if(!(repf=fopen(numefis1,"r"))){printf("Error: could not open '%s'\r\n",numefis1); exit(1);}

fscanf(repf,"%s",numefis1);
fscanf(repf,"%s",numefis2);

objs=readtrack(numefis2,&nob,&nto,&backcol,&light); /*read objects from file*/
objs=readvehicle(numefis1,objs,&nto,&nob,&car); /*read vehicle from file*/

printf("\r\n");

backcol.glred=(GLfloat)(backcol.red)/255;
backcol.glgreen=(GLfloat)(backcol.green)/255;
backcol.glblue=(GLfloat)(backcol.blue)/255; /*background color for OpenGL*/


/*Initialize SDL*/
if(SDL_Init(SDL_INIT_VIDEO)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}

   SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
   SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE,5 );
   SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 16 );

/*Initialize display*/
screen=SDL_SetVideoMode(GLWIDTH,GLHEIGHT,CLBITS,SDL_OPENGL|SDL_FULLSCREEN);
if(screen == NULL){printf("Couldn't set requested video mode: %s\n",SDL_GetError());SDL_Quit();return 0;}
printf("Set %dx%dx%d\n",(screen->pitch)/(screen->format->BytesPerPixel),SCREENHEIGHT,screen->format->BitsPerPixel);
/*Set window title*/
SDL_WM_SetCaption("Skunks-4.2.0 - replay - OpenGL", "Skunks-4.2.0 - replay - OpenGL");
/* Enable Unicode translation */
SDL_EnableUNICODE(1);
/*SDL initialized*/
SDL_ShowCursor(SDL_DISABLE);

   glViewport(0,0,GLWIDTH,GLHEIGHT);
   glClearColor(backcol.glred,backcol.glgreen,backcol.glblue,0.0f); /*background color*/
   glClearDepth(0.0);
   glDepthFunc(GL_GREATER);
   glEnable(GL_DEPTH_TEST);
   glShadeModel(GL_SMOOTH);
   glMatrixMode(GL_PROJECTION);

   gluLookAt(
   0.0f, 0.0f, 0.0f,
   0.0f, 0.0f, 1.0f,
   0.0f, 1.0f, 0.0f);

#if ASPCOR==0
   vfov=114.6*atan(tan(0.008727*FOV)*GLHEIGHT/GLWIDTH);
   gluPerspective(vfov,(GLfloat)GLWIDTH/(GLfloat)GLHEIGHT,0.25f,1.5*zmax); /*0.25, zmax - clipping planes*/
#else
   vfov=114.6*atan(tan(0.008727*FOV)*GLHEIGHT*(GLfloat)WIDTHFACTOR/GLWIDTH);
   gluPerspective(vfov,(GLfloat)GLWIDTH/(GLfloat)GLHEIGHT/(GLfloat)WIDTHFACTOR,0.25f,1.5*zmax); /*0.25, zmax - clipping planes*/
#endif

   glPolygonOffset(10.0f,0.001f);
   glMatrixMode(GL_MODELVIEW);

speed=0;

timp=0,dstr=0; /*pornit cronometru*/
tframe=0.5; /*assuming 2 frames/second*/


while(!quit){

/*t0frame=clock();*/
t0frame=SDL_GetTicks();
xan++;



runsim(objs,&car,repf,&timp,&speed,0);
/*^reads replay data*/


for(i=1;i<=nob;i++){
  if(objs[i].lev==3){
    rotab(&objs[i],objs[i].vx[0],objs[i].vy[0],objs[i].vz[0],objs[i].vx[3],objs[i].vy[3],objs[i].vz[3],vrot3*tframe);
  }
}

backcol.glred=2; backcol.glgreen=speed; /*no text on screen with OpenGL*/
/*sprintf(textglob,"%3.0f km/h",speed*3.6);*/



setcamg(objs,&camera,&car,camflag);

rotc+=vrotc*tframe; if(camflag==2){rotc=0; vrotc=0;}
if(rotc){rotatx(&camera,objs[car.oid[1]].vy[0],objs[car.oid[1]].vz[0],rotc);}

odis(screen,objs,nob,backcol,zfog,zmax,&camera,&light); /*display image*/

dstr+=(speed*tframe);


while(SDL_PollEvent(&event)){
switch(event.type){

case SDL_KEYDOWN:
	switch(event.key.keysym.sym){
		case SDLK_c: camflag++; if(camflag>2){camflag=1;}
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
timp+=tframe;
if(feof(repf)){quit=1;}
}


printf("\r\n\r\n\r\n**********************************************\r\n");
printf("\r\nDistance: %1.2f km\r\nTime: %1.2f seconds\r\n",dstr/1000,timp);
printf("Average speed: %1.2f km/h\r\n",3.6*dstr/timp);
printf("Average framerate: %1.2f f/s\r\n\r\n",xan/timp);
printf("**********************************************\r\n\r\n");


SDL_Quit();


runsim(objs,&car,repf,&timp,&speed,1); /*freed static variables from runsim()*/
for(i=1;i<=nto;i++){free(fceglob[i]);}
free(fceglob); free (refglob); free(objs);

fclose(repf);

odis(0,0,0,backcol,0,0,0,0); /*freed static variables from odis() in "camera.h"*/

/* printf("Press ENTER: ");getchar();printf("\r\n"); */

return 0;}
