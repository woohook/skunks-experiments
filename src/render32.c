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

#include "config.h"
#include <SDL.h>
#include <ode/ode.h>
#include "defstr.h"
#include "render32.h"

typedef struct _pixcol
{int red;int green;int blue; /*culoarea pixelului*/
} pixcol;

typedef struct _tria
{float x1; float y1;float z1;
float x2; float y2;float z2;
float x3; float y3;float z3;
int red; int green; int blue; /*culoarea triunghiului*/
int redd; int greend; int blued;
unsigned char cull; /*first bit(&1):1-cull;0-no cull;second bit(&2):1-fullbright;0-usual*/
} tria;

typedef struct _tripf
{float x1; float y1;
float x2; float y2;
float x3; float y3;
} tripf;

typedef struct _trilim
{int imin;int imax;} trilim;

typedef struct _lightpr
{float ambient;
float headlight;
float directional;
float dx;
float dy;
float dz;
} lightpr; /*light parameters*/

struct _tria **fceglob = 0; // array with triangles and colors of object types
int mesh_count = 0;
int face_count = 0;
pixcol g_backcol = {0,0,0};
lightpr g_light = {0,0,0,0,0,0};
float g_width_factor = 1.0f;
float g_view_angle = 90.0f;
int g_double_pixel = 0;

void create_mesh()
{
  if(mesh_count==0)
  {
    if(!(fceglob=(tria **)malloc(2*sizeof(tria *)))){printf("Out of memory");}
    mesh_count = 2;
  }
  else
  {
    if(!(fceglob=(tria **)realloc(fceglob,(mesh_count+1)*sizeof(tria *)))){printf("Out of memory");}
    mesh_count++;
  }
  face_count = 0;
}

void add_face(int mesh_id, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
  face_count++;
  if(face_count==1)
  {
    face_count = 2;
    if(!(fceglob[mesh_id]=(tria *)malloc(face_count*sizeof(tria)))){printf("Out of memory");}
  }
  else
  {
    if(!(fceglob[mesh_id]=(tria *)realloc(fceglob[mesh_id], face_count*sizeof(tria)))){printf("Out of memory");}
  }

  tria* face = fceglob[mesh_id];
  int face_id = face_count-1;

  face[face_id].x1 = x1; face[face_id].y1 = y1; face[face_id].z1 = z1;
  face[face_id].x2 = x2; face[face_id].y2 = y2; face[face_id].z2 = z2;
  face[face_id].x3 = x3; face[face_id].y3 = y3; face[face_id].z3 = z3;
  face[face_id].red = 255;
  face[face_id].green = 200;
  face[face_id].blue = 0;

  face[face_id].cull = 0; /*no culling and no fullbright if not specified*/
}

void set_face_color(int mesh_id, int face_id, int red, int green, int blue)
{
  tria* face = fceglob[mesh_id];
  face[face_id].red   = red;
  face[face_id].green = green;
  face[face_id].blue  = blue;
}

void get_face_color(int mesh_id, int face_id, int* red, int* green, int* blue)
{
  tria* face = fceglob[mesh_id];
  *red   = face[face_id].red;
  *green = face[face_id].green;
  *blue  = face[face_id].blue;
}

void set_face_fullbright(int mesh_id, int face_id)
{
  tria* face = fceglob[mesh_id];
  face[face_id].cull = ((face[face_id].cull)&1)+2;
}

void get_face_vertex(int mesh_id, int face_id, int vertex_id, float *x, float *y, float *z)
{
  tria* face = fceglob[mesh_id];
  switch(vertex_id)
  {
    case 1:
      *x = face[face_id].x1;
      *y = face[face_id].y1;
      *z = face[face_id].z1;
      break;
    case 2:
      *x = face[face_id].x2;
      *y = face[face_id].y2;
      *z = face[face_id].z2;
      break;
    case 3:
      *x = face[face_id].x3;
      *y = face[face_id].y3;
      *z = face[face_id].z3;
      break;
    default:
      break;
  }
}

void flip_face(int mesh_id, int face_id)
{
  tria* face = fceglob[mesh_id];
  float tmp;
  tmp = face[face_id].x1; face[face_id].x1 = face[face_id].x2; face[face_id].x2 = tmp;
  tmp = face[face_id].y1; face[face_id].y1 = face[face_id].y2; face[face_id].y2 = tmp;
  tmp = face[face_id].z1; face[face_id].z1 = face[face_id].z2; face[face_id].z2 = tmp;
}

void enable_face_culling(int mesh_id, int face_id)
{
  tria* face = fceglob[mesh_id];
  face[face_id].cull = ((face[face_id].cull)&2)+1;
}

void set_background_color(int red, int green, int blue)
{
  g_backcol.red   = red;
  g_backcol.green = green;
  g_backcol.blue  = blue;
}

void set_ambient_light(float ambient_light)
{
  g_light.ambient = ambient_light;
}

void set_headlight(float headlight)
{
  g_light.headlight = headlight;
}

void set_directional_light(float directional_light, float dx, float dy, float dz)
{
  g_light.directional = directional_light;
  g_light.dx = dx;
  g_light.dy = dy;
  g_light.dz = dz;
}

void set_width_factor(float width_factor)
{
  g_width_factor = width_factor;
}

void set_view_angle(float view_angle)
{
  g_view_angle = view_angle;
}

void set_double_pixel(int double_pixel)
{
  g_double_pixel = double_pixel;
}

/*functie care elimina triunghiurile care sunt in plus*/
int fclip(tria *face,int nrfaces,float zmin,tria *facedisp,float zmax,float tgh,float tgv)
{int i,j,k,l,kmin,invs;
float x[4],y[4],z[4],tmp,tmp2;
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

    for(k=1;k<=2;k++){kmin=k;
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
int vertri(float x1,float y1,float x2,float y2,float x3,float y3,float x,float y)
{float l1,l2,m1,m2,p; /*p - produs vectorial*/
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
float ccplan(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3,float xf,float yf,float zf)
{float a1,b1,c1,a2,b2,c2,a,b,c,d;
float xi,yi,zi;
float dist; /*patratul distantei de la origine pana la intersectia dreptei cu planul*/

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
void findplan(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float *a,float *b,float *c,float *d)
{float a1,b1,c1,a2,b2,c2;

	a1=x1-x2; b1=y1-y2; c1=z1-z2;
	a2=x1-x3; b2=y1-y3; c2=z1-z3;

	*a=b1*c2-b2*c1; *b=a2*c1-a1*c2; *c=a1*b2-a2*b1;
	*d=(*a)*x1+(*b)*y1+(*c)*z1;
	/*ecuatia planului este ax+by+cz=d*/
}


void displaysdl(SDL_Surface *screen,tria *face,int nrfaces,float *distmin,unsigned int width,unsigned int height,float focal,float zfog,float zmax, lightpr* light)
{int i,j,jmin,jmax,xcr,ycr,isp,bitd,red,green,blue,red0,green0,blue0;
Uint8 *ptr;
pixcol pixcb; /*culoarea pixelului curent*/
float ystart,yend,zf,dist;
unsigned long int idx,crf,area;
float tmp,ratio2,ratio3,ratio4,thres;
tripf ftr; /*proiectii varfuri triunghi*/
trilim lim; /*limite triunghi pe axa y*/
float a,b,c,d,izf, /*izf=1/zf - pt. marit viteza; ecuatia planului este ax+by+cz=d*/
      a1,bright,redf,greenf,bluef,
      aizf,bizf,aizfxcr,id,izmax,izfog;

if(g_double_pixel == 1)
{
  width/=2; height/=2; focal/=2;
}

pixcol backcol = g_backcol;
red0=backcol.red; green0=backcol.green; blue0=backcol.blue;

thres=1/(width+height);

area=width*height+1;

izmax=1/zmax; izfog=1/zfog;

const int bits_per_pixel = 32;
bitd=bits_per_pixel/8;

/*desenare imagine*/
for(i=0;i<=(int)area;i++){distmin[i]=izmax;}
zf=-focal;
izf=1/zf;

/*Lock screen*/
if(SDL_MUSTLOCK(screen)){
  if(SDL_LockSurface(screen)<0){
    printf("Can't lock screen: %s\n", SDL_GetError());SDL_Quit();return;}}

for(crf=1;(int)crf<=nrfaces;crf++){

tria* pFace = &face[crf];
findplan(pFace->x1,pFace->y1,pFace->z1,pFace->x2,pFace->y2,pFace->z2,pFace->x3,pFace->y3,pFace->z3,&a,&b,&c,&d);

aizf=a*izf; bizf=b*izf; id=1/d;

if(g_width_factor != 1.0f)
{
  bizf/=g_width_factor;
}

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

    if(g_width_factor != 1.0f)
    {
      ystart*=g_width_factor;
      yend*=g_width_factor;
    }

    jmin=(int)(-yend+(int)(width>>1))+1; jmax=(int)(-ystart+(int)(width>>1));
    if(jmin<0){jmin=0;}
    if(jmax>((int)width-1)){jmax=(int)width-1;}

    if(g_double_pixel == 0)
    {
      ptr=(Uint8 *)screen->pixels+isp+jmin*bitd;
    }
    else
    {
      ptr=(Uint8 *)screen->pixels+2*isp+jmin*bitd*2;
    }

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

    if(g_double_pixel == 0)
    {
      ptr+=bitd;
    }
    else
    {
      ptr+=2*bitd;
    }

  }
}
}

crf=0; ratio2=1/(izmax-izfog);
for(j=0;j<=(int)height-1;j++){
  tmp=1.0-sin(3.1416*j/(float)height); tmp*=50;
  redf=red0-tmp; greenf=green0-tmp; bluef=blue0-tmp;
  backcol.red=(int)redf; backcol.green=(int)greenf; backcol.blue=(int)bluef;

  if(g_double_pixel == 0)
  {
    ptr=(Uint8 *)screen->pixels + j*screen->pitch;
  }
  else
  {
    ptr=(Uint8 *)screen->pixels + 2*j*screen->pitch;
  }
  isp=screen->pitch;

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

  if(g_double_pixel == 0)
  {
    ptr+=bitd;
  }
  else
  {
    ptr[bitd]=ptr[isp]=ptr[isp+bitd]=ptr[0];
    ptr[bitd+1]=ptr[isp+1]=ptr[isp+bitd+1]=ptr[1];
    ptr[bitd+2]=ptr[isp+2]=ptr[isp+bitd+2]=ptr[2];
    ptr+=2*bitd;
  }

}}

if(g_double_pixel == 1)
{
  width*=2; height*=2;
}

/*Unlock screen*/
if(SDL_MUSTLOCK(screen)){SDL_UnlockSurface(screen);}
/* Update display*/
  SDL_UpdateRect();
/*desenat imagine*/
}

/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera*/
void odis(SDL_Surface *screen,sgob *objs,int nob,float zfog,float zmax,sgob *cam)
{int i,j,focal;
unsigned int width,height;
unsigned long int area;
static int sem=0,nrfm=0; /*number of triangles for which memory has been allocated*/
static tria *face,*facedisp; /*triangles and displayed triangles in global system*/
static sgob *obdis; /*displayed objects (copies)*/
static float *distmin; /*Zbuffer for sending to displaysdl()*/
int nrfaces,nrdisp,crf, /*number of triangles and of displayed triangles, current triangle*/
    nobdis=0; /*number of displayed objects*/

lightpr rotlight; /*rotated light parameters*/

float tgh,tgv,zmin;
float x,y,z,ix,iy,iz,jx,jy,jz,kx,ky,kz; /*temporary variables for transformations*/

if(nob==0){free(face); free(facedisp); free(obdis); free(distmin); return;}
/*to free static variables, call odis(0,0,0,0,0,0,0)*/

width=screen->w;
height=screen->h;
focal=(int)(width/(2*tan(g_view_angle*0.008726646)));

area=(width+1)*(height+1);

tgh=(float)width/(2*(float)focal);
tgv=(float)height/(2*(float)focal);

if(g_width_factor != 1.0f)
{
  focal=(int)((float)focal/g_width_factor);
  tgv*=g_width_factor;
}

zmin=1e-3;

if(!sem){
  if(!(distmin=(float *)malloc(area*sizeof(float)))){printf("Out of memory");}
  if(!(face=(tria *)malloc(11*sizeof(tria)))){printf("Out of memory");}
  if(!(facedisp=(tria *)malloc(22*sizeof(tria)))){printf("Out of memory");}
  if(!(obdis=(sgob *)malloc((nob+1)*sizeof(sgob)))){printf("Out of memory");}
  nrfm=1; sem=1;}

for(i=1;i<=nob;i++){
  if(objs[i].xcen-cam->vx[0]-objs[i].radius>(1.74*zmax)){continue;}
  if(objs[i].xcen-cam->vx[0]+objs[i].radius<(-1.74*zmax)){continue;}
  if(objs[i].ycen-cam->vy[0]-objs[i].radius>(1.74*zmax)){continue;}
  if(objs[i].ycen-cam->vy[0]+objs[i].radius<(-1.74*zmax)){continue;}
  if(objs[i].zcen-cam->vz[0]-objs[i].radius>(1.74*zmax)){continue;}
  if(objs[i].zcen-cam->vz[0]+objs[i].radius<(-1.74*zmax)){continue;}
  nobdis++;
  obdis[nobdis]=objs[i];
} /*copied objects*/

ix=cam->vx[1]-cam->vx[0];
jx=cam->vx[2]-cam->vx[0];
kx=cam->vx[3]-cam->vx[0];
  iy=cam->vy[1]-cam->vy[0];
  jy=cam->vy[2]-cam->vy[0];
  ky=cam->vy[3]-cam->vy[0];
    iz=cam->vz[1]-cam->vz[0];
    jz=cam->vz[2]-cam->vz[0];
    kz=cam->vz[3]-cam->vz[0]; /*unit vectors of the local axes of camera in global system*/

for(i=1;i<=nobdis;i++){
  for(j=0;j<=3;j++){
    obdis[i].vx[j]-=cam->vx[0];
    obdis[i].vy[j]-=cam->vy[0];
    obdis[i].vz[j]-=cam->vz[0];
  }
  obdis[i].xcen-=cam->vx[0];
  obdis[i].ycen-=cam->vy[0];
  obdis[i].zcen-=cam->vz[0]; /*translated object*/

  for(j=0;j<=3;j++){
    x=obdis[i].vx[j];
    y=obdis[i].vy[j];
    z=obdis[i].vz[j];
    obdis[i].vx[j]=x*ix+y*iy+z*iz;
    obdis[i].vy[j]=x*jx+y*jy+z*jz;
    obdis[i].vz[j]=x*kx+y*ky+z*kz;
  }
  x=obdis[i].xcen;
  y=obdis[i].ycen;
  z=obdis[i].zcen;
  obdis[i].xcen=x*ix+y*iy+z*iz;
  obdis[i].ycen=x*jx+y*jy+z*jz;
  obdis[i].zcen=x*kx+y*ky+z*kz; /*rotated objects*/
}

x=g_light.dx;
y=g_light.dy;
z=g_light.dz;
rotlight.dx=x*ix+y*iy+z*iz;
rotlight.dy=x*jx+y*jy+z*jz;
rotlight.dz=x*kx+y*ky+z*kz; /*rotated light*/
rotlight.ambient=g_light.ambient;
rotlight.headlight=g_light.headlight;
rotlight.directional=g_light.directional;

nrfaces=0;
crf=0;
for(i=1;i<=nobdis;i++){
  if((obdis[i].ycen-obdis[i].radius)>(obdis[i].zcen+obdis[i].radius)*tgh){continue;}
  if((obdis[i].ycen+obdis[i].radius)<(obdis[i].zcen+obdis[i].radius)*(-tgh)){continue;}
  if((obdis[i].xcen-obdis[i].radius)>(obdis[i].zcen+obdis[i].radius)*tgv){continue;}
  if((obdis[i].xcen+obdis[i].radius)<(obdis[i].zcen+obdis[i].radius)*(-tgv)){continue;}
  if(((obdis[i].zcen-obdis[i].radius)<zmax)&&((obdis[i].zcen+obdis[i].radius)>0)){
    nrfaces+=obdis[i].nfa;
    if(nrfaces>nrfm){
      nrfm=nrfaces;
      if(!(face=(tria *)realloc(face,(nrfm+10)*sizeof(tria)))){printf("Out of memory");}
      if(!(facedisp=(tria *)realloc(facedisp,(2*nrfm+20)*sizeof(tria)))){printf("Out of memory");}
    }

    ix=obdis[i].vx[1]-obdis[i].vx[0];
    jx=obdis[i].vx[2]-obdis[i].vx[0];
    kx=obdis[i].vx[3]-obdis[i].vx[0];
      iy=obdis[i].vy[1]-obdis[i].vy[0];
      jy=obdis[i].vy[2]-obdis[i].vy[0];
      ky=obdis[i].vy[3]-obdis[i].vy[0];
        iz=obdis[i].vz[1]-obdis[i].vz[0];
        jz=obdis[i].vz[2]-obdis[i].vz[0];
        kz=obdis[i].vz[3]-obdis[i].vz[0]; /*unit vectors of the local axes in global system*/

    for(j=1;j<=obdis[i].nfa;j++){
      face[j+crf]=fceglob[obdis[i].otyp][j]; /*added triangles*/
        x=fceglob[obdis[i].otyp][j].x1;
        y=fceglob[obdis[i].otyp][j].y1;
        z=fceglob[obdis[i].otyp][j].z1;
      face[j+crf].x1=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y1=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z1=obdis[i].vz[0]+x*iz+y*jz+z*kz;
        x=fceglob[obdis[i].otyp][j].x2;
        y=fceglob[obdis[i].otyp][j].y2;
        z=fceglob[obdis[i].otyp][j].z2;
      face[j+crf].x2=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y2=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z2=obdis[i].vz[0]+x*iz+y*jz+z*kz;
        x=fceglob[obdis[i].otyp][j].x3;
        y=fceglob[obdis[i].otyp][j].y3;
        z=fceglob[obdis[i].otyp][j].z3;
      face[j+crf].x3=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y3=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z3=obdis[i].vz[0]+x*iz+y*jz+z*kz; /*updated positions of triangles*/
    }
    crf=nrfaces;
  }
}

nrdisp=fclip(face,nrfaces,zmin,facedisp,zmax,tgh,tgv);

displaysdl(screen,facedisp,nrdisp,distmin,width,height,focal,zfog,zmax,&rotlight);

}

void renderer_release()
{
  for(int i=1;i<mesh_count;i++)
  {
    free(fceglob[i]);
  }
  free(fceglob);
}
