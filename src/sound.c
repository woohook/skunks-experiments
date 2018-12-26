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

#include <SDL.h>
#include "config.h"
#include "sound.h"


#if SOUND==1

SDL_AudioSpec *desired, *obtained;
SDL_AudioSpec *hardware_spec;
REALN volum[6]={0,0,0,0,0,0};


REALN sintr(REALN x)
{const REALN pi=3.1415926536;
REALN y;
x=(x/(2*pi)-floor(x/(2*pi)))*2*pi;
if(x<=pi/2){y=x/(pi/2);return y;}
if((x>pi/2)&&(x<=3*pi/2)){y=1-(x-pi/2)/(pi/2);return y;}
if(x>3*pi/2){y=-1+(x-3*pi/2)/(pi/2);return y;}
return 0;}


REALN sinst(REALN x)
{const REALN pi=3.1415926536;
REALN y;
x=(x/(2*pi)-floor(x/(2*pi)))*2*pi;
if(x<=pi){y=x/(pi/2);return y;}
if(x>pi){y=-1+(x-3*pi/2)/(pi/2);return y;}
return 0;}


REALN sinrd()
{REALN x;
x=rand();
x=(REALN)(((int)x)%100)/50-1;
return x;
}


/*audio callback function for SDL*/
void my_audio_callback(void *userdata, Uint8 *stream, int len)
{static REALD cr=0;
static int sem=0;
int freqtr,i,j1,j2,n=40;
REALN *volum,a,c,tmp;
Uint16 format;
Uint8 channels;
REALN coef;

static REALN freq[41]; /*frequencies of all sounds*/
REALN freqdo=0.3*261.6255653; /*first frequency*/



  if(sem==0){
    sem=1;
    tmp=1.06;
    freq[1]=freqdo;
    for(i=2;i<=40;i++){
      freq[i]=freq[i-1]*tmp;
    }
  } /*set frequencies*/

volum=(REALN *)userdata;

format=(Uint16)volum[2];
channels=(Uint8)volum[3];
freqtr=(int)volum[4];

switch(format){
	case AUDIO_U8: coef=1; break;
	case AUDIO_U16: coef=0.5; break;
	case AUDIO_S8: coef=1; break;
	case AUDIO_S16: coef=0.5; break;
	default: coef=0.5; break;
}
switch(channels){
	case 1: break;
	case 2: coef*=0.5;break;
	default: printf("Unsupported number of channels\r\n");exit(1);
}
coef=coef/freqtr;

/*SDL_Delay(10);*/

a=volum[1]/200; j1=(int)floor(a*(n-2)+1); j2=(int)floor(a*(n-2)+2);
a=a*(n-2)+1-j1;

  for(i=0;i<=len-1;i++){
    stream[i]=63;
    cr++;if(cr>1e6){cr=0;}
      c=0.85+0.15*sin(0.005*cr);
      stream[i]+=(Uint8)(10.0*c*(1.0-a)*sinst((freq[j1])*6.283185307*cr*coef));
      stream[i]+=(Uint8)(10.0*c*a*sinst((freq[j2])*6.283185307*cr*coef));
      if(volum[5]>20){
        stream[i]+=(Uint8)(40.0*c*a*sinrd());
      }

  }
}
#endif

void sound_initialize()
{
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
}

void sound_update(float rotspeed, float acc)
{
#if SOUND==1
  volum[1]=rotspeed;
  if(volum[1]>200)
  {
    volum[1]=200;
  }
  volum[5]=acc;
#endif
}

void sound_terminate()
{
#if SOUND==1
  // Stop playing
  SDL_PauseAudio(1);
  SDL_CloseAudio();
#endif
}

void sound_release()
{
#if SOUND==1
  free(obtained);
#endif
}