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

#include "trans.h"

/*function which translates an object by x,y and z*/
void translat(struct _matrix *mtrx,float x,float y,float z)
{int i;

  for(i=0;i<=3;i++){
    mtrx->vx[i]+=x;
    mtrx->vy[i]+=y;
    mtrx->vz[i]+=z;
  }
}

// function which translates a vector (x,y,z) by dx,dy and dz
void translate_vector(float* x,float* y,float* z, float dx,float dy,float dz)
{
  *x+=dx;
  *y+=dy;
  *z+=dz;
}

/*functie care roteste un obiect in jurul unei axe paralele cu z care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatz(struct _matrix *mtrx,float x,float y,float tt)
{int i;
float xtm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=3;i++){
    xtm=mtrx->vx[i];
    mtrx->vx[i]=x+(mtrx->vx[i]-x)*costt-(mtrx->vy[i]-y)*sintt;
    mtrx->vy[i]=y+(mtrx->vy[i]-y)*costt+(xtm-x)*sintt;
  }
}

void rotate_vector_z(float* x,float* y, float rx, float ry, float tt)
{
  float xtm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  xtm = *x;
  *x = rx+(*x-rx)*costt-(*y-ry)*sintt;
  *y = ry+(*y-ry)*costt+(xtm-rx)*sintt;
}

/*functie care roteste un obiect in jurul unei axe paralele cu y care intersecteaza
planul xOz intr-un punct de coordonate x si z*/
void rotaty(struct _matrix *mtrx,float x,float z,float tt)
{int i;
float xtm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=3;i++){
    xtm=mtrx->vx[i];
    mtrx->vx[i]=x+(mtrx->vx[i]-x)*costt+(mtrx->vz[i]-z)*sintt;
    mtrx->vz[i]=z+(mtrx->vz[i]-z)*costt-(xtm-x)*sintt;
  }
}

void rotate_vector_y(float* x,float* z, float rx, float rz, float tt)
{
  float xtm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  xtm = *x;
  *x = rx+(*x-rx)*costt+(*z-rz)*sintt;
  *z = rz+(*z-rz)*costt-(xtm-rx)*sintt;
}

/*functie care roteste toate triunghiurile in jurul unei axe paralele cu x care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatx(struct _matrix *mtrx,float y,float z,float tt)
{int i;
float ytm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=3;i++){
    ytm=mtrx->vy[i];
    mtrx->vy[i]=y+(mtrx->vy[i]-y)*costt-(mtrx->vz[i]-z)*sintt;
    mtrx->vz[i]=z+(mtrx->vz[i]-z)*costt+(ytm-y)*sintt;
  }
}

void rotate_vector_x(float* y,float* z, float ry, float rz, float tt)
{
  float ytm,sintt,costt;

  sintt=sin(tt);costt=cos(tt);

  ytm = *y;
  *y = ry+(*y-ry)*costt-(*z-rz)*sintt;
  *z = rz+(*z-rz)*costt+(ytm-ry)*sintt;
}

/*functie care roteste un obiect in jurul unei axe oarecare care trece prin A1(x,y,z) si B(xb,yb,zb)*/
void rotab(struct _matrix *mtrx,float x,float y,float z,float xb,float yb,float zb,float tt)
{int i;
float xtm,
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
    xtm=mtrx->vx[i];
    mtrx->vx[i]=x+(mtrx->vx[i]-x)*cosalf+(mtrx->vy[i]-y)*sinalf;
    mtrx->vy[i]=y+(mtrx->vy[i]-y)*cosalf-(xtm-x)*sinalf;
  }
  
/*2 - rotire cu (-beta) in jurul axei y*/
  for(i=0;i<=3;i++){
    xtm=mtrx->vx[i];
    mtrx->vx[i]=x+(mtrx->vx[i]-x)*cosbt-(mtrx->vz[i]-z)*sinbt;
    mtrx->vz[i]=z+(mtrx->vz[i]-z)*cosbt+(xtm-x)*sinbt;
  }
	}
/*3 - rotire cu teta in jurul axei z*/
  for(i=0;i<=3;i++){
    xtm=mtrx->vx[i];
    mtrx->vx[i]=x+(mtrx->vx[i]-x)*costt-(mtrx->vy[i]-y)*sintt;
    mtrx->vy[i]=y+(mtrx->vy[i]-y)*costt+(xtm-x)*sintt;
  }

	if(len1>thres){
/*4 - rotire cu beta in jurul axei y*/
  for(i=0;i<=3;i++){
    xtm=mtrx->vx[i];
    mtrx->vx[i]=x+(mtrx->vx[i]-x)*cosbt+(mtrx->vz[i]-z)*sinbt;
    mtrx->vz[i]=z+(mtrx->vz[i]-z)*cosbt-(xtm-x)*sinbt;
  }
  
/*5 - rotire cu alfa in jurul axei z*/
  for(i=0;i<=3;i++){
    xtm=mtrx->vx[i];
    mtrx->vx[i]=x+(mtrx->vx[i]-x)*cosalf-(mtrx->vy[i]-y)*sinalf;
    mtrx->vy[i]=y+(mtrx->vy[i]-y)*cosalf+(xtm-x)*sinalf;
  }
	}
}

void rotate_vector_ab(float* x,float* y,float* z, float xa, float ya, float za, float xb, float yb, float zb, float tt)
{
  float xtm,
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

void matrix_identity(struct _matrix *mtrx)
{
  mtrx->vx[0] = 0; mtrx->vy[0] = 0; mtrx->vz[0] = 0;
  mtrx->vx[1] = 1; mtrx->vy[1] = 0; mtrx->vz[1] = 0;
  mtrx->vx[2] = 0; mtrx->vy[2] = 1; mtrx->vz[2] = 0;
  mtrx->vx[3] = 0; mtrx->vy[3] = 0; mtrx->vz[3] = 1;
}
