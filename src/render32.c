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

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "render32.h"
#include "surface.h"
#include "trans.h"
#include "list.h"

#define DOUBLEPIX 0  // 1 - large pixels; 0 - normal pixels

#define FOV 90  // horizontal viewing angle (in degrees)

#define ASPCOR 0  // 1 - aspect correction; 0 - no aspect correction (WIDTHFACTOR ignored)

#define WIDTHFACTOR 0.78125 // 0.78125 recommended for monitor with 1024x600 resolution, set at 800x600 (if the video driver doesn't support 1024x600)

typedef struct _pixcol
{int red;int green;int blue; /*culoarea pixelului*/
} pixcol;

struct material
{
  int red; int green; int blue;
  int redd; int greend; int blued;
  unsigned char fullbright; // 1 = fullbright; 0 = usual
};

typedef struct _tria
{float x1; float y1;float z1;
float x2; float y2;float z2;
float x3; float y3;float z3;
unsigned char cull; // 1 = cull face; 0 = no culling
int material_id;
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

typedef struct _mesh
{
  struct _list* faces;
  float xcen, ycen, zcen, radius;
  struct _list* materials;
} mesh;

typedef struct mesh_instance
{
  struct _mesh* pMesh;
  matrix* transform;
} mesh_instance;

struct _list* g_meshes = 0;

struct _list* g_instances = 0;

pixcol g_backcol = {0,0,0};
lightpr g_light = {0,0,0,0,0,0};
float g_width_factor = 1.0f;
float g_view_angle = 90.0f;
int g_double_pixel = 0;
float zfog=80;  // fog distance (m)
float zmin=1e-3;
float zmax=120; // visibility (m)

struct _mesh* create_mesh()
{
  if(g_meshes==0)
  {
    g_meshes = list_create();
  }

  mesh* pMesh = (mesh*)malloc(sizeof(mesh));
  if(pMesh==0)
  {
    printf("Out of memory");
    exit(-1);
  }
  else
  {
    pMesh->faces = list_create();
    pMesh->materials = list_create();
    struct material* pMaterial = (struct material*)malloc(sizeof(struct material));
    pMaterial->red = 255;
    pMaterial->green = 200;
    pMaterial->blue = 0;
    pMaterial->fullbright = 0;
    list_add_value(pMesh->materials, pMaterial);
    list_add_value(g_meshes, pMesh);
  }

  return pMesh;
}

void create_mesh_instance(struct _mesh* pMesh, matrix* transform)
{
  if(g_instances==0)
  {
    g_instances = list_create();
  }

  mesh_instance* pInstance = (mesh_instance*)malloc(sizeof(mesh_instance));
  if(pInstance == 0)
  {
    printf("Out of memory");
    exit(-1);
  }
  else
  {
    pInstance->pMesh = pMesh;
    pInstance->transform = transform;
    list_add_value(g_instances, pInstance);
  }
}

// function which finds the center and size of the current mesh
void complete_mesh(struct _mesh* pMesh)
{
  float xmin,xmax,ymin,ymax,zmin,zmax,lenx,leny,lenz;
  float x1,y1,z1, x2,y2,z2, x3,y3,z3;

  struct _list_item* faceNode = list_get_first(pMesh->faces);
  tria* face = list_item_get_value(faceNode);
  x1 = face->x1; y1 = face->y1; z1 = face->z1;
  x2 = face->x2; y2 = face->y2; z2 = face->z2;
  x3 = face->x3; y3 = face->y3; z3 = face->z3;
  xmin=xmax=x1;
  ymin=ymax=y1;
  zmin=zmax=z1;

  while(faceNode != 0)
  {
    face     = list_item_get_value(faceNode);

    x1 = face->x1; y1 = face->y1; z1 = face->z1;
    x2 = face->x2; y2 = face->y2; z2 = face->z2;
    x3 = face->x3; y3 = face->y3; z3 = face->z3;

    if(xmin>x1){xmin=x1;}
    if(xmin>x2){xmin=x2;}
    if(xmin>x3){xmin=x3;}
    if(xmax<x1){xmax=x1;}
    if(xmax<x2){xmax=x2;}
    if(xmax<x3){xmax=x3;}

    if(ymin>y1){ymin=y1;}
    if(ymin>y2){ymin=y2;}
    if(ymin>y3){ymin=y3;}
    if(ymax<y1){ymax=y1;}
    if(ymax<y2){ymax=y2;}
    if(ymax<y3){ymax=y3;}

    if(zmin>z1){zmin=z1;}
    if(zmin>z2){zmin=z2;}
    if(zmin>z3){zmin=z3;}
    if(zmax<z1){zmax=z1;}
    if(zmax<z2){zmax=z2;}
    if(zmax<z3){zmax=z3;}

    faceNode = list_item_get_next(faceNode);
  }

  lenx=xmax-xmin;
  leny=ymax-ymin;
  lenz=zmax-zmin;

  pMesh->xcen=(xmax+xmin)/2;
  pMesh->ycen=(ymax+ymin)/2;
  pMesh->zcen=(zmax+zmin)/2;
  pMesh->radius=sqrt(lenx*lenx+leny*leny+lenz*lenz)/2;
}

void add_face(struct _mesh* pMesh, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
  tria* face = malloc(sizeof(tria));

  face->x1 = x1; face->y1 = y1; face->z1 = z1;
  face->x2 = x2; face->y2 = y2; face->z2 = z2;
  face->x3 = x3; face->y3 = y3; face->z3 = z3;
  face->material_id = 0;

  face->cull = 0; // no culling

  list_add_value(pMesh->faces, face);
}

void add_material(struct _mesh* pMesh, int red, int green, int blue, int fullbright)
{
  struct material* pMaterial = (struct material*)malloc(sizeof(struct material));
  pMaterial->red   = red;
  pMaterial->green = green;
  pMaterial->blue  = blue;
  pMaterial->fullbright = fullbright;
  list_add_value(pMesh->materials, pMaterial);
}

void set_face_material(struct _mesh* pMesh, int face_id, int material_id)
{
  tria* face = list_get_value(pMesh->faces, face_id-1);
  face->material_id = material_id;
}

void get_face_vertex(struct _mesh* pMesh, int face_id, int vertex_id, float *x, float *y, float *z)
{
  tria* face = list_get_value(pMesh->faces, face_id-1);
  switch(vertex_id)
  {
    case 1:
      *x = face->x1;
      *y = face->y1;
      *z = face->z1;
      break;
    case 2:
      *x = face->x2;
      *y = face->y2;
      *z = face->z2;
      break;
    case 3:
      *x = face->x3;
      *y = face->y3;
      *z = face->z3;
      break;
    default:
      break;
  }
}

void flip_face(struct _mesh* pMesh, int face_id)
{
  tria* face = list_get_value(pMesh->faces, face_id-1);
  float tmp;
  tmp = face->x1; face->x1 = face->x2; face->x2 = tmp;
  tmp = face->y1; face->y1 = face->y2; face->y2 = tmp;
  tmp = face->z1; face->z1 = face->z2; face->z2 = tmp;
}

void enable_face_culling(struct _mesh* pMesh, int face_id)
{
  tria* face = list_get_value(pMesh->faces, face_id-1);
  face->cull = 1;
}

int get_face_count(struct _mesh* pMesh)
{
  return list_get_size(pMesh->faces);
}

void adjust_material_colors(float fred, float fgreen, float fblue)
{
  struct _list_item* pMeshItem = list_get_first(g_meshes);
  while(pMeshItem != 0)
  {
    struct _mesh* pMesh = list_item_get_value(pMeshItem);

    struct _list_item* pMaterialItem = list_get_first(pMesh->materials);
    while(pMaterialItem != 0)
    {
      struct material* pMaterial = list_item_get_value(pMaterialItem);

      pMaterial->red *= fred;
      if(pMaterial->red > 255)
      {
        pMaterial->red = 255;
      }

      pMaterial->green *= fgreen;
      if(pMaterial->green > 255)
      {
        pMaterial->green = 255;
      }

      pMaterial->blue *= fblue;
      if(pMaterial->blue > 255)
      {
        pMaterial->blue = 255;
      }

      pMaterialItem = list_item_get_next(pMaterialItem);
    }

    pMeshItem = list_item_get_next(pMeshItem);
  }
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

void displaysdl(struct _surface* pSurface,struct _list* materials,tria *face,float *distmin,float focal, lightpr* light);

/*functie care elimina triunghiurile care sunt in plus*/
void fclip(struct _surface* pSurface, struct _list* materials, tria *face,float tgh,float tgv, float* distmin, int focal, lightpr* pRotLight)
{int j,k,l,kmin,invs;
float x[4],y[4],z[4],tmp,tmp2;
tria facedisp;

  if((face->z1<=zmin)&&(face->z2<=zmin)&&(face->z3<=zmin)){return;}
  if((face->z1>=zmax)&&(face->z2>=zmax)&&(face->z3>=zmax)){return;}
    if((face->x1>tgv*face->z1)&&(face->x2>tgv*face->z2)&&(face->x3>tgv*face->z3)){return;}
    if((face->y1>tgh*face->z1)&&(face->y2>tgh*face->z2)&&(face->y3>tgh*face->z3)){return;}
  if((face->x1 < -tgv*face->z1)&&(face->x2 < -tgv*face->z2)&&(face->x3 < -tgv*face->z3)){return;}
  if((face->y1 < -tgh*face->z1)&&(face->y2 < -tgh*face->z2)&&(face->y3 < -tgh*face->z3)){return;}

  if((face->z1>zmin)&&(face->z2>zmin)&&(face->z3>zmin))
  {
    j++;facedisp=*face;
    displaysdl(pSurface,materials,&facedisp,distmin,focal,pRotLight);
  }
  else{
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
    j++;
      tmp=(zmin-z[1])/(z[3]-z[1]);
        facedisp.x1=tmp*(x[3]-x[1])+x[1];
        facedisp.y1=tmp*(y[3]-y[1])+y[1];
      tmp=(zmin-z[2])/(z[3]-z[2]);
        facedisp.x2=tmp*(x[3]-x[2])+x[2];
        facedisp.y2=tmp*(y[3]-y[2])+y[2];
	  facedisp.x3=x[3];
	  facedisp.y3=y[3];
	    facedisp.z1=facedisp.z2=zmin;
	    facedisp.z3=z[3];
	      facedisp.material_id = face->material_id;

	      facedisp.cull=face->cull;
	      if(invs==(-1)){
	        tmp2=facedisp.x1; facedisp.x1=facedisp.x2; facedisp.x2=tmp2;
                tmp2=facedisp.y1; facedisp.y1=facedisp.y2; facedisp.y2=tmp2;
                tmp2=facedisp.z1; facedisp.z1=facedisp.z2; facedisp.z2=tmp2;
	      }
              displaysdl(pSurface,materials,&facedisp,distmin,focal,pRotLight);
  }else{
    j++;
      tmp=(zmin-z[1])/(z[2]-z[1]);
        facedisp.x1=tmp*(x[2]-x[1])+x[1];
	facedisp.y1=tmp*(y[2]-y[1])+y[1];
	  facedisp.x2=x[2];
	  facedisp.y2=y[2];
	    facedisp.x3=x[3];
	    facedisp.y3=y[3];
	      facedisp.z1=zmin;
	      facedisp.z2=z[2];
	      facedisp.z3=z[3];
	        facedisp.material_id = face->material_id;

	        facedisp.cull=face->cull;
	        if(invs==(-1)){
	          tmp2=facedisp.x1; facedisp.x1=facedisp.x2; facedisp.x2=tmp2;
                  tmp2=facedisp.y1; facedisp.y1=facedisp.y2; facedisp.y2=tmp2;
                  tmp2=facedisp.z1; facedisp.z1=facedisp.z2; facedisp.z2=tmp2;
	        }
                displaysdl(pSurface,materials,&facedisp,distmin,focal,pRotLight);
    j++;
        facedisp.x1=tmp*(x[2]-x[1])+x[1];
	facedisp.y1=tmp*(y[2]-y[1])+y[1];
      tmp=(zmin-z[1])/(z[3]-z[1]);
        facedisp.x2=tmp*(x[3]-x[1])+x[1];
	facedisp.y2=tmp*(y[3]-y[1])+y[1];
	  facedisp.x3=x[3];
	  facedisp.y3=y[3];
	    facedisp.z1=facedisp.z2=zmin;
	    facedisp.z3=z[3];
	      facedisp.material_id = face->material_id;

	      facedisp.cull=face->cull;
	      if(invs==(1)){
	        tmp2=facedisp.x1; facedisp.x1=facedisp.x2; facedisp.x2=tmp2;
                tmp2=facedisp.y1; facedisp.y1=facedisp.y2; facedisp.y2=tmp2;
                tmp2=facedisp.z1; facedisp.z1=facedisp.z2; facedisp.z2=tmp2;
	      }
              displaysdl(pSurface,materials,&facedisp,distmin,focal,pRotLight);
  }
  }
}


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

void clear_depth_buffer(float* depthbuffer, unsigned int width, unsigned int height)
{
  int i;
  unsigned long int area = width*height+1;
  float izmax=1/zmax;
  for(i=0;i<=(int)area;i++)
  {
    depthbuffer[i]=izmax;
  }
}

void finish_frame(struct _surface* pSurface, float* depthbuffer)
{
  int i,j;
  unsigned long int crf=0;
  float izmax=1/zmax;
  float izfog=1/zfog;
  float ratio2=1/(izmax-izfog);
  pixcol backcol = g_backcol;
  int red0=backcol.red;
  int green0=backcol.green;
  int blue0=backcol.blue;
  pixcol pixcb;
  int width = surface_get_width(pSurface);
  int height = surface_get_height(pSurface);

  for(j=0;j<=(int)height-1;j++)
  {
    float tmp=1.0-sin(3.1416*j/(float)height); tmp*=50;
    float redf=red0-tmp;
    float greenf=green0-tmp;
    float bluef=blue0-tmp;
    backcol.red=(int)redf; backcol.green=(int)greenf; backcol.blue=(int)bluef;

    surface_set_current_pixel(pSurface,0,j);

    for(i=0;i<=(int)width-1;i++)
    {
      if(depthbuffer[++crf]==izmax)
      {
        surface_set_current_pixel_color(pSurface,backcol.red,backcol.green,backcol.blue);
      }
      else
      {
        if(depthbuffer[crf]<izfog)
        {
          surface_get_current_pixel_color(pSurface,&pixcb.red,&pixcb.green,&pixcb.blue);

          tmp=(depthbuffer[crf]-izfog)*ratio2;
          //pixcb.red=(int)(pixcb.red*(1-tmp));
          //pixcb.green=(int)(pixcb.green*(1-tmp));
          //pixcb.blue=(int)(pixcb.blue*(1-tmp));
          pixcb.red+=(int)(tmp*(backcol.red-pixcb.red));
          pixcb.green+=(int)(tmp*(backcol.green-pixcb.green));
          pixcb.blue+=(int)(tmp*(backcol.blue-pixcb.blue));

          surface_set_current_pixel_color(pSurface,pixcb.red,pixcb.green,pixcb.blue);
        }
      }

      surface_advance_current_pixel(pSurface);
    }
  }
}

void render_triangle(struct _surface* pSurface, float *distmin, struct _list* materials, tria *face, float zf, float aizf, float bizf, float id, float c)
{
  float aizfxcr;
  float tmp;
  float ratio2, ratio3, ratio4;
  int width = surface_get_width(pSurface);
  int height = surface_get_height(pSurface);
  float thres = 1/(width+height);
  tripf ftr;    // proiectii varfuri triunghi
  trilim lim;    // limite triunghi pe axa y
  float ystart, yend;
  float dist;
  int i, j, jmin, jmax, xcr, ycr;
  unsigned long int idx;
  struct material* pMaterial = 0;

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

    if(g_width_factor != 1.0f)
    {
      ystart*=g_width_factor;
      yend*=g_width_factor;
    }

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
        pMaterial = list_get_value(materials, face->material_id);
        surface_set_current_pixel_color(pSurface,pMaterial->redd,pMaterial->greend,pMaterial->blued);
      }

      surface_advance_current_pixel(pSurface);
    }
  }
}

void displaysdl(struct _surface* pSurface, struct _list* materials,tria *face,float *distmin,float focal, lightpr* light)
{
int red,green,blue;
float zf,dist;
float tmp;
float a,b,c,d,izf, /*izf=1/zf - pt. marit viteza; ecuatia planului este ax+by+cz=d*/
      a1,bright,
      aizf,bizf,id;
unsigned int width = surface_get_width(pSurface);
unsigned int height = surface_get_height(pSurface);
struct material* pMaterial = 0;

if(g_double_pixel == 1)
{
  width/=2; height/=2; focal/=2;
}

zf=-focal;
izf=1/zf;

findplan(face->x1,face->y1,face->z1,face->x2,face->y2,face->z2,face->x3,face->y3,face->z3,&a,&b,&c,&d);

aizf=a*izf; bizf=b*izf; id=1/d;

if(g_width_factor != 1.0f)
{
  bizf/=g_width_factor;
}

pMaterial = list_get_value(materials,face->material_id);

/*start lighting and backface culling*/
tmp=a*face->x1+b*face->y1+c*face->z1;
/*dot product used for lighting and backface culling*/
if(face->cull==1){ // backface culling
  if(tmp<0){return;}
}

if(pMaterial->fullbright==1){
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

red=(int)(bright*pMaterial->red); green=(int)(bright*pMaterial->green); blue=(int)(bright*pMaterial->blue);
if(red>255){red=255;}
if(green>255){green=255;}
if(blue>255){blue=255;}

pMaterial->redd=red;
pMaterial->greend=green;
pMaterial->blued=blue;
/*finished lighting and backface culling*/

  render_triangle(pSurface, distmin, materials, face, zf, aizf, bizf, id, c);
}

/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera*/
void odis(struct _surface* pSurface, struct _matrix* view_transform)
{int j,focal;
unsigned int width,height;
unsigned long int area;
static int sem=0; /*number of triangles for which memory has been allocated*/
static tria face; /*triangles and displayed triangles in global system*/
static matrix transform; /* current object's world transformation */
static float *distmin; /*Zbuffer for sending to displaysdl()*/

lightpr rotlight; /*rotated light parameters*/

float tgh,tgv;
float x,y,z,ix,iy,iz,jx,jy,jz,kx,ky,kz; /*temporary variables for transformations*/
float xcen,ycen,zcen,radius;

if(pSurface==0){free(distmin); return;}
/*to free static variables, call odis(0,0,0,0,0,0,0)*/

width=surface_get_width(pSurface);
height=surface_get_height(pSurface);
focal=(int)(width/(2*tan(g_view_angle*0.008726646)));

area=(width+1)*(height+1);

tgh=(float)width/(2*(float)focal);
tgv=(float)height/(2*(float)focal);

if(g_width_factor != 1.0f)
{
  focal=(int)((float)focal/g_width_factor);
  tgv*=g_width_factor;
}

if(!sem){
  if(!(distmin=(float *)malloc(area*sizeof(float)))){printf("Out of memory");}
  sem=1;}

clear_depth_buffer(distmin,width,height);

surface_begin_rendering(pSurface);

ix=view_transform->vx[1]-view_transform->vx[0];
jx=view_transform->vx[2]-view_transform->vx[0];
kx=view_transform->vx[3]-view_transform->vx[0];
  iy=view_transform->vy[1]-view_transform->vy[0];
  jy=view_transform->vy[2]-view_transform->vy[0];
  ky=view_transform->vy[3]-view_transform->vy[0];
    iz=view_transform->vz[1]-view_transform->vz[0];
    jz=view_transform->vz[2]-view_transform->vz[0];
    kz=view_transform->vz[3]-view_transform->vz[0]; /*unit vectors of the local axes of camera in global system*/

x=g_light.dx;
y=g_light.dy;
z=g_light.dz;
rotlight.dx=x*ix+y*iy+z*iz;
rotlight.dy=x*jx+y*jy+z*jz;
rotlight.dz=x*kx+y*ky+z*kz; /*rotated light*/
rotlight.ambient=g_light.ambient;
rotlight.headlight=g_light.headlight;
rotlight.directional=g_light.directional;

struct _list_item* instanceNode = list_get_first(g_instances);
while(instanceNode != 0){
  mesh_instance* pInstance = list_item_get_value(instanceNode);
  mesh* pMesh = pInstance->pMesh;
  transform = *pInstance->transform;
  instanceNode = list_item_get_next(instanceNode);

    float fix=transform.vx[1]-transform.vx[0];
    float fjx=transform.vx[2]-transform.vx[0];
    float fkx=transform.vx[3]-transform.vx[0];
      float fiy=transform.vy[1]-transform.vy[0];
      float fjy=transform.vy[2]-transform.vy[0];
      float fky=transform.vy[3]-transform.vy[0];
        float fiz=transform.vz[1]-transform.vz[0];
        float fjz=transform.vz[2]-transform.vz[0];
        float fkz=transform.vz[3]-transform.vz[0]; /*unit vectors of the local axes in global system*/

  xcen=pMesh->xcen;
  ycen=pMesh->ycen;
  zcen=pMesh->zcen;
  radius=pMesh->radius;
  x=xcen;
  y=ycen;
  z=zcen;
  xcen=transform.vx[0]-xcen+x*fix+y*fjx+z*fkx;
  ycen=transform.vy[0]-ycen+x*fiy+y*fjy+z*fky;
  zcen=transform.vz[0]-zcen+x*fiz+y*fjz+z*fkz;

  if(xcen-view_transform->vx[0]-radius>(1.74*zmax)){continue;}
  if(xcen-view_transform->vx[0]+radius<(-1.74*zmax)){continue;}
  if(ycen-view_transform->vy[0]-radius>(1.74*zmax)){continue;}
  if(ycen-view_transform->vy[0]+radius<(-1.74*zmax)){continue;}
  if(zcen-view_transform->vz[0]-radius>(1.74*zmax)){continue;}
  if(zcen-view_transform->vz[0]+radius<(-1.74*zmax)){continue;}

  for(j=0;j<=3;j++){
    transform.vx[j]-=view_transform->vx[0];
    transform.vy[j]-=view_transform->vy[0];
    transform.vz[j]-=view_transform->vz[0];
  }
  xcen-=view_transform->vx[0];
  ycen-=view_transform->vy[0];
  zcen-=view_transform->vz[0]; /*translated object*/

  for(j=0;j<=3;j++){
    x=transform.vx[j];
    y=transform.vy[j];
    z=transform.vz[j];
    transform.vx[j]=x*ix+y*iy+z*iz;
    transform.vy[j]=x*jx+y*jy+z*jz;
    transform.vz[j]=x*kx+y*ky+z*kz;
  }
  x=xcen;
  y=ycen;
  z=zcen;
  xcen=x*ix+y*iy+z*iz;
  ycen=x*jx+y*jy+z*jz;
  zcen=x*kx+y*ky+z*kz; /*rotated objects*/

  if((ycen-radius)>(zcen+radius)*tgh){continue;}
  if((ycen+radius)<(zcen+radius)*(-tgh)){continue;}
  if((xcen-radius)>(zcen+radius)*tgv){continue;}
  if((xcen+radius)<(zcen+radius)*(-tgv)){continue;}
  if(((zcen-radius)<zmax)&&((zcen+radius)>0)){

    fix=transform.vx[1]-transform.vx[0];
    fjx=transform.vx[2]-transform.vx[0];
    fkx=transform.vx[3]-transform.vx[0];
      fiy=transform.vy[1]-transform.vy[0];
      fjy=transform.vy[2]-transform.vy[0];
      fky=transform.vy[3]-transform.vy[0];
        fiz=transform.vz[1]-transform.vz[0];
        fjz=transform.vz[2]-transform.vz[0];
        fkz=transform.vz[3]-transform.vz[0]; /*unit vectors of the local axes in global system*/

    struct _list_item* faceNode = list_get_first(pMesh->faces);

    while(faceNode != 0){
      tria* pface = list_item_get_value(faceNode);
      face = *(tria*)list_item_get_value(faceNode);
        x=pface->x1;
        y=pface->y1;
        z=pface->z1;
      face.x1=transform.vx[0]+x*fix+y*fjx+z*fkx;
      face.y1=transform.vy[0]+x*fiy+y*fjy+z*fky;
      face.z1=transform.vz[0]+x*fiz+y*fjz+z*fkz;
        x=pface->x2;
        y=pface->y2;
        z=pface->z2;
      face.x2=transform.vx[0]+x*fix+y*fjx+z*fkx;
      face.y2=transform.vy[0]+x*fiy+y*fjy+z*fky;
      face.z2=transform.vz[0]+x*fiz+y*fjz+z*fkz;
        x=pface->x3;
        y=pface->y3;
        z=pface->z3;
      face.x3=transform.vx[0]+x*fix+y*fjx+z*fkx;
      face.y3=transform.vy[0]+x*fiy+y*fjy+z*fky;
      face.z3=transform.vz[0]+x*fiz+y*fjz+z*fkz; /*updated positions of triangles*/

      fclip(pSurface, pMesh->materials, &face,tgh,tgv, distmin, focal, &rotlight);

      faceNode = list_item_get_next(faceNode);
    }
  }
}

finish_frame(pSurface,distmin);

surface_end_rendering(pSurface);
}

void renderer_initialize()
{
  set_view_angle(FOV);
  set_double_pixel(DOUBLEPIX);
#if ASPCOR==1
  set_width_factor(WIDTHFACTOR);
#endif
}

void renderer_process()
{
}

void renderer_release()
{
  odis(0,0); // free static variables

  list_release(g_instances, 1);

  struct _list_item* meshNode = list_get_first(g_meshes);
  while(meshNode != 0)
  {
    mesh* pMesh = list_item_get_value(meshNode);
    list_release(pMesh->faces, 1);
    meshNode = list_item_get_next(meshNode);
  }
  list_release(g_meshes, 1);
  g_meshes = 0;
}
