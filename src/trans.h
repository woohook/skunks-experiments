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

/*function which translates an object by x,y and z*/
void translat(sgob *objs,REALD x,REALD y,REALD z)
{int i;

  for(i=1;i<=objs->nref;i++){
    objs->xref[i]+=x;
    objs->yref[i]+=y;
    objs->zref[i]+=z;
  }

  for(i=0;i<=3;i++){
    objs->transform.vx[i]+=x;
    objs->transform.vy[i]+=y;
    objs->transform.vz[i]+=z;
  }
}

/*functie care roteste un obiect in jurul unei axe paralele cu z care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatz(sgob *objs,REALD x,REALD y,REALD tt)
{int i,nref;
REALD xtm,sintt,costt;

  nref=objs->nref;

sintt=sin(tt);costt=cos(tt);

  for(i=1;i<=nref;i++){
    xtm=objs->xref[i];
    objs->xref[i]=x+(objs->xref[i]-x)*costt-(objs->yref[i]-y)*sintt;
    objs->yref[i]=y+(objs->yref[i]-y)*costt+(xtm-x)*sintt;
  }

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*costt-(objs->transform.vy[i]-y)*sintt;
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*costt+(xtm-x)*sintt;
  }
}

/*functie care roteste un obiect in jurul unei axe paralele cu y care intersecteaza
planul xOz intr-un punct de coordonate x si z*/
void rotaty(sgob *objs,REALD x,REALD z,REALD tt)
{int i,nref;
REALD xtm,sintt,costt;

  nref=objs->nref;

sintt=sin(tt);costt=cos(tt);

  for(i=1;i<=nref;i++){
    xtm=objs->xref[i];
    objs->xref[i]=x+(objs->xref[i]-x)*costt+(objs->zref[i]-z)*sintt;
    objs->zref[i]=z+(objs->zref[i]-z)*costt-(xtm-x)*sintt;
  }

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*costt+(objs->transform.vz[i]-z)*sintt;
    objs->transform.vz[i]=z+(objs->transform.vz[i]-z)*costt-(xtm-x)*sintt;
  }
}


/*functie care roteste toate triunghiurile in jurul unei axe paralele cu x care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatx(sgob *objs,REALD y,REALD z,REALD tt)
{int i,nref;
REALD ytm,sintt,costt;

  nref=objs->nref;

sintt=sin(tt);costt=cos(tt);

  for(i=1;i<=nref;i++){
    ytm=objs->yref[i];
    objs->yref[i]=y+(objs->yref[i]-y)*costt-(objs->zref[i]-z)*sintt;
    objs->zref[i]=z+(objs->zref[i]-z)*costt+(ytm-y)*sintt;
  }

  for(i=0;i<=3;i++){
    ytm=objs->transform.vy[i];
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*costt-(objs->transform.vz[i]-z)*sintt;
    objs->transform.vz[i]=z+(objs->transform.vz[i]-z)*costt+(ytm-y)*sintt;
  }
}


/*functie care roteste un obiect in jurul unei axe oarecare care trece prin A1(x,y,z) si B(xb,yb,zb)*/
void rotab(sgob *objs,REALD x,REALD y,REALD z,REALD xb,REALD yb,REALD zb,REALD tt)
{int i,nref;
REALD xtm,
sinalf=0,cosalf=0,sinbt=0,cosbt=0,sintt,costt,
len2,len1, /*lungimea segmentului AB si a proiectiei lui in planul xOy*/
abx,aby,abz, /*lungimile proiectiilor segmentului AB pe cele 3 axe*/
thres=1e-5; /*valoare minima admisa pentru len1*/

  nref=objs->nref;

abx=xb-x; aby=yb-y; abz=zb-z;
len1=sqrt(abx*abx+aby*aby);
len2=sqrt(abx*abx+aby*aby+abz*abz);

if(len1>thres){sinalf=aby/len1; cosalf=abx/len1;
	       sinbt=len1/len2; cosbt=abz/len2;
	      }else{if(abz<0){tt=-tt;}}

sintt=sin(tt);costt=cos(tt);

	if(len1>thres){
/*1 - rotire cu (-alfa) in jurul axei z*/
  for(i=1;i<=nref;i++){
    xtm=objs->xref[i];
    objs->xref[i]=x+(objs->xref[i]-x)*cosalf+(objs->yref[i]-y)*sinalf;
    objs->yref[i]=y+(objs->yref[i]-y)*cosalf-(xtm-x)*sinalf;
  }

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*cosalf+(objs->transform.vy[i]-y)*sinalf;
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*cosalf-(xtm-x)*sinalf;
  }
  
/*2 - rotire cu (-beta) in jurul axei y*/
  for(i=1;i<=nref;i++){
    xtm=objs->xref[i];
    objs->xref[i]=x+(objs->xref[i]-x)*cosbt-(objs->zref[i]-z)*sinbt;
    objs->zref[i]=z+(objs->zref[i]-z)*cosbt+(xtm-x)*sinbt;
  }

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*cosbt-(objs->transform.vz[i]-z)*sinbt;
    objs->transform.vz[i]=z+(objs->transform.vz[i]-z)*cosbt+(xtm-x)*sinbt;
  }
	}
/*3 - rotire cu teta in jurul axei z*/
  for(i=1;i<=nref;i++){
    xtm=objs->xref[i];
    objs->xref[i]=x+(objs->xref[i]-x)*costt-(objs->yref[i]-y)*sintt;
    objs->yref[i]=y+(objs->yref[i]-y)*costt+(xtm-x)*sintt;
  }

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*costt-(objs->transform.vy[i]-y)*sintt;
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*costt+(xtm-x)*sintt;
  }

	if(len1>thres){
/*4 - rotire cu beta in jurul axei y*/
  for(i=1;i<=nref;i++){
    xtm=objs->xref[i];
    objs->xref[i]=x+(objs->xref[i]-x)*cosbt+(objs->zref[i]-z)*sinbt;
    objs->zref[i]=z+(objs->zref[i]-z)*cosbt-(xtm-x)*sinbt;
  }

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*cosbt+(objs->transform.vz[i]-z)*sinbt;
    objs->transform.vz[i]=z+(objs->transform.vz[i]-z)*cosbt-(xtm-x)*sinbt;
  }
  
/*5 - rotire cu alfa in jurul axei z*/
  for(i=1;i<=nref;i++){
    xtm=objs->xref[i];
    objs->xref[i]=x+(objs->xref[i]-x)*cosalf-(objs->yref[i]-y)*sinalf;
    objs->yref[i]=y+(objs->yref[i]-y)*cosalf+(xtm-x)*sinalf;
  }

  for(i=0;i<=3;i++){
    xtm=objs->transform.vx[i];
    objs->transform.vx[i]=x+(objs->transform.vx[i]-x)*cosalf-(objs->transform.vy[i]-y)*sinalf;
    objs->transform.vy[i]=y+(objs->transform.vy[i]-y)*cosalf+(xtm-x)*sinalf;
  }
	}
}
