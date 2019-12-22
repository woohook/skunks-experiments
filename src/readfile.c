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

#include "readfile.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "list.h"
#include "config.h"
#include "defstr.h"
#include "render32.h"
#include "physics.h"
#include "vehicle.h"

#define UNKNOWN 0

#define BACKGROUND_COLOR 3
#define CLFACTORS 4
#define ACCELERATION_COEFF 5
#define BRAKE_COEFF 6
#define SPRING_COEFF 7
#define DAMPER_COEFF 8
#define FRICTION_COEFF 9
#define TRAILER_JOINT 10
#define CAMERA_POSITION 11
#define AMBIENT_LIGHT 12
#define HEADLIGHT 13
#define DIRECTIONAL_LIGHT 14
#define LIGHT_DIRECTION 15
#define FULLBRIGHT 16
#define OBJECT_INSTANCE 18

#define BOX 1
#define CYLINDER 2
#define SPHERE 3
#define TRIANGLE_MESH 4

#define CAR_BODY 1
#define TRAILER_BODY 2

#define MOTOR_WHEEL 3
#define STEERING_WHEEL 4
#define STEERING_MOTOR_WHEEL 5
#define FREE_WHEEL 6
#define TRAILER_WHEEL 7

struct object_type
{
  char name[MAXWLG];
  int index;
};

int g_numberOfMeshes = 0;
char item_name[] = "object0000";

struct _list* g_object_types = 0;

void prepare_item_name(int id)
{
  int index = id;
  for(int digit_index = 9; digit_index > 5; digit_index--)
  {
    item_name[digit_index] = (char)('0' + (index % 10));
    index = index / 10;
  }
}

/*functie care returneaza 1 daca i se transmite un caracter de delimitare si 0 daca nu*/
int verdel(char s)
{char a[6]={' ','\r','\n','\t',':'};
int i,sem=0;
  for(i=0;i<=4;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care returneaza 1 daca i se transmite o litera mica si 0 daca nu*/
int vercrl(char s)
{char a[28]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int i,sem=0;
  for(i=0;i<=25;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care returneaza 1 daca i se transmite o cifra sau '.' si 0 daca nu*/
int vercrcf(char s)
{char a[13]={'0','1','2','3','4','5','6','7','8','9','.'};
int i,sem=0;
  for(i=0;i<=10;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care se uita sa vada ce e un sir de caractere: intreg(0), cuvant(1), real(2) sau altceva(3) ...*/
int detsir(char *s)
{int i=1,sem=3,crp=0; /*crp - variabila pentru numarat punctele*/
  if(vercrl(s[0])){sem=1; /*adica daca primul caracter e o litera*/
    while((!verdel(s[i]))&&s[i]){if(!vercrl(s[i])){sem=3;return sem;} i++;}
  return sem;}
    if((vercrcf(s[0]))||(s[0]=='-')){sem=2; /*adica daca primul caracter e o cifra*/
	if(s[0]=='.'){crp++;}
      while((!verdel(s[i]))&&s[i]){
	if(s[i]=='.'){crp++;}
	if(!vercrcf(s[i])){sem=3;return sem;} i++;
      }if((crp>=2)||((crp==1)&&(i==1))){sem=3;return sem;}
    if((!crp)&&(s[0]!='-')){sem=0;}
    return sem;}
return sem;}


/*functie care citeste un cuvant s din fisierul fis; lincr - linia curenta*/
int fisgetw(FILE *fis,char *s,int *lincr)
{int i,sem=0;
  static int sem2=0;if(sem2){(*lincr)++;sem2=0;}
s[0]=getc(fis);if(s[0]=='\n'){(*lincr)++;}
  while((!(feof(fis)))&&(verdel(s[0]))){s[0]=getc(fis);if(s[0]=='\n'){(*lincr)++;}}
  /*sarit peste caracterele de dinaintea cuvantului*/
i=1;
  while((!(feof(fis)))&&(i<MAXWLG)&&(!verdel(s[i-1])))
  {s[i]=getc(fis);i++;} if(!verdel(s[i-1])){sem=1;}
  if(s[i-1]=='\n'){sem2=1;} s[i-1]='\0';
return sem;} /*1 daca s-a depasit lungimea maxima a cuvantului sau sfarsitul fisierului si 0 daca nu*/


/*for readtrack()*/
int identcom(char *s)
{if(strcmp(s,"background")==0){return BACKGROUND_COLOR;}
 if(strcmp(s,"clfactors")==0){return CLFACTORS;}
 if(strcmp(s,"accel")==0){return ACCELERATION_COEFF;}
 if(strcmp(s,"brake")==0){return BRAKE_COEFF;}
 if(strcmp(s,"spring")==0){return SPRING_COEFF;}
 if(strcmp(s,"damper")==0){return DAMPER_COEFF;}
 if(strcmp(s,"friction")==0){return FRICTION_COEFF;}
 if(strcmp(s,"trjoint")==0){return TRAILER_JOINT;}
 if(strcmp(s,"camera")==0){return CAMERA_POSITION;}
 if(strcmp(s,"ambient")==0){return AMBIENT_LIGHT;}
 if(strcmp(s,"headlight")==0){return HEADLIGHT;}
 if(strcmp(s,"directional")==0){return DIRECTIONAL_LIGHT;}
 if(strcmp(s,"lightdir")==0){return LIGHT_DIRECTION;}
 if(strcmp(s,"fullbright")==0){return FULLBRIGHT;}
 if(strcmp(s,"object")==0){return OBJECT_INSTANCE;}
 return UNKNOWN;}


/*for readref()*/
int identcmg(char *s)
{if(strcmp(s,"box")==0){return BOX;}
 if(strcmp(s,"cyl")==0){return CYLINDER;}
 if(strcmp(s,"sph")==0){return SPHERE;}
 if(strcmp(s,"trim")==0){return TRIANGLE_MESH;}
 return UNKNOWN;}


/*functie care afiseaza un mesaj de eroare si iese din program daca e cazul*/
void afermex(char *numefis,int lincr,char *s,int cdvr)
{/*cdvr - conditie de verificare (0-intreg,1-cuvant,2-real)*/
if((cdvr==0)&&(detsir(s)!=0)){printf("Error: '%s' line %d - integer expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
if((cdvr==1)&&(detsir(s)!=1)){printf("Error: '%s' line %d - word expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
if((cdvr==2)&&(detsir(s)!=2)&&(detsir(s)!=0)){printf("Error: '%s' line %d - number expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
}


/*functie care afla numarul de triunghiuri*/
int findnf(char *numefis)
{int nvert,nrfaces;
FILE *fis;

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&nvert);
fscanf(fis,"%d",&nrfaces);

fclose(fis);
return nrfaces;}


/*functie care afla coordonatele varfurilor triunghiurilor*/
void faces(int mesh_id, char *numefis)
{int lincr=0,i,j,k,l,nrfaces,nvert;
FILE *fis;
REALN *x,*y,*z;
char s[MAXWLG];

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&nvert);
fscanf(fis,"%d",&nrfaces);

if(!(x=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory");}
if(!(y=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory");}
if(!(z=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory");}

for(i=1;i<=nvert;i++){
  fisgetw(fis,s,&lincr); x[i]=atof(s);
  fisgetw(fis,s,&lincr); y[i]=atof(s);
  fisgetw(fis,s,&lincr); z[i]=atof(s);
} /*aflat coordonatele punctelor*/

for(i=1;i<=nrfaces;i++){
  fisgetw(fis,s,&lincr); /*sarit peste "f"*/
  fscanf(fis,"%d %d %d", &j, &k, &l);
  add_face(mesh_id, x[j], y[j], z[j], x[k], y[k], z[k], x[l], y[l], z[l]);
}

free(x);free(y);free(z);
fclose(fis);}


/*function which reads colors of triangles
*bred,*bgreen,*bblue - background color*/
void readcolor(int mesh_id,char *numefis)
{int i,j,colors,fstart,fend,fred,fgreen,fblue; /*colors - number of colors*/
/*fstart si fend - first and last triangle with color(fred,fgreen,fblue)*/
FILE *fis;
char s[MAXWLG];

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&colors); /*found number of colors*/

for(j=1;j<=colors;j++){
fscanf(fis,"%d %d %d %d %d",&fstart,&fend,&fred,&fgreen,&fblue);
  for(i=fstart;i<=fend;i++)
  {
    set_face_color(mesh_id, i, fred, fgreen, fblue);
  }
}

j=fisgetw(fis,s,&i);
if(identcom(s)==FULLBRIGHT){
  fscanf(fis,"%d",&colors);
  for(j=1;j<=colors;j++){
    fscanf(fis,"%d %d",&fstart,&fend);
    for(i=fstart;i<=fend;i++){
      set_face_fullbright(mesh_id, i);
    }
  }
}

fclose(fis);
}


/*function which reads the reference points and geom types of an object*/
void readref(char *numefis)
{int i,err,lincr,ngeom; /*number of geoms*/
char s[MAXWLG];
FILE *fis;
float x1, y1, z1, x2, y2, z2, lx, ly, lz;
int ttip;

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&ngeom);
if(ngeom>=(MAXGEOM-1)){printf("File '%s' - too many geoms\r\n",numefis);exit(1);}

for(i=1;i<=ngeom;i++){
	if(!(err=fisgetw(fis,s,&lincr))){afermex(numefis,lincr,s,1);}

	switch(identcmg(s)){
	  case BOX:
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); lx=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ly=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); lz=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z2=atof(s);
                  create_collision_box(x1,y1,z1,x2,y2,z2,lx,ly,lz);
	          break;

          case CYLINDER:
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); lx=atof(s); // radius
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ly=atof(s); // length
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z2=atof(s);
                  create_collision_cylinder(x1,y1,z1,x2,y2,z2,lx,ly);
	          break;

          case SPHERE:
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); lx=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z2=atof(s);
                  create_collision_sphere(x1,y1,z1,x2,y2,z2,lx);
	          break;

	  case TRIANGLE_MESH:
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ttip=atoi(s); // type
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z2=atof(s);
                  create_collision_mesh(x1,y1,z1,x2,y2,z2,ttip);
	          break;

	  default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
	}
}

fclose(fis);
}


/*order vertices, so triangles can be culled correctly*/
void ordercl(int mesh_id,char *numefis)
{int i,j,nrcmd,nf1,nf2;
REALN x,y,z,dx,dy,dz,a,b,c,d,prodscal;
REALN x1,y1,z1, x2,y2,z2, x3,y3,z3;
char vis; /*'v'-visible from (x,y,z); 'i'-not visible*/
FILE *fis;

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&nrcmd);

for(i=1;i<=nrcmd;i++){
  fscanf(fis,"%d %d %c %f %f %f",&nf1,&nf2,&vis,&x,&y,&z);

  for(j=nf1;j<=nf2;j++){
    get_face_vertex(mesh_id,j,1,&x1,&y1,&z1);
    get_face_vertex(mesh_id,j,2,&x2,&y2,&z2);
    get_face_vertex(mesh_id,j,3,&x3,&y3,&z3);
    findplan(x1,y1,z1,x2,y2,z2,x3,y3,z3,&a,&b,&c,&d);
     dx=x1-x;
     dy=y1-y;
     dz=z1-z;
    prodscal=a*dx+b*dy+c*dz;
    switch(vis){
      case 'v': enable_face_culling(mesh_id,j);
        if(prodscal<0){
          flip_face(mesh_id,j);
        } break;

      case 'i': enable_face_culling(mesh_id,j);
        if(prodscal>=0){
          flip_face(mesh_id,j);
        } break;

      default: break;
    }
  }
}

fclose(fis);
}


/*function which finds the center and size of the object*/
void eval_obj(int mesh_id,sgob *object)
{int i,nrfaces;
REALD xmin,xmax,ymin,ymax,zmin,zmax,lenx,leny,lenz;
REALN x1,y1,z1, x2,y2,z2, x3,y3,z3;

nrfaces=get_face_count(mesh_id);

get_face_vertex(mesh_id,1,1,&x1,&y1,&z1);
xmin=xmax=x1;
ymin=ymax=y1;
zmin=zmax=z1;

for(i=1;i<=nrfaces;i++){
get_face_vertex(mesh_id,i,1,&x1,&y1,&z1);
get_face_vertex(mesh_id,i,2,&x2,&y2,&z2);
get_face_vertex(mesh_id,i,3,&x3,&y3,&z3);

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
}

lenx=xmax-xmin;
leny=ymax-ymin;
lenz=zmax-zmin;
object->radius=sqrt(lenx*lenx+leny*leny+lenz*lenz)/2;
}

int find_object_type(char* name)
{
  int object_type_index = 0;

  struct _list_item* pObjectTypeItem = 0;

  if(g_object_types == 0)
  {
    g_object_types = list_create();
  }

  pObjectTypeItem = list_get_first(g_object_types);
  while(pObjectTypeItem != 0)
  {
    struct object_type* pObjectType = list_item_get_value(pObjectTypeItem);
    pObjectTypeItem = list_item_get_next(pObjectTypeItem);

    if(strcmp(pObjectType->name, name)==0)
    {
      object_type_index = pObjectType->index;
      break;
    }
  }

  return object_type_index;
}

int load_object_type(char* part_name)
{
  int line_number = 1;
  char filename[MAXWLG];
  FILE* part_stream = 0;

  part_stream = fopen(part_name, "r");
  if(!part_stream)
  {
    printf("Error: File %s could not be opened\r\n", part_name);
    exit(1);
  }

  if(g_object_types == 0)
  {
    g_object_types = list_create();
  }

  g_numberOfMeshes++;
  struct object_type* pNewType = (struct object_type*)malloc(sizeof(struct object_type));
  strcpy(pNewType->name, part_name);
  pNewType->index = g_numberOfMeshes;
  list_add_value(g_object_types, pNewType);

  create_mesh();

  fisgetw(part_stream, filename, &line_number); // file with triangles
  faces(g_numberOfMeshes, filename);

  fisgetw(part_stream, filename, &line_number); // file with colors
  readcolor(g_numberOfMeshes, filename);

  create_collision_geometry();
  fisgetw(part_stream, filename, &line_number); // file with reference points
  readref(filename);

  fisgetw(part_stream, filename, &line_number); // file with data for backface culling
  ordercl(g_numberOfMeshes, filename);

  complete_mesh();

  fclose(part_stream);

  return g_numberOfMeshes;
}

/*function which reads the vehicle; must be called AFTER readtrack()
nrtyp,nrobt - number of object types and objects given by readtrack()*/
struct _sgob* readvehicle(char *numefis, float dx, float dy, float dz)
{int err,lincr=1; /*lincr-current line*/
char s[MAXWLG]; /*a word*/
FILE *fis;
int object_type_index = 0;
REALN tx,ty,tz, /*initial translations*/
      len;
float spring = 0;  // hinge spring coefficient
float damper = 0;  // hinge damper coefficient

float friction = 0;

sgob* object = 0;
struct physics_instance* primaryBody = 0, *secondaryBody = 0;
int part_type_id = 0;

int tjflag=0; // no trailer yet

struct _list* parts = list_create();
struct _list* parts_types = list_create();
vhc* car = vehicle_create();
car->action_accelerate = 0.0f;
car->action_brake = 0.0f;
car->action_left = 0.0f;
car->action_right = 0.0f;
car->action_reverse = 0.0f;

start_physics_group();

  if(!(fis=fopen(numefis,"r"))){printf("Error: File %s could not be open\r\n",numefis);exit(1);}
s[0]='1';while(s[0]){
	if(!(err=fisgetw(fis,s,&lincr))){afermex(numefis,lincr,s,1);}

	switch(identcom(s)){
	  case OBJECT_INSTANCE:
	            err=fisgetw(fis,s,&lincr);
                    object_type_index = find_object_type(s);
                    if(!object_type_index)
                    {
                      object_type_index = load_object_type(s);
                    }
                    prepare_item_name(list_get_size(parts));

                      object=(sgob*)malloc(sizeof(sgob));
                      object->otyp = 0;
                      object->radius = 0;
                      object->lev = 0;
                      object->physics_object = 0;
                      object->vehicle = 0;
                      if(list_get_size(parts)==0)
                      {
                        object->vehicle = car;
                      }

                      list_add_value(parts,object);
	              object->otyp = object_type_index;
                      create_mesh_instance(object->otyp, &object->transform);
	              object->transform.vx[0]=object->transform.vy[0]=object->transform.vz[0]=0;
	              object->transform.vx[1]=object->transform.vy[2]=object->transform.vz[3]=1;
	              object->transform.vx[2]=object->transform.vx[3]=0;
	              object->transform.vy[1]=object->transform.vy[3]=0;
	              object->transform.vz[1]=object->transform.vz[2]=0;
	              eval_obj(object->otyp,object);

	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); part_type_id = atoi(s);
	            list_add_value(parts_types, (void*)part_type_id);
	            switch(list_get_size(parts)){
	              case 1: if(part_type_id!=1){printf("Error: '%s' line %d - Car body not yet defined. Second number must be 1\r\n",numefis,lincr); exit(1);}
	                      break;
	              case 2: if(part_type_id==1){printf("Error: '%s' line %d - Car body already defined. Second number must not be 1\r\n",numefis,lincr); exit(1);}
	                      break;
	              default: if(part_type_id<=2){printf("Error: '%s' line %d - second number must be > 2\r\n",numefis,lincr); exit(1);}
	                      break;
	            }
	            /*^object's function; 1-car; 2-trailer; 3,4,5,6-car wheel; 7-trailer wheel*/
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s);
	            tx+=dx; ty+=dy; tz+=dz;

	              translat(&object->transform,tx,ty,tz);

                    object->physics_object = create_collision_geometry_instance(object->otyp, tx, ty, tz, 0, 0, 0, &object->transform);

                    if(part_type_id == CAR_BODY)
                    {
                      primaryBody = object->physics_object;
                    }
                    if(part_type_id == TRAILER_BODY)
                    {
                      secondaryBody = object->physics_object;
                    }

                    if(part_type_id >= 2){physics_enableImprovedSpinning(object->physics_object, 1);}

	            if(!(err=fisgetw(fis,s,&lincr)))
	            {
                      int distribution_type;
                      afermex(numefis,lincr,s,1);
                      distribution_type = identcmg(s);

                      len = 0.0;
	              switch(distribution_type){
	                case BOX: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); len=atof(s); /*mass*/
	                        err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
	                        err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
	                        err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s); /*box lengths*/
                                break;

	                case SPHERE: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); len=atof(s); /*mass*/
	                        err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s); /*sphere radius*/
                                break;

                        default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
	              }
                      physics_setBodyMass(object->physics_object,len,distribution_type,tx,ty,tz);
                    }
                    physics_setBodyPosition(object->physics_object,object->transform.vx[0],object->transform.vy[0],object->transform.vz[0]);
	            /*^set mass parameters*/
	          break;

	  case ACCELERATION_COEFF: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->accel=atof(s);
                  break;

          case BRAKE_COEFF: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->brake=atof(s);
                  break;

          case SPRING_COEFF: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); spring=atof(s);
                  break;

          case DAMPER_COEFF: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); damper=atof(s);
                  break;

          case FRICTION_COEFF: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); friction = atof(s);
                  break;

          case TRAILER_JOINT: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
                   err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
                   err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s);
                   if((int)list_get_value(parts_types,1)!=2){printf("Error: '%s' line %d - trailer joint without trailer\r\n",numefis,lincr);exit(1);}
                   if(tjflag==1){printf("Error: '%s' line %d - only one such joint allowed\r\n",numefis,lincr);exit(1);}
                   tjflag=1;
                   physics_createUniversalJoint(primaryBody, secondaryBody,tx,ty,tz);
                   break;

          case CAMERA_POSITION: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2);  // camera position ignored
                   err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2);
                   break;
	
	  default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
	}
}
fclose(fis);

/*set joints*/
struct _list_item* part = list_get_first(parts);
struct _list_item* part_type = list_get_first(parts_types);

while(part != 0){
  int object_type = (int)list_item_get_value(part_type);
  object = list_item_get_value(part);
  physics_setBodyFriction(object->physics_object, friction);
  part = list_item_get_next(part);
  part_type = list_item_get_next(part_type);

  if(object_type>=3){

      struct physics_instance* parentBody = primaryBody;
      float* vrx = 0, *af = 0, *bf = &car->bf;
      switch(object_type)
      {
        case MOTOR_WHEEL:
          af = &car->af;
          break;
        case STEERING_WHEEL:
          vrx = &car->vrx;
          break;
        case STEERING_MOTOR_WHEEL:
          vrx = &car->vrx;
          af  = &car->af;
          break;
        case TRAILER_WHEEL:
          parentBody = secondaryBody;
          break;
        default:
          break;
      }

      physics_createHinge2(parentBody,object->physics_object,object->transform.vx[0],object->transform.vy[0],object->transform.vz[0], vrx, af, bf, spring, damper);
  }
}
/*^set joints*/

  object = list_get_value(parts,0);

list_release(parts_types, 0);
list_release(parts, 0);

  end_physics_group();

  return object;
}


// function which reads the track
void readtrack(char *numefis)
{int err,lincr=1; /*lincr-current line*/
char s[MAXWLG]; /*a word*/
FILE *fis;
int i,j,
    bred=130,bgreen=160,bblue=200; /*background color*/
int object_type_index = 0;
sgob* object = 0;
REALN tx,ty,tz,rx,ry,rz, /*initial translations and rotations of the object*/
      fred=1.0,fgreen=1.0,fblue=1.0, /*color multiplication factors*/
      len;
int previousNumberOfMeshes = g_numberOfMeshes;
int object_index = previousNumberOfMeshes+1;

float light_ambient=0.5;
float light_headlight=0.3;
float light_directional=0.5;
float light_dx=-0.5;
float light_dy=1;
float light_dz=1; /*set default values for the light*/

  if(!(fis=fopen(numefis,"r"))){printf("Error: File %s could not be open\r\n",numefis);exit(1);}
s[0]='1';while(s[0]){
	if(!(err=fisgetw(fis,s,&lincr))){afermex(numefis,lincr,s,1);}

	switch(identcom(s)){
	  case BACKGROUND_COLOR: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bred=atoi(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bgreen=atoi(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bblue=atoi(s);
	          break;

	  case CLFACTORS: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fred=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fgreen=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fblue=atof(s);
	          break;

	  case OBJECT_INSTANCE:
	            err=fisgetw(fis,s,&lincr);
                    object_type_index = find_object_type(s);
                    if(!object_type_index)
                    {
                      object_type_index = load_object_type(s);
                    }
                      prepare_item_name(object_index);

                      object=(sgob*)malloc(sizeof(sgob));
                      object_index++;
                      object->otyp = 0;
                      object->radius = 0;
                      object->lev = 0;
                      object->physics_object = 0;
                      object->vehicle = 0;

	              object->otyp=object_type_index;
                      create_mesh_instance(object->otyp, &object->transform);
	              object->transform.vx[0]=object->transform.vy[0]=object->transform.vz[0]=0;
	              object->transform.vx[1]=object->transform.vy[2]=object->transform.vz[3]=1;
	              object->transform.vx[2]=object->transform.vx[3]=0;
	              object->transform.vy[1]=object->transform.vy[3]=0;
	              object->transform.vz[1]=object->transform.vz[2]=0;
	              eval_obj(object->otyp,object);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); rz=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ry=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); rx=atof(s);
	              rotatz(&object->transform,0,0,rz);
	              rotaty(&object->transform,0,0,ry);
	              rotatx(&object->transform,0,0,rx);
	              translat(&object->transform,tx,ty,tz);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); object->lev=atoi(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); // friction value not used

                    object->physics_object = create_collision_geometry_instance(object->otyp, tx, ty, tz, rx, ry, rz, 0);
	          break;

	  case AMBIENT_LIGHT: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_ambient=atof(s); break;

	  case HEADLIGHT: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_headlight=atof(s); break;

	  case DIRECTIONAL_LIGHT: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_directional=atof(s); break;

	  case LIGHT_DIRECTION: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_dx=atof(s);
	           err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_dy=atof(s);
	           err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_dz=atof(s);
	           break;
	
	  default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
	}
}
fclose(fis);

for(i=previousNumberOfMeshes+1;i<=g_numberOfMeshes;i++){
  for(j=1;j<=get_face_count(i);j++){
    int red, green, blue;
    get_face_color(i,j,&red,&green,&blue);
    len=red*fred;
    if(len>255){len=255;}
    red=(int)len;
    len=green*fgreen;
    if(len>255){len=255;}
    green=(int)len;
    len=blue*fblue;
    if(len>255){len=255;}
    blue=(int)len;
    set_face_color(i, j, red, green, blue);
  }
}

len=sqrt(light_dx*light_dx+light_dy*light_dy+light_dz*light_dz);
light_dx/=len;
light_dy/=len;
light_dz/=len; /*normalized light direction*/

set_ambient_light(light_ambient);
set_headlight(light_headlight);
set_directional_light(light_directional, light_dx, light_dy, light_dz);
set_background_color(bred,bgreen,bblue);
}
