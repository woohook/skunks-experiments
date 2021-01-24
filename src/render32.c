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

#include "renderer3d.h"

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
float headlight_intensity;
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

struct material_action
{
  float* input;
  float last_input_state;
  struct material* pMaterial;
};

struct _list* g_meshes = 0;

struct tile_list
{
  int y, z;
  struct _list* entities;
};

int g_scene_initialized = 0;
int g_center_tile_y = 0;
int g_center_tile_z = 0;
struct _list* g_tile_lists = 0;
struct tile_list* g_scene[3][3] = {{0,0,0}, {0,0,0}, {0,0,0}};

struct _list* g_actions = 0;

pixcol g_backcol = {0,0,0};
lightpr g_light = {0,1.0f,0,0,0,0,0};
float g_width_factor = 1.0f;
float g_view_angle = 90.0f;
int g_double_pixel = 0;
float zmin=1e-3;
float zmax=120; // visibility (m)

struct frustum aFrustum;
struct lights theLights;

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

struct find_tile_list_params
{
  int y, z;
  struct tile_list* pOutTileList;
};

int find_tile_list(struct _list_item* pTileListEntry, void* pContext)
{
  struct find_tile_list_params* params = (struct find_tile_list_params*)pContext;
  struct tile_list* pTileList = list_item_get_value(pTileListEntry);
  if( (pTileList->y == params->y)
  &&  (pTileList->z == params->z))
  {
    params->pOutTileList = pTileList;
    return 1;
  }

  return 0;
}

struct tile_list* create_tile_list(int y, int z)
{
  printf("new tile_list (%d,%d)\n", y, z);

  struct tile_list* pTileList = (struct tile_list*)malloc(sizeof(struct tile_list));
  pTileList->y = y;
  pTileList->z = z;
  pTileList->entities = list_create();

  if(g_tile_lists == 0)
  {
    g_tile_lists = list_create();
  }
  list_add_value(g_tile_lists, pTileList);

  return pTileList;
}

struct mesh_instance* create_mesh_instance(struct _mesh* pMesh, matrix* transform)
{
  struct tile_list* pTileList = 0;

  int y = ((int)transform->vy[0]) / 100;
  int z = ((int)transform->vz[0]) / 100;
  struct find_tile_list_params params = {y,z,0};

  if( (g_tile_lists == 0)
  ||  (list_apply(g_tile_lists, find_tile_list, &params) != 1))
  {
    pTileList = create_tile_list(y,z);
  }
  else
  {
    pTileList = params.pOutTileList;
  }

  mesh_instance* pInstance = (mesh_instance*)malloc(sizeof(mesh_instance));
  if(pInstance == 0)
  {
    printf("Out of memory");
    exit(-1);
  }

  pInstance->pMesh = pMesh;
  pInstance->transform = transform;
  list_add_value(pTileList->entities, pInstance);

  return pInstance;
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

void add_material_action(struct _mesh* pMesh, float* input, int material_id)
{
  if(g_actions == 0)
  {
    g_actions = list_create();
  }

  struct material_action* action = (struct material_action*)malloc(sizeof(struct material_action));
  action->input = input;
  action->last_input_state = *input;
  action->pMaterial = list_get_value(pMesh->materials,material_id);
  list_add_value(g_actions, action);
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
  g_light.headlight_intensity = 1.0f;
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

//void displaysdl(struct _surface* pSurface,struct _list* materials,tria *face,float *distmin,float focal, lightpr* light);


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

int update_scene_tile_lists(struct _list_item* pTileListEntry, void* pContext)
{
  pContext = pContext;  // prevent unused parameter warning

  struct tile_list* pTileList = list_item_get_value(pTileListEntry);
  if( (pTileList->y >= g_center_tile_y - 1)
  &&  (pTileList->y <= g_center_tile_y + 1)
  &&  (pTileList->z >= g_center_tile_z - 1)
  &&  (pTileList->z <= g_center_tile_z + 1))
  {
    int tile_y = 1 + pTileList->y - g_center_tile_y;
    int tile_z = 1 + pTileList->z - g_center_tile_z;
    g_scene[tile_y][tile_z] = pTileList;
  }

  return 0;
}

void scene_update_tile_set(int new_center_tile_y, int new_center_tile_z)
{
  if( (new_center_tile_y != g_center_tile_y)
  ||  (new_center_tile_z != g_center_tile_z)
  ||  (g_scene_initialized == 0))
  {
    g_center_tile_y = new_center_tile_y;
    g_center_tile_z = new_center_tile_z;
    g_scene_initialized = 1;
    list_apply(g_tile_lists, update_scene_tile_lists, 0);
  }
}

/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera*/
void odis(struct _surface* pSurface, struct _matrix* view_transform)
{int j,focal;
unsigned int width,height;
static int sem=0; /*number of triangles for which memory has been allocated*/
static tria face; /*triangles and displayed triangles in global system*/
static matrix transform; /* current object's world transformation */
static float *distmin; /*Zbuffer for sending to displaysdl()*/

float tgh,tgv;
float x,y,z,ix,iy,iz,jx,jy,jz,kx,ky,kz; /*temporary variables for transformations*/
float xcen,ycen,zcen,radius;

struct _list_item* pActionItem;

if(pSurface==0){free(distmin); return;}
/*to free static variables, call odis(0,0,0,0,0,0,0)*/

pActionItem = 0;
if(g_actions != 0)
{
  pActionItem = list_get_first(g_actions);
}
while(pActionItem != 0)
{
  struct material_action* action = list_item_get_value(pActionItem);
  if( (action->pMaterial->red==255) && (action->pMaterial->green==255) && (action->pMaterial->blue==200) )
  {
    if( (action->last_input_state < 0.5f) && (*action->input > 0.5f) )
    {
      if(g_light.headlight_intensity > 0.5f)
      {
        g_light.headlight_intensity = 0.0f;
        action->pMaterial->fullbright = 0;
      }
      else
      {
        g_light.headlight_intensity = 1.0f;
        action->pMaterial->fullbright = 1;
      }
    }
  }
  if( (action->pMaterial->red==200) && (action->pMaterial->green==0) && (action->pMaterial->blue==0) )
  {
    if(*action->input == 0)
    {
      action->pMaterial->fullbright = 0;
    }
    else
    {
      action->pMaterial->fullbright = 1;
    }
  }

  action->last_input_state = *action->input;

  pActionItem = list_item_get_next(pActionItem);
}

width=surface_get_width(pSurface);
height=surface_get_height(pSurface);
focal=(int)(width/(2*tan(g_view_angle*0.008726646)));

tgh=(float)width/(2*(float)focal);
tgv=(float)height/(2*(float)focal);

aFrustum.zmin = zmin;
aFrustum.zmax = zmax;
aFrustum.tgh  = tgh;
aFrustum.tgv  = tgv;

if(g_width_factor != 1.0f)
{
  focal=(int)((float)focal/g_width_factor);
  tgv*=g_width_factor;
}

if(!sem){
  renderer3d_initialize(pSurface, &aFrustum, &theLights);
  sem=1;}

surface_begin_rendering(pSurface);

renderer3d_start_frame();

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
theLights.directional_light_x=x*ix+y*iy+z*iz;
theLights.directional_light_y=x*jx+y*jy+z*jz;
theLights.directional_light_z=x*kx+y*ky+z*kz; /*rotated light*/
theLights.ambient_light_intensity=g_light.ambient;
theLights.head_light_intensity=g_light.headlight*g_light.headlight_intensity;
theLights.directional_light_intensity=g_light.directional;

int new_center_tile_y = ((int)view_transform->vy[0]) / 100;
int new_center_tile_z = ((int)view_transform->vz[0]) / 100;
scene_update_tile_set(new_center_tile_y, new_center_tile_z);

for(int tile_y = 0; tile_y < 3; ++tile_y)
{
  for(int tile_z = 0; tile_z < 3; ++tile_z)
  {
    struct tile_list* pTileList = g_scene[tile_y][tile_z];
    if(pTileList == 0)
    {
      continue;
    }

struct _list_item* instanceNode = list_get_first(pTileList->entities);
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

  if(xcen-view_transform->vx[0]-radius>(1.74*aFrustum.zmax)){continue;}
  if(xcen-view_transform->vx[0]+radius<(-1.74*aFrustum.zmax)){continue;}
  if(ycen-view_transform->vy[0]-radius>(1.74*aFrustum.zmax)){continue;}
  if(ycen-view_transform->vy[0]+radius<(-1.74*aFrustum.zmax)){continue;}
  if(zcen-view_transform->vz[0]-radius>(1.74*aFrustum.zmax)){continue;}
  if(zcen-view_transform->vz[0]+radius<(-1.74*aFrustum.zmax)){continue;}

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
  if(((zcen-radius)<aFrustum.zmax)&&((zcen+radius)>0)){

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

      struct material* pMat = list_get_value(pMesh->materials, face.material_id);
      struct renderer_triangle tri = {face.x1,face.y1,face.z1,face.x2,face.y2,face.z2,face.x3,face.y3,face.z3,pMat->red,pMat->green,pMat->blue, (int)pMat->fullbright, (int)face.cull};
      renderer3d_render_triangle(&tri);

      faceNode = list_item_get_next(faceNode);
    }
  }
    }  // end for tile_z
  }  // end for tile_y
}

renderer3d_finish_frame(g_backcol.red, g_backcol.green, g_backcol.blue);

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

  for(int tile_y = 0; tile_y < 3; ++tile_y)
  {
    for(int tile_z = 0; tile_z < 3; ++tile_z)
    {
      g_scene[tile_y][tile_z] = 0;
    }
  }
  list_release(g_tile_lists,1);

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
