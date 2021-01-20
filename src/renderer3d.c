/*
Copyright (C) 2007-2011 Victor Matei Petrescu
Copyright (C) 2021 woohook

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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "renderer3d.h"

#include "surface.h"

typedef struct _tripf
{
  float x1; float y1;
  float x2; float y2;
  float x3; float y3;
} tripf;

typedef struct _trilim
{
  int imin;
  int imax;
} trilim;

int width, height;
unsigned long int area = 0;

float focal = 0;
float zf = 0;
float izf = 0;
float aizf = 0;
float bizf = 0;
float id = 0;
float c;
float* distmin = 0;
float g_zfog=80;  // fog distance (m)

struct _surface* g_surface = 0;
struct frustum* g_frustum = 0;
struct lights* g_lights = 0;

void renderer3d_initialize(struct _surface* pSurface, struct frustum* pFrustum, struct lights* pLights)
{
  g_surface = pSurface;
  g_frustum = pFrustum;
  g_lights  = pLights;

  width = surface_get_width(g_surface);
  height = surface_get_height(g_surface);

/*  if(g_double_pixel == 1)
  {
    width/=2; height/=2; focal/=2;
  }*/

  float view_angle = 90.0f;

  focal=(int)(width/(2*tan(view_angle*0.008726646)));
  zf=-focal;
  izf=1/zf;

  area=(width+1)*(height+1);
  if(!(distmin=(float *)malloc(area*sizeof(float)))){printf("Out of memory");}
}

void clear_depth_buffer()
{
  float izmax=1/g_frustum->zmax;
  for(unsigned long int i=0; i<area; ++i)
  {
    distmin[i] = izmax;
  }
}

void renderer3d_findplan(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float *a,float *b,float *c,float *d)
{float a1,b1,c1,a2,b2,c2;

	a1=x1-x2; b1=y1-y2; c1=z1-z2;
	a2=x1-x3; b2=y1-y3; c2=z1-z3;

	*a=b1*c2-b2*c1; *b=a2*c1-a1*c2; *c=a1*b2-a2*b1;
	*d=(*a)*x1+(*b)*y1+(*c)*z1;
	/*ecuatia planului este ax+by+cz=d*/
}

int clip_triangle(struct renderer_triangle* face, struct renderer_triangle* extra_face)
{
  int k,l,kmin,invs;
  float x[4],y[4],z[4],tmp,tmp2;
  float zmin = g_frustum->zmin;
  float zmax = g_frustum->zmax;
  float tgh  = g_frustum->tgh;
  float tgv  = g_frustum->tgv;

  if((face->z1<=zmin)&&(face->z2<=zmin)&&(face->z3<=zmin)){return 0;}
  if((face->z1>=zmax)&&(face->z2>=zmax)&&(face->z3>=zmax)){return 0;}
    if((face->x1>tgv*face->z1)&&(face->x2>tgv*face->z2)&&(face->x3>tgv*face->z3)){return 0;}
    if((face->y1>tgh*face->z1)&&(face->y2>tgh*face->z2)&&(face->y3>tgh*face->z3)){return 0;}
  if((face->x1 < -tgv*face->z1)&&(face->x2 < -tgv*face->z2)&&(face->x3 < -tgv*face->z3)){return 0;}
  if((face->y1 < -tgh*face->z1)&&(face->y2 < -tgh*face->z2)&&(face->y3 < -tgh*face->z3)){return 0;}

  if((face->z1>zmin)&&(face->z2>zmin)&&(face->z3>zmin))
  {
    return 1;
  }

    x[1]=face->x1;x[2]=face->x2;x[3]=face->x3;
    y[1]=face->y1;y[2]=face->y2;y[3]=face->y3;
    z[1]=face->z1;z[2]=face->z2;z[3]=face->z3;

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
      tmp=(zmin-z[1])/(z[3]-z[1]);
        face->x1=tmp*(x[3]-x[1])+x[1];
        face->y1=tmp*(y[3]-y[1])+y[1];
      tmp=(zmin-z[2])/(z[3]-z[2]);
        face->x2=tmp*(x[3]-x[2])+x[2];
        face->y2=tmp*(y[3]-y[2])+y[2];
	  face->x3=x[3];
	  face->y3=y[3];
	    face->z1=face->z2=zmin;
	    face->z3=z[3];

	      if(invs==(-1)){
	        tmp2=face->x1; face->x1=face->x2; face->x2=tmp2;
                tmp2=face->y1; face->y1=face->y2; face->y2=tmp2;
                tmp2=face->z1; face->z1=face->z2; face->z2=tmp2;
	      }
    return 1;
  }

      tmp=(zmin-z[1])/(z[2]-z[1]);
        face->x1=tmp*(x[2]-x[1])+x[1];
	face->y1=tmp*(y[2]-y[1])+y[1];
	  face->x2=x[2];
	  face->y2=y[2];
	    face->x3=x[3];
	    face->y3=y[3];
	      face->z1=zmin;
	      face->z2=z[2];
	      face->z3=z[3];

	        if(invs==(-1)){
	          tmp2=face->x1; face->x1=face->x2; face->x2=tmp2;
                  tmp2=face->y1; face->y1=face->y2; face->y2=tmp2;
                  tmp2=face->z1; face->z1=face->z2; face->z2=tmp2;
	        }

        extra_face->x1=tmp*(x[2]-x[1])+x[1];
	extra_face->y1=tmp*(y[2]-y[1])+y[1];
      tmp=(zmin-z[1])/(z[3]-z[1]);
        extra_face->x2=tmp*(x[3]-x[1])+x[1];
	extra_face->y2=tmp*(y[3]-y[1])+y[1];
	  extra_face->x3=x[3];
	  extra_face->y3=y[3];
	    extra_face->z1=zmin;
            extra_face->z2=zmin;
	    extra_face->z3=z[3];

	      extra_face->cull=face->cull;

	      if(invs==(1)){
	        tmp2=extra_face->x1; extra_face->x1=extra_face->x2; extra_face->x2=tmp2;
                tmp2=extra_face->y1; extra_face->y1=extra_face->y2; extra_face->y2=tmp2;
                tmp2=extra_face->z1; extra_face->z1=extra_face->z2; extra_face->z2=tmp2;
	      }

  return 2;
}

int shade_triangle(struct renderer_triangle* face)
{
int red,green,blue;
float zf,dist;
float tmp;
float a,b,d,izf, //izf=1/zf - pt. marit viteza; ecuatia planului este ax+by+cz=d
      a1,bright;

/*if(g_double_pixel == 1)
{
  width/=2; height/=2; focal/=2;
}*/

zf=-focal;
izf=1/zf;

renderer3d_findplan(face->x1,face->y1,face->z1,face->x2,face->y2,face->z2,face->x3,face->y3,face->z3,&a,&b,&c,&d);

aizf=a*izf; bizf=b*izf; id=1/d;

/*if(g_width_factor != 1.0f)
{
  bizf/=g_width_factor;
}*/

//start lighting and backface culling
tmp=a*face->x1+b*face->y1+c*face->z1;
//dot product used for lighting and backface culling
if(face->cull==1){ // backface culling
  if(tmp<0){return -1;}
}

if(face->fullbright==1){
  a1=1;
}
else
{
  dist=sqrt(a*a+b*b+c*c);
  a1=(g_lights->directional_light_x*a+g_lights->directional_light_y*b+g_lights->directional_light_z*c)/dist;
  if(tmp>=0){
    if(a1<0){a1=0;}
  }else{
    if(a1<0){a1=-a1;}else{a1=0;}
  }
  a1=g_lights->ambient_light_intensity+fabs(g_lights->head_light_intensity*c/dist)+g_lights->directional_light_intensity*a1;
}

bright=a1; if(bright<0){bright=0;}

red=(int)(bright*face->red); green=(int)(bright*face->green); blue=(int)(bright*face->blue);
if(red>255){red=255;}
if(green>255){green=255;}
if(blue>255){blue=255;}

face->red=red;
face->green=green;
face->blue=blue;

  return 0;
}

void rasterize_triangle(struct renderer_triangle* face)
{
  float aizfxcr;
  float tmp;
  float ratio2, ratio3, ratio4;
  float thres = 1/(width+height);
  tripf ftr;    // proiectii varfuri triunghi
  trilim lim;    // limite triunghi pe axa y
  float ystart, yend;
  float dist;
  int i, j, jmin, jmax, xcr, ycr;
  unsigned long int idx;
//  struct material* pMaterial = 0;

  // find trangle limits
  ftr.x1=face->x1*zf/face->z1;
  ftr.x2=face->x2*zf/face->z2;
  ftr.x3=face->x3*zf/face->z3;

  ftr.y1=face->y1*zf/face->z1;
  ftr.y2=face->y2*zf/face->z2;
  ftr.y3=face->y3*zf/face->z3;

  if(ftr.x2>ftr.x3){tmp=ftr.y2;ftr.y2=ftr.y3;ftr.y3=tmp; tmp=ftr.x2;ftr.x2=ftr.x3;ftr.x3=tmp;}
  if(ftr.x1>ftr.x2){{tmp=ftr.y1;ftr.y1=ftr.y2;ftr.y2=tmp; tmp=ftr.x1;ftr.x1=ftr.x2;ftr.x2=tmp;}
  if(ftr.x2>ftr.x3){tmp=ftr.y2;ftr.y2=ftr.y3;ftr.y3=tmp; tmp=ftr.x2;ftr.x2=ftr.x3;ftr.x3=tmp;}}
  // ordered triangle vertices

  lim.imin=(int)((int)(height>>1)+ftr.x1+1);
  lim.imax=(int)((int)(height>>1)+ftr.x3);

  if(lim.imin<0){lim.imin=0;}
  if(lim.imax>((int)height-1)){lim.imax=(int)height-1;}
  // found triangle limits

  if(fabs(ftr.x2-ftr.x1)>thres){ratio2=(ftr.y2-ftr.y1)/(ftr.x2-ftr.x1);}else{ratio2=1e5;}
  if(fabs(ftr.x3-ftr.x1)>thres){ratio3=(ftr.y3-ftr.y1)/(ftr.x3-ftr.x1);}else{ratio3=1e5;}
  if(fabs(ftr.x3-ftr.x2)>thres){ratio4=(ftr.y3-ftr.y2)/(ftr.x3-ftr.x2);}else{ratio4=1e5;}

  for(i=lim.imin;i<=lim.imax;i++)
  {
    xcr=-(int)(height>>1)+i;

    aizfxcr=aizf*xcr;

    if(xcr<ftr.x2)
    {
      ystart=(ratio2*(xcr-ftr.x1)+ftr.y1);
      yend=(ratio3*(xcr-ftr.x1)+ftr.y1);
    }
    else
    {
      ystart=(ratio4*(xcr-ftr.x2)+ftr.y2);
      yend=(ratio3*(xcr-ftr.x1)+ftr.y1);
    }

    if(yend<ystart)
    {
      tmp=ystart; ystart=yend; yend=tmp;
    }

/*    if(g_width_factor != 1.0f)
    {
      ystart*=g_width_factor;
      yend*=g_width_factor;
    }*/

    jmin=(int)(-yend+(int)(width>>1))+1;
    jmax=(int)(-ystart+(int)(width>>1));
    if(jmin<0)
    {
      jmin=0;
    }
    if(jmax>((int)width-1))
    {
      jmax=(int)width-1;
    }

    surface_set_current_pixel(g_surface,jmin,i);

    idx=i*width+jmin;

    for(j=jmin;j<=jmax;j++)
    {
      ycr=(int)(width>>1)-j;
      dist=id*(aizfxcr+bizf*ycr+c);
      idx++;
      if(distmin[idx]<dist)
      {
        distmin[idx]=dist;
        surface_set_current_pixel_color(g_surface,face->red,face->green,face->blue);
      }

      surface_advance_current_pixel(g_surface);
    }
  }
}

void renderer3d_render_triangle(struct renderer_triangle* pTriangle)
{
  struct renderer_triangle extra_face;
  int triangle_count = clip_triangle(pTriangle, &extra_face);
  if(triangle_count == 0)
  {
    return;
  }

  if(shade_triangle(pTriangle) != 0)
  {
    return;
  }

  rasterize_triangle(pTriangle);
  if(triangle_count == 2)
  {
    extra_face.red   = pTriangle->red;
    extra_face.green = pTriangle->green;
    extra_face.blue  = pTriangle->blue;
    rasterize_triangle(&extra_face);
  }
}

void renderer3d_start_frame()
{
  clear_depth_buffer();
}

void renderer3d_finish_frame(int back_red, int back_green, int back_blue)
{
  int i,j;
  unsigned long int crf=0;
  float izmax=1/g_frustum->zmax;
  float izfog=1/g_zfog;
  float ratio2=1/(izmax-izfog);
  int red0   = back_red;
  int green0 = back_green;
  int blue0  = back_blue;

  for(j=0;j<=(int)height-1;j++)
  {
    float tmp=1.0-sin(3.1416*j/(float)height); tmp*=50;
    float redf=red0-tmp;
    float greenf=green0-tmp;
    float bluef=blue0-tmp;
    back_red=(int)redf; back_green=(int)greenf; back_blue=(int)bluef;

    surface_set_current_pixel(g_surface,0,j);

    for(i=0;i<=(int)width-1;i++)
    {
      if(distmin[++crf]==izmax)
      {
        surface_set_current_pixel_color(g_surface,back_red,back_green,back_blue);
      }
      else
      {
        if(distmin[crf]<izfog)
        {
          int pixel_red, pixel_green, pixel_blue;
          surface_get_current_pixel_color(g_surface,&pixel_red,&pixel_green,&pixel_blue);

          tmp=(distmin[crf]-izfog)*ratio2;
          //pixel_red=(int)(pixel_red*(1-tmp));
          //pixel_green=(int)(pixel_green*(1-tmp));
          //pixel_blue=(int)(pixel_blue*(1-tmp));
          pixel_red+=(int)(tmp*(back_red-pixel_red));
          pixel_green+=(int)(tmp*(back_green-pixel_green));
          pixel_blue+=(int)(tmp*(back_blue-pixel_blue));

          surface_set_current_pixel_color(g_surface,pixel_red,pixel_green,pixel_blue);
        }
      }

      surface_advance_current_pixel(g_surface);
    }
  }
}
