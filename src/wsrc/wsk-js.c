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

#define dDOUBLE
/*for ODE*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL/SDL.h>
#include <ode/ode.h>
#include <time.h>
#include "config.h"


#define SDL_main main
#pragma library ("SDL.lib")
#pragma library ("SDLmain.lib")
#pragma library ("ode_double.lib")


#if CLBITS==16
  #include "render16.h"
#elif CLBITS==24
  #include "render32.h"
#elif CLBITS==32
  #include "render32.h"
#endif

#include "defstr.h"
#include "trans.h"
#include "camera.h"
#include "readfile.h"
#include "game.h"


/*functions for reading joystick configuration*/
int idconf(char *s)
{if(strcmp(s,"sensx")==0){return 1;}
 if(strcmp(s,"sensy")==0){return 2;} /*sensitivies*/
 if(strcmp(s,"deadx")==0){return 3;}
 if(strcmp(s,"deady")==0){return 4;} /*dead zones*/
 if(strcmp(s,"nonlinear")==0){return 5;} /*0-linear, 1-nonlinear steering*/
 return 0;}
 
void readjscfg(REALN *sx,REALN *sy,REALN *dx,REALN *dy,int *nl)
{int err,lincr=1; /*lincr-current line*/
char numefis[MAXWLG],s[MAXWLG];
FILE *fis;

strcpy(numefis,"config/joystick");

if(!(fis=fopen(numefis,"r"))){printf("Error: File %s could not be open\r\n",numefis);exit(1);}
s[0]='1';while(s[0]){
	if(!(err=fisgetw(fis,s,&lincr))){afermex(numefis,lincr,s,1);}

	switch(idconf(s)){
	  case 1: err=fisgetw(fis,s,&lincr); afermex(numefis,lincr,s,2); (*sx)=atof(s);
	          break;
	  case 2: err=fisgetw(fis,s,&lincr); afermex(numefis,lincr,s,2); (*sy)=atof(s);
	          break;
	  case 3: err=fisgetw(fis,s,&lincr); afermex(numefis,lincr,s,2); (*dx)=atof(s);
	          break;
	  case 4: err=fisgetw(fis,s,&lincr); afermex(numefis,lincr,s,2); (*dy)=atof(s);
	          break;
	  case 5: err=fisgetw(fis,s,&lincr); afermex(numefis,lincr,s,0); (*nl)=atoi(s);
	          break;
	  default: break;
	}
}
fclose(fis);
}
/*^functions for reading joystick configuration*/


int main()
{char numefis[MAXWLG];

char argv[3][51];

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


SDL_Joystick *joystick;

pixcol backcol; /*culoarea fundalului*/
REALN  zfog,zmax; /*zfog,zmax - distanta de la care incepe ceatza, respectiv de la care nu se mai vede nimic*/
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
      af=0,bf=0, /*acceleration and brake factors*/
      jsteer, /*joystick steering*/
      sensx=1,sensy=1,ddzx=600,ddzy=3000; /*sensitivities and deadzones for joystick*/
int dmode, /*1 forward, -1 reverse*/
    nstepsf, /*number of simulation steps/frame*/
    nonl=0; /*nonlinear steering (joystick)*/
FILE *repf;
/*for game^*/

readjscfg(&sensx,&sensy,&ddzx,&ddzy,&nonl); /*read joystick configuration*/

zfog=80;
zmax=120; /*visibility (m)*/

camera.vx[0]=0; camera.vy[0]=0; camera.vz[0]=0;
camera.vx[1]=1; camera.vy[1]=0; camera.vz[1]=0;
camera.vx[2]=0; camera.vy[2]=1; camera.vz[2]=0;
camera.vx[3]=0; camera.vy[3]=0; camera.vz[3]=1; /*set camera parameters*/


printf("\r\n Skunks - 4.2.0 - Copyright (C) 2010-2013 Matei Petrescu\r\n\r\n");
printf(" This program is free software; you can redistribute it and/or\r\n\
 modify it under the terms of the GNU General Public License\r\n\
 as published by the Free Software Foundation; either version 3\r\n\
 of the License, or (at your option) any later version.\r\n See the file 'COPYING' for details.\r\n\r\n");

printf(" AVAILABLE SCREEN SIZES:\r\n\r\n 1: 640 x 480 (full screen)\r\n 2: 800 x 600 (full screen)\r\n 3: 800 x 600 wide (full screen)\r\n 4: Custom (window)\r\n\r\n");
printf(" SELECT SCREEN SIZE (1...4): ");
scanf("%d",&nob); printf("\r\n");

switch(nob){
  case 1: SCREENWIDTH=640; SCREENHEIGHT=480; SDLSCREENFLAG=SDL_FULLSCREEN; WIDTHFACTOR=1.0; break;
  case 2: SCREENWIDTH=800; SCREENHEIGHT=600; SDLSCREENFLAG=SDL_FULLSCREEN; WIDTHFACTOR=1.0; break;
  case 3: SCREENWIDTH=800; SCREENHEIGHT=600; SDLSCREENFLAG=SDL_FULLSCREEN; WIDTHFACTOR=0.78; break;
  default: printf(" Screen width (pixels): "); scanf("%d",&SCREENWIDTH);
           printf(" Screen height (pixels): "); scanf("%d",&SCREENHEIGHT);
           SDLSCREENFLAG=SDL_SWSURFACE; WIDTHFACTOR=1.0;
           printf("\r\n");
           break;
}


GSTART:
 
printf(" SELECT CAR (1...7): "); scanf("%d",&i);
if(i>7){i=7;} if(i<1){i=1;}
sprintf(argv[1],"cars/car%d",i); printf("\r\n");

printf(" SELECT TRACK (1...11): "); scanf("%d",&i);
if(i>11){i=11;} if(i<1){i=1;}
sprintf(argv[2],"tracks/track%d",i); printf("\r\n");

quit=0;


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
if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}
/*Initialize display*/
screen=SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,CLBITS,SDLSCREENFLAG);
if(screen == NULL){printf("Couldn't set requested video mode: %s\n",SDL_GetError());SDL_Quit();return 0;}
printf("Set %dx%dx%d\n",(screen->pitch)/(screen->format->BytesPerPixel),SCREENHEIGHT,screen->format->BitsPerPixel);
/*Set window title*/
SDL_WM_SetCaption("Skunks-4.2.0", "Skunks-4.2.0");
/* Enable Unicode translation */
SDL_EnableUNICODE(1);
/*SDL initialized*/
SDL_ShowCursor(SDL_DISABLE);


SDL_JoystickEventState(SDL_ENABLE);
joystick = SDL_JoystickOpen(0);


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
jsteer=0;
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

vrx=jsteer;
  if(speed>10){vrx/=0.1*speed;}


/*simulation*/
nstepsf=(int)(tframe/STIMESTEP)+1; /*number of simulation steps/frame*/
realstep=tframe/nstepsf; /*simulation time step*/

speed=0.1/realstep; /*decrease simulation speed if < 10fps*/
if(nstepsf>(int)speed){nstepsf=(int)speed;}

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

switch(dmode){
  case 1: sprintf(textglob,"%3.0f km/h",speed*3.6);
          break;
  case -1: sprintf(textglob,"%3.0f km/h-R",speed*3.6);
          break;
  default: break;
}


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
		case SDLK_r: af=0;
		             break;

		case SDLK_n: vrotc=0;
		               break;

		case SDLK_m: vrotc=0;
		               break;

		default: break;
	} break;

case SDL_JOYAXISMOTION:  /* Handle Joystick Motion */
    if ( ( event.jaxis.value < -320 ) || (event.jaxis.value > 320 ) ) 
    {
        if( event.jaxis.axis == 0) 
        {
            jsteer=0;
            if(nonl){
              if(event.jaxis.value>ddzx){jsteer=(event.jaxis.value-ddzx)*(event.jaxis.value-ddzx)*sensx*4e-10;}
              if(event.jaxis.value<-ddzx){jsteer=-(event.jaxis.value+ddzx)*(event.jaxis.value+ddzx)*sensx*4e-10;}
            }else{
              if(event.jaxis.value>ddzx*0.2){jsteer=(event.jaxis.value-ddzx)*sensx*1.2e-5;}
              if(event.jaxis.value<-ddzx*0.2){jsteer=(event.jaxis.value+ddzx)*sensx*1.2e-5;}
            }
        }

        if( event.jaxis.axis == 1) 
        {
            if(event.jaxis.value>ddzy){af=0; bf=(event.jaxis.value-ddzy)*sensy*3e-5;}
            if(event.jaxis.value<-ddzy){bf=0; af=-dmode*(event.jaxis.value+ddzx)*sensy*3e-5;}
            if(af>1){af=1;}
            if(bf>1){bf=1;}
        }
    }
    break;

case SDL_JOYBUTTONDOWN:  /* Handle Joystick Button Presses */
    if ( event.jbutton.button == 0 ){af=dmode;}
    if ( event.jbutton.button == 1 ){bf=1;}
    if ( event.jbutton.button == 2 ){dmode=-dmode;}
    break;

case SDL_JOYBUTTONUP:  /* Handle Joystick Button Presses */
    if ( event.jbutton.button == 0 ){af=0;}
    if ( event.jbutton.button == 1 ){bf=0;}
    if ( event.jbutton.button == 2 ){af=0;}
    break;

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

goto GSTART;

odis(0,0,0,backcol,0,0,0,0); /*freed static variables from odis() in "camera.h"*/

return 0;}
