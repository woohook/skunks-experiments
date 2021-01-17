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

void renderer3d_initialize(struct _surface* pSurface)
{
  width = surface_get_width(pSurface);
  height = surface_get_height(pSurface);

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

void renderer3d_clear_depth_buffer(struct frustum* pFrustum)
{
  float izmax=1/pFrustum->zmax;
  for(int i=0; i<area; ++i)
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

int clip_triangle(struct renderer_triangle* face, struct frustum* pFrustum, struct renderer_triangle* extra_face)
{
  int k,l,kmin,invs;
  float x[4],y[4],z[4],tmp,tmp2;
  float zmin = pFrustum->zmin;
  float zmax = pFrustum->zmax;
  float tgh  = pFrustum->tgh;
  float tgv  = pFrustum->tgv;

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

//	      face->material_id = face->material_id;
//	      face->cull=face->cull;

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

//	        face->material_id = face->material_id;
//	        face->cull=face->cull;

	        if(invs==(-1)){
	          tmp2=face->x1; face->x1=face->x2; face->x2=tmp2;
                  tmp2=face->y1; face->y1=face->y2; face->y2=tmp2;
                  tmp2=face->z1; face->z1=face->z2; face->z2=tmp2;
	        }
//                displaysdl(pSurface,materials,&facedisp,distmin,focal,pRotLight);
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

//	      extra_face->material_id = face->material_id;
//	      extra_face->cull=face->cull;

	      if(invs==(1)){
	        tmp2=extra_face->x1; extra_face->x1=extra_face->x2; extra_face->x2=tmp2;
                tmp2=extra_face->y1; extra_face->y1=extra_face->y2; extra_face->y2=tmp2;
                tmp2=extra_face->z1; extra_face->z1=extra_face->z2; extra_face->z2=tmp2;
	      }

  return 2;
}

void shade_triangle(struct renderer_triangle* face)
{
  float a, b, /*c*/ d;
  renderer3d_findplan(face->x1,face->y1,face->z1,face->x2,face->y2,face->z2,face->x3,face->y3,face->z3,&a,&b,&c,&d);

  aizf=a*izf; bizf=b*izf; id=1/d;

/*  if(g_width_factor != 1.0f)
  {
    bizf/=g_width_factor;
  }*/
}

void rasterize_triangle(struct renderer_triangle* face, struct _surface* pSurface)
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

    surface_set_current_pixel(pSurface,jmin,i);

    idx=i*width+jmin;

    for(j=jmin;j<=jmax;j++)
    {
      ycr=(int)(width>>1)-j;
      dist=id*(aizfxcr+bizf*ycr+c);
      idx++;
      if(distmin[idx]<dist)
      {
        distmin[idx]=dist;
        surface_set_current_pixel_color(pSurface,face->red,face->green,face->blue);
      }

      surface_advance_current_pixel(pSurface);
    }
  }
}

void renderer3d_render_triangle(struct renderer_triangle* pTriangle, struct frustum* pFrustum, struct _surface* pSurface, struct lights* pLights)
{
  struct renderer_triangle extra_face;
  int triangle_count = clip_triangle(pTriangle, pFrustum, &extra_face);
  if(triangle_count == 0)
  {
    return;
  }

  shade_triangle(pTriangle);

  rasterize_triangle(pTriangle, pSurface);
  if(triangle_count == 2)
  {
    extra_face.red   = pTriangle->red;
    extra_face.green = pTriangle->green;
    extra_face.blue  = pTriangle->blue;
    rasterize_triangle(&extra_face, pSurface);
  }
}
