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

#include "sdltext.h"
char textglob[MAXWLG];

typedef struct _pixcol
{int red;int green;int blue; /*culoarea pixelului*/
} pixcol;

typedef struct _tria
{REALN x1; REALN y1;REALN z1;
REALN x2; REALN y2;REALN z2;
REALN x3; REALN y3;REALN z3;
int red; int green; int blue; /*culoarea triunghiului*/
int redd; int greend; int blued;
unsigned char cull; /*first bit(&1):1-cull;0-no cull;second bit(&2):1-fullbright;0-usual*/
} tria;

typedef struct _tripf
{REALN x1; REALN y1;
REALN x2; REALN y2;
REALN x3; REALN y3;
} tripf;

typedef struct _trilim
{int imin;int imax;} trilim;

typedef struct _lightpr
{REALN ambient;
REALN headlight;
REALN directional;
REALN dx;
REALN dy;
REALN dz;
} lightpr; /*light parameters*/

/*functie care elimina triunghiurile care sunt in plus*/
int fclip(tria *face,int nrfaces,REALN zmin,tria *facedisp,REALN zmax,REALN tgh,REALN tgv)
{int i,j,k,l,kmin,invs;
REALN x[4],y[4],z[4],tmp,tmp2,min;
j=0; /*variabila pt. numarat triunghiurile afisate*/
for(i=1;i<=nrfaces;i++){

  if((face[i].z1<=zmin)&&(face[i].z2<=zmin)&&(face[i].z3<=zmin)){continue;}
  if((face[i].z1>=zmax)&&(face[i].z2>=zmax)&&(face[i].z3>=zmax)){continue;}
    if((face[i].x1>tgv*face[i].z1)&&(face[i].x2>tgv*face[i].z2)&&(face[i].x3>tgv*face[i].z3)){continue;}
    if((face[i].y1>tgh*face[i].z1)&&(face[i].y2>tgh*face[i].z2)&&(face[i].y3>tgh*face[i].z3)){continue;}
  if((face[i].x1<-tgv*face[i].z1)&&(face[i].x2<-tgv*face[i].z2)&&(face[i].x3<-tgv*face[i].z3)){continue;}
  if((face[i].y1<-tgh*face[i].z1)&&(face[i].y2<-tgh*face[i].z2)&&(face[i].y3<-tgh*face[i].z3)){continue;}

  if((face[i].z1>zmin)&&(face[i].z2>zmin)&&(face[i].z3>zmin)){j++;facedisp[j]=face[i];}
  else{
    x[1]=face[i].x1;x[2]=face[i].x2;x[3]=face[i].x3;
    y[1]=face[i].y1;y[2]=face[i].y2;y[3]=face[i].y3;
    z[1]=face[i].z1;z[2]=face[i].z2;z[3]=face[i].z3;

    invs=1;

    for(k=1;k<=2;k++){min=z[k];kmin=k;
      for(l=k+1;l<=3;l++){
	if(z[l]<z[kmin]){kmin=l;}
      }if(kmin!=k){ invs*=-1;
	tmp=x[k];x[k]=x[kmin];x[kmin]=tmp;
	tmp=y[k];y[k]=y[kmin];y[kmin]=tmp;
	tmp=z[k];z[k]=z[kmin];z[kmin]=tmp;
      }
    } /*ordonat varfuri dupa z*/

  if((z[1]<=zmin)&&(z[2]<=zmin)){
    j++;
      tmp=(zmin-z[1])/(z[3]-z[1]);
        facedisp[j].x1=tmp*(x[3]-x[1])+x[1];
        facedisp[j].y1=tmp*(y[3]-y[1])+y[1];
      tmp=(zmin-z[2])/(z[3]-z[2]);
        facedisp[j].x2=tmp*(x[3]-x[2])+x[2];
        facedisp[j].y2=tmp*(y[3]-y[2])+y[2];
	  facedisp[j].x3=x[3];
	  facedisp[j].y3=y[3];
	    facedisp[j].z1=facedisp[j].z2=zmin;
	    facedisp[j].z3=z[3];
	      facedisp[j].red=face[i].red;facedisp[j].green=face[i].green;facedisp[j].blue=face[i].blue;

	      facedisp[j].cull=face[i].cull;
	      if(invs==(-1)){
	        tmp2=facedisp[j].x1; facedisp[j].x1=facedisp[j].x2; facedisp[j].x2=tmp2;
                tmp2=facedisp[j].y1; facedisp[j].y1=facedisp[j].y2; facedisp[j].y2=tmp2;
                tmp2=facedisp[j].z1; facedisp[j].z1=facedisp[j].z2; facedisp[j].z2=tmp2;
	      }
  }else{
    j++;
      tmp=(zmin-z[1])/(z[2]-z[1]);
        facedisp[j].x1=tmp*(x[2]-x[1])+x[1];
	facedisp[j].y1=tmp*(y[2]-y[1])+y[1];
	  facedisp[j].x2=x[2];
	  facedisp[j].y2=y[2];
	    facedisp[j].x3=x[3];
	    facedisp[j].y3=y[3];
	      facedisp[j].z1=zmin;
	      facedisp[j].z2=z[2];
	      facedisp[j].z3=z[3];
	        facedisp[j].red=face[i].red;facedisp[j].green=face[i].green;facedisp[j].blue=face[i].blue;

	        facedisp[j].cull=face[i].cull;
	        if(invs==(-1)){
	          tmp2=facedisp[j].x1; facedisp[j].x1=facedisp[j].x2; facedisp[j].x2=tmp2;
                  tmp2=facedisp[j].y1; facedisp[j].y1=facedisp[j].y2; facedisp[j].y2=tmp2;
                  tmp2=facedisp[j].z1; facedisp[j].z1=facedisp[j].z2; facedisp[j].z2=tmp2;
	        }
    j++;
        facedisp[j].x1=tmp*(x[2]-x[1])+x[1];
	facedisp[j].y1=tmp*(y[2]-y[1])+y[1];
      tmp=(zmin-z[1])/(z[3]-z[1]);
        facedisp[j].x2=tmp*(x[3]-x[1])+x[1];
	facedisp[j].y2=tmp*(y[3]-y[1])+y[1];
	  facedisp[j].x3=x[3];
	  facedisp[j].y3=y[3];
	    facedisp[j].z1=facedisp[j].z2=zmin;
	    facedisp[j].z3=z[3];
	      facedisp[j].red=face[i].red;facedisp[j].green=face[i].green;facedisp[j].blue=face[i].blue;

	      facedisp[j].cull=face[i].cull;
	      if(invs==(1)){
	        tmp2=facedisp[j].x1; facedisp[j].x1=facedisp[j].x2; facedisp[j].x2=tmp2;
                tmp2=facedisp[j].y1; facedisp[j].y1=facedisp[j].y2; facedisp[j].y2=tmp2;
                tmp2=facedisp[j].z1; facedisp[j].z1=facedisp[j].z2; facedisp[j].z2=tmp2;
	      }
  }
  }

}

return j;}


/*functie care verifica daca punctul M(x,y,z) se afla in interiorul triunghiului ABC
A(x1,y1,z1);B(x2,y2,z2);c(x3,y3,z3)
(punctul M apartine planului ABC)
nu se mai foloseste functia asta*/
int vertri(REALN x1,REALN y1,REALN x2,REALN y2,REALN x3,REALN y3,REALN x,REALN y)
{REALN l1,l2,m1,m2,p; /*p - produs vectorial*/
char s1,s2,s3; /*semnele celor 3 produse vectoriale care vor fi calculate*/

  l1=x2-x1;l2=x-x1;
  m1=y2-y1;m2=y-y1;
  p=l1*m2-l2*m1; if(p>0){s1=1;}else{s1=2;}

  l1=x3-x2;l2=x-x2;
  m1=y3-y2;m2=y-y2;
  p=l1*m2-l2*m1; if(p>0){s2=1;}else{s2=2;}

  l1=x1-x3;l2=x-x3;
  m1=y1-y3;m2=y-y3;
  p=l1*m2-l2*m1; if(p>0){s3=1;}else{s3=2;}

  if((s1==s2)&&(s1==s3)){return 1;}

return 0;}


/*functie care determina ecuatia unui plan care trece prin 3 puncte
A(x1,y1,z1);B(x2,y2,z2);c(x3,y3,z3)
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nu se mai foloseste functia asta ci findplan()*/
REALN ccplan(REALN x1,REALN y1,REALN z1,REALN x2,REALN y2,REALN z2,REALN x3,REALN y3,REALN z3,REALN xf,REALN yf,REALN zf)
{REALN a1,b1,c1,a2,b2,c2,a,b,c,d;
REALN xi,yi,zi;
REALN dist; /*patratul distantei de la origine pana la intersectia dreptei cu planul*/

  a1=x1-x2; b1=y1-y2; c1=z1-z2;
  a2=x1-x3; b2=y1-y3; c2=z1-z3;

    a=b1*c2-b2*c1; b=a2*c1-a1*c2; c=a1*b2-a2*b1;

    d=a*x1+b*y1+c*z1;
    /*ecuatia planului este ax+by+cz=d*/

   zi=d/(a*xf/zf+b*yf/zf+c); yi=zi*yf/zf; xi=zi*xf/zf;
   /*xi,yi si zi sunt punctele de intersectie ale dreptei cu planul*/

   if(zi<0){return 0;}

   dist=xi*xi+yi*yi+zi*zi;

return dist;}


/*functie care determina ecuatia planului cu numarul de ordine 'i'
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nrfaces - numar total de triunghiuri*/
void findplan(tria *face,int i,REALN *a,REALN *b,REALN *c,REALN *d)
{REALN a1,b1,c1,a2,b2,c2;

	a1=face[i].x1-face[i].x2; b1=face[i].y1-face[i].y2; c1=face[i].z1-face[i].z2;
	a2=face[i].x1-face[i].x3; b2=face[i].y1-face[i].y3; c2=face[i].z1-face[i].z3;

	*a=b1*c2-b2*c1; *b=a2*c1-a1*c2; *c=a1*b2-a2*b1;
	*d=(*a)*face[i].x1+(*b)*face[i].y1+(*c)*face[i].z1;
	/*ecuatia planului este ax+by+cz=d*/
}


void displaysdl(SDL_Surface *screen,tria *face,int nrfaces,REALN *distmin,unsigned int width,unsigned int height,REALN focal,pixcol backcol,REALN zfog,REALN zmax,lightpr *light)
{int i,j,jmin,jmax,xcr,ycr,isp,bitd,red,green,blue,red0,green0,blue0;
Uint8 *ptr;
pixcol pixcb; /*culoarea pixelului curent*/
REALN ystart,yend,zf,dist;
unsigned long int idx,crf,area;
REALN tmp,ratio2,ratio3,ratio4,thres;
tripf ftr; /*proiectii varfuri triunghi*/
trilim lim; /*limite triunghi pe axa y*/
REALN a,b,c,d,izf, /*izf=1/zf - pt. marit viteza; ecuatia planului este ax+by+cz=d*/
      a1,bright,redf,greenf,bluef,
      aizf,bizf,aizfxcr,id,izmax,izfog;

#if DOUBLEPIX==1
  width/=2; height/=2; focal/=2;
#elif DOUBLEPIX!=0
  printf("Error: DOUBLEPIX should be set to 0 or 1 in 'src/config.h'\r\n"); exit(1);
#endif

red0=backcol.red; green0=backcol.green; blue0=backcol.blue;

thres=1/(width+height);

area=width*height+1;

izmax=1/zmax; izfog=1/zfog;

bitd=CLBITS/8;

/*desenare imagine*/
for(i=0;i<=(int)area;i++){distmin[i]=izmax;}
zf=-focal;
izf=1/zf;

/*Lock screen*/
if(SDL_MUSTLOCK(screen)){
  if(SDL_LockSurface(screen)<0){
    printf("Can't lock screen: %s\n", SDL_GetError());SDL_Quit();return;}}

for(crf=1;(int)crf<=nrfaces;crf++){

findplan(face,crf,&a,&b,&c,&d);

aizf=a*izf; bizf=b*izf; id=1/d;

#if ASPCOR==1
  bizf/=WIDTHFACTOR;
#endif

/*start lighting and backface culling*/
tmp=a*face[crf].x1+b*face[crf].y1+c*face[crf].z1;
/*dot product used for lighting and backface culling*/
if((face[crf].cull)&1){ /*backface culling*/
  if(tmp<0){continue;}
}

if((face[crf].cull)&2){ /*fullbright*/
  a1=1;
}else{
  dist=sqrt(a*a+b*b+c*c);
  a1=(light->dx*a+light->dy*b+light->dz*c)/dist;
  if(tmp>=0){
    if(a1<0){a1=0;}
  }else{
    if(a1<0){a1=-a1;}else{a1=0;}
  }
  a1=light->ambient+fabs(light->headlight*c/dist)+light->directional*a1;
}

bright=a1; if(bright<0){bright=0;}

red=(int)(bright*face[crf].red); green=(int)(bright*face[crf].green); blue=(int)(bright*face[crf].blue);
if(red>255){red=255;}
if(green>255){green=255;}
if(blue>255){blue=255;}

face[crf].redd=red;
face[crf].greend=green;
face[crf].blued=blue;
/*finished lighting and backface culling*/

	/*find trangle limits*/
	ftr.x1=face[crf].x1*zf/face[crf].z1;
	ftr.x2=face[crf].x2*zf/face[crf].z2;
	ftr.x3=face[crf].x3*zf/face[crf].z3;

	ftr.y1=face[crf].y1*zf/face[crf].z1;
	ftr.y2=face[crf].y2*zf/face[crf].z2;
	ftr.y3=face[crf].y3*zf/face[crf].z3;

	if(ftr.x2>ftr.x3){tmp=ftr.y2;ftr.y2=ftr.y3;ftr.y3=tmp; tmp=ftr.x2;ftr.x2=ftr.x3;ftr.x3=tmp;}
	if(ftr.x1>ftr.x2){{tmp=ftr.y1;ftr.y1=ftr.y2;ftr.y2=tmp; tmp=ftr.x1;ftr.x1=ftr.x2;ftr.x2=tmp;}
	if(ftr.x2>ftr.x3){tmp=ftr.y2;ftr.y2=ftr.y3;ftr.y3=tmp; tmp=ftr.x2;ftr.x2=ftr.x3;ftr.x3=tmp;}}
	/*ordered triangle vertices*/

	lim.imax=(int)((int)(height>>1)+ftr.x3); lim.imin=(int)((int)(height>>1)+ftr.x1+1);

	if(lim.imin<0){lim.imin=0;}
	if(lim.imax>((int)height-1)){lim.imax=(int)height-1;}
	/*found triangle limits*/

    if(fabs(ftr.x2-ftr.x1)>thres){ratio2=(ftr.y2-ftr.y1)/(ftr.x2-ftr.x1);}else{ratio2=1e5;}
    if(fabs(ftr.x3-ftr.x1)>thres){ratio3=(ftr.y3-ftr.y1)/(ftr.x3-ftr.x1);}else{ratio3=1e5;}
    if(fabs(ftr.x3-ftr.x2)>thres){ratio4=(ftr.y3-ftr.y2)/(ftr.x3-ftr.x2);}else{ratio4=1e5;}

for(i=lim.imin;i<=lim.imax;i++){

    isp=i*screen->pitch;

    xcr=-(int)(height>>1)+i;

    aizfxcr=aizf*xcr;

    if(xcr<ftr.x2){
      ystart=(ratio2*(xcr-ftr.x1)+ftr.y1);
      yend=(ratio3*(xcr-ftr.x1)+ftr.y1);}
    else{
      ystart=(ratio4*(xcr-ftr.x2)+ftr.y2);
      yend=(ratio3*(xcr-ftr.x1)+ftr.y1);}
    if(yend<ystart){tmp=ystart;ystart=yend;yend=tmp;}

#if ASPCOR==1
  ystart*=WIDTHFACTOR;
  yend*=WIDTHFACTOR;
#endif

    jmin=(int)(-yend+(int)(width>>1))+1; jmax=(int)(-ystart+(int)(width>>1));
    if(jmin<0){jmin=0;}
    if(jmax>((int)width-1)){jmax=(int)width-1;}

#if DOUBLEPIX==0
  ptr=(Uint8 *)screen->pixels+isp+jmin*bitd;
#else
  ptr=(Uint8 *)screen->pixels+2*isp+jmin*bitd*2;
#endif

  idx=i*width+jmin;

  for(j=jmin;j<=jmax;j++){
    ycr=(int)(width>>1)-j;
      dist=id*(aizfxcr+bizf*ycr+c);
        idx++;
	  if(distmin[idx]<dist){
	    distmin[idx]=dist;
	    ptr[0] = face[crf].blued;
            ptr[1] = face[crf].greend;
            ptr[2] = face[crf].redd;
	  }

#if DOUBLEPIX==0
	ptr+=bitd;
#else
	ptr+=2*bitd;
#endif

  }
}
}

crf=0; ratio2=1/(izmax-izfog);
for(j=0;j<=(int)height-1;j++){
  tmp=1.0-sin(3.1416*j/(REALN)height); tmp*=50;
  redf=red0-tmp; greenf=green0-tmp; bluef=blue0-tmp;
  backcol.red=(int)redf; backcol.green=(int)greenf; backcol.blue=(int)bluef;

#if DOUBLEPIX==0
  ptr=(Uint8 *)screen->pixels + j*screen->pitch;
#else
  ptr=(Uint8 *)screen->pixels + 2*j*screen->pitch;
  isp=screen->pitch;
#endif

for(i=0;i<=(int)width-1;i++){
if(distmin[++crf]==izmax){
	ptr[0]=backcol.blue;
	ptr[1]=backcol.green;
	ptr[2]=backcol.red;}
      else{if(distmin[crf]<izfog){
	pixcb.red=ptr[2];
	pixcb.green=ptr[1];
	pixcb.blue=ptr[0];

        tmp=(distmin[crf]-izfog)*ratio2;
        /*pixcb.red=(int)(pixcb.red*(1-tmp));
        pixcb.green=(int)(pixcb.green*(1-tmp));
        pixcb.blue=(int)(pixcb.blue*(1-tmp));*/
	pixcb.red+=(int)(tmp*(backcol.red-pixcb.red));
	pixcb.green+=(int)(tmp*(backcol.green-pixcb.green));
	pixcb.blue+=(int)(tmp*(backcol.blue-pixcb.blue));
	

	ptr[0] = pixcb.blue;
        ptr[1] = pixcb.green;
        ptr[2] = pixcb.red;
      }}

#if DOUBLEPIX==0
    ptr+=bitd;
#else
    ptr[bitd]=ptr[isp]=ptr[isp+bitd]=ptr[0];
    ptr[bitd+1]=ptr[isp+1]=ptr[isp+bitd+1]=ptr[1];
    ptr[bitd+2]=ptr[isp+2]=ptr[isp+bitd+2]=ptr[2];
    ptr+=2*bitd;
#endif

}}

#if DOUBLEPIX==1
width*=2; height*=2;
#endif

dtext(screen,textglob,0.35*width,0.05*height,20);

/*Unlock screen*/
if(SDL_MUSTLOCK(screen)){SDL_UnlockSurface(screen);}
/* Update display*/
  SDL_UpdateRect(screen, 0, 0, 0, 0);
/*desenat imagine*/
}



