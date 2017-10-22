/*
Copyright (C) 2013 Victor Matei Petrescu

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


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL.h>
#include "crs_sdl.h"


#define SCREENWIDTH 640
#define SCREENHEIGHT 480


void dtel(float ycr,float zcr,float scale,int tcol) /*draw track element*/
{static float lasty=0.0,lastz=0.0;
float dy,dz;

dpoint(0.5*SCREENWIDTH+lasty*0.1*scale,0.5*SCREENHEIGHT+lastz*0.1*scale,tcol);
dy=0.1*scale*(ycr-lasty);
dz=0.1*scale*(zcr-lastz);
dline(dy,dz,tcol);

lasty=ycr; lastz=zcr;
refresh();
}


int main()
{int quit=0,tcol=10; /*tcol-line colour*/
float ycr=0.0,zcr=0.0,ttcr=0.0,tlen=0.0,
      scale=8.0; /*8 pix:10m*/
char sel;
FILE *f;


initgraph(SCREENWIDTH,SCREENHEIGHT,0);


initscr();start_color();
init_pair(1,COLOR_WHITE,COLOR_BLUE);
init_pair(2,COLOR_WHITE,COLOR_BLACK);
init_pair(3,COLOR_BLUE,COLOR_GREEN);
init_pair(4,COLOR_YELLOW,COLOR_BLACK);
init_pair(5,COLOR_CYAN,COLOR_BLACK);
init_pair(6,COLOR_BLACK,COLOR_GREEN);
init_pair(7,COLOR_WHITE,COLOR_RED);
init_pair(8,COLOR_RED,COLOR_BLACK);
init_pair(9,COLOR_BLACK,COLOR_CYAN);
init_pair(10,COLOR_GREEN,COLOR_BLACK);
bkgd(COLOR_PAIR(2));curs_set(1);
/*initializat ecran grafic*/


if(!(f=fopen("trk","w"))){printf("Could not open 'trk'\r\n"); exit(1);}


while(!quit){

move(0,0); sel=getch_graph(); move(0,0); printw("     ");

switch(sel){

  case '0':
    fprintf(f,"1 0\r\n");
    ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
    break;

  case '3':
    fprintf(f,"1 3\r\n");
    ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
    break;

  case '4':
    fprintf(f,"1 4\r\n");
    ycr+=9.0031*sin(ttcr)-3.7292*cos(ttcr); zcr+=9.0031*cos(ttcr)+3.7292*sin(ttcr); ttcr+=-0.7853982;
    break;

  case '1':
    fprintf(f,"1 1\r\n");
    ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
    break;

  case '2':
    fprintf(f,"1 2\r\n");
    ycr+=9.0031*sin(ttcr)+3.7292*cos(ttcr); zcr+=9.0031*cos(ttcr)-3.7292*sin(ttcr); ttcr+=0.7853982;
    break;

  case 'a':
    fprintf(f,"1 14\r\n");
    tcol=25;
    ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
    break;

  case 'b':
    fprintf(f,"1 15\r\n");
    tcol=20;
    ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
    break;

  case 'c':
    fprintf(f,"1 16\r\n");
    tcol=15;
    ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
    break;


  case 'q': quit=1;

  default: break;
}

if(!quit){
  tlen+=10.0;
  attron(10);
  move(1,0); printw("Track length: %1.1f m",tlen);
  move(2,0); printw("Direction: %1.1f deg",ttcr*180.0/3.1415927);
  attron(2);
  dtel(ycr,zcr,scale,tcol);
  tcol=10; /*default*/
}

}


fclose(f);

freegraph();

return 0;}
