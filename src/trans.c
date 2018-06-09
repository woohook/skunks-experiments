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

#include <math.h>
#include <ode/ode.h>

#include "config.h"
#include "trans.h"
#include "defstr.h"

/*function which translates an object by x,y and z*/
void translat(sgob *objs,REALD x,REALD y,REALD z)
{int i;

  for(i=0;i<=3;i++){
    objs->transform.vx[i]+=x;
    objs->transform.vy[i]+=y;
    objs->transform.vz[i]+=z;
  }
}

// function which translates a vector (x,y,z) by dx,dy and dz
void translate_vector(REALD* x,REALD* y,REALD* z, REALD dx,REALD dy,REALD dz)
{
  *x+=dx;
  *y+=dy;
  *z+=dz;
}

/*functie care roteste un obiect in jurul unei axe paralele cu z care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatz(sgob *objs,REALD x,REALD y,REALD tt)
{int i;
REALD xtm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*costt-(objs->transform.vy[i]-y)*sintt;
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*costt+(xtm-x)*sintt;
  }
}

void rotate_vector_z(REALD* x,REALD* y, REALD rx, REALD ry, REALD tt)
{
  REALD xtm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  xtm = *x;
  *x = rx+(*x-rx)*costt-(*y-ry)*sintt;
  *y = ry+(*y-ry)*costt+(xtm-rx)*sintt;
}

/*functie care roteste un obiect in jurul unei axe paralele cu y care intersecteaza
planul xOz intr-un punct de coordonate x si z*/
void rotaty(sgob *objs,REALD x,REALD z,REALD tt)
{int i;
REALD xtm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*costt+(objs->transform.vz[i]-z)*sintt;
    objs->transform.vz[i]=z+(objs->transform.vz[i]-z)*costt-(xtm-x)*sintt;
  }
}

void rotate_vector_y(REALD* x,REALD* z, REALD rx, REALD rz, REALD tt)
{
  REALD xtm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  xtm = *x;
  *x = rx+(*x-rx)*costt+(*z-rz)*sintt;
  *z = rz+(*z-rz)*costt-(xtm-rx)*sintt;
}

/*functie care roteste toate triunghiurile in jurul unei axe paralele cu x care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatx(sgob *objs,REALD y,REALD z,REALD tt)
{int i;
REALD ytm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=3;i++){
    ytm=objs->transform.vy[i];
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*costt-(objs->transform.vz[i]-z)*sintt;
    objs->transform.vz[i]=z+(objs->transform.vz[i]-z)*costt+(ytm-y)*sintt;
  }
}

void rotate_vector_x(REALD* y,REALD* z, REALD ry, REALD rz, REALD tt)
{
  REALD ytm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  ytm = *y;
  *y = ry+(*y-ry)*costt-(*z-rz)*sintt;
  *z = rz+(*z-rz)*costt+(ytm-ry)*sintt;
}

/*functie care roteste un obiect in jurul unei axe oarecare care trece prin A1(x,y,z) si B(xb,yb,zb)*/
void rotab(sgob *objs,REALD x,REALD y,REALD z,REALD xb,REALD yb,REALD zb,REALD tt)
{int i;
REALD xtm,
sinalf=0,cosalf=0,sinbt=0,cosbt=0,sintt,costt,
len2,len1, /*lungimea segmentului AB si a proiectiei lui in planul xOy*/
abx,aby,abz, /*lungimile proiectiilor segmentului AB pe cele 3 axe*/
thres=1e-5; /*valoare minima admisa pentru len1*/

abx=xb-x; aby=yb-y; abz=zb-z;
len1=sqrt(abx*abx+aby*aby);
len2=sqrt(abx*abx+aby*aby+abz*abz);

if(len1>thres){sinalf=aby/len1; cosalf=abx/len1;
	       sinbt=len1/len2; cosbt=abz/len2;
	      }else{if(abz<0){tt=-tt;}}

sintt=sin(tt);costt=cos(tt);

	if(len1>thres){
/*1 - rotire cu (-alfa) in jurul axei z*/
  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*cosalf+(objs->transform.vy[i]-y)*sinalf;
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*cosalf-(xtm-x)*sinalf;
  }
  
/*2 - rotire cu (-beta) in jurul axei y*/
  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*cosbt-(objs->transform.vz[i]-z)*sinbt;
    objs->transform.vz[i]=z+(objs->transform.vz[i]-z)*cosbt+(xtm-x)*sinbt;
  }
	}
/*3 - rotire cu teta in jurul axei z*/
  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*costt-(objs->transform.vy[i]-y)*sintt;
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*costt+(xtm-x)*sintt;
  }

	if(len1>thres){
/*4 - rotire cu beta in jurul axei y*/
  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*cosbt+(objs->transform.vz[i]-z)*sinbt;
    objs->transform.vz[i]=z+(objs->transform.vz[i]-z)*cosbt-(xtm-x)*sinbt;
  }
  
/*5 - rotire cu alfa in jurul axei z*/
  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*cosalf-(objs->transform.vy[i]-y)*sinalf;
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*cosalf+(xtm-x)*sinalf;
  }
	}
}

void rotate_vector_ab(REALD* x,REALD* y,REALD* z, REALD xa, REALD ya, REALD za, REALD xb, REALD yb, REALD zb, REALD tt)
{
  REALD xtm,
    sinalf=0,cosalf=0,sinbt=0,cosbt=0,sintt,costt,
    len2,len1,   // lungimea segmentului AB si a proiectiei lui in planul xOy
    abx,aby,abz, // lungimile proiectiilor segmentului AB pe cele 3 axe
    thres=1e-5;  // valoare minima admisa pentru len1

  abx=xb-xa; aby=yb-ya; abz=zb-za;
  len1=sqrt(abx*abx+aby*aby);
  len2=sqrt(abx*abx+aby*aby+abz*abz);

  if(len1>thres){sinalf=aby/len1; cosalf=abx/len1;
	       sinbt=len1/len2; cosbt=abz/len2;
	      }else{if(abz<0){tt=-tt;}}

  sintt=sin(tt);costt=cos(tt);

  if(len1>thres)
  {
    // 1 - rotire cu (-alfa) in jurul axei z
    xtm = *x;
    *x = xa+(*x-xa)*cosalf+(*y-ya)*sinalf;
    *y = ya+(*y-ya)*cosalf-(xtm-xa)*sinalf;

    // 2 - rotire cu (-beta) in jurul axei y
    xtm = *x;
    *x = xa+(*x-xa)*cosbt-(*z-za)*sinbt;
    *z = za+(*z-za)*cosbt+(xtm-xa)*sinbt;
  }

  // 3 - rotire cu teta in jurul axei z
  xtm = *x;
  *x = xa+(*x-xa)*costt-(*y-ya)*sintt;
  *y = ya+(*y-ya)*costt+(xtm-xa)*sintt;

  if(len1>thres)
  {
    // 4 - rotire cu beta in jurul axei y
    xtm = *x;
    *x = xa+(*x-xa)*cosbt+(*z-za)*sinbt;
    *z = za+(*z-za)*cosbt-(xtm-xa)*sinbt;

    // 5 - rotire cu alfa in jurul axei z
    xtm = *x;
    *x = xa+(*x-xa)*cosalf-(*y-ya)*sinalf;
    *y = ya+(*y-ya)*cosalf+(xtm-xa)*sinalf;
  }
}
