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
{if(strcmp(s,"objtypes")==0){return 1;} /*pt. declarat nr. de tipuri de obiecte*/
 if(strcmp(s,"objects")==0){return 2;} /*pt. declarat nr. de obiecte*/
 if(strcmp(s,"background")==0){return 3;}
 if(strcmp(s,"clfactors")==0){return 4;}
 if(strcmp(s,"accel")==0){return 5;}
 if(strcmp(s,"brake")==0){return 6;} /*acceleration and brake torques/wheel*/
 if(strcmp(s,"spring")==0){return 7;}
 if(strcmp(s,"damper")==0){return 8;} /*suspension coefficients*/
 if(strcmp(s,"friction")==0){return 9;}
 if(strcmp(s,"trjoint")==0){return 10;} /*trailer joint*/
 if(strcmp(s,"camera")==0){return 11;} /*camera position*/
 if(strcmp(s,"ambient")==0){return 12;} /*ambient light*/
 if(strcmp(s,"headlight")==0){return 13;} /*forward light*/
 if(strcmp(s,"directional")==0){return 14;} /*intensity of light with fixed direction*/
 if(strcmp(s,"lightdir")==0){return 15;} /*direction of light*/
 if(strcmp(s,"fullbright")==0){return 16;}
 return 0;}


/*for readref()*/
int identcmg(char *s)
{if(strcmp(s,"box")==0){return 1;}
 if(strcmp(s,"cyl")==0){return 2;} /*cylinder*/
 if(strcmp(s,"sph")==0){return 3;} /*sphere*/
 if(strcmp(s,"trim")==0){return 4;} /*triangle mesh*/
 return 0;}


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
if(identcom(s)==16){ /*fullbright*/
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
	  case 1: // box
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

          case 2: // cylinder
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

          case 3: // sphere
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); lx=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z1=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); x2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); y2=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); z2=atof(s);
                  create_collision_sphere(x1,y1,z1,x2,y2,z2,lx);
	          break;

	  case 4: // triangle mesh
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
void eval_obj(int mesh_id,sgob *objs)
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
objs->radius=sqrt(lenx*lenx+leny*leny+lenz*lenz)/2;
}


/*function which reads the vehicle; must be called AFTER readtrack()
nrtyp,nrobt - number of object types and objects given by readtrack()*/
sgob** readvehicle(char *numefis,sgob** objs,int *nrtyp,int *nrobt,vhc *car)
{int err,lincr=1; /*lincr-current line*/
char s[MAXWLG]; /*a word*/
FILE *fis;
int i,k,nto,nob; /*number of object types and number of objects*/
REALN tx,ty,tz, /*initial translations*/
      len;
float spring = 0;  // hinge spring coefficient
float damper = 0;  // hinge damper coefficient

float friction = 0;

nto=*nrtyp;
nob=*nrobt;

int tjflag=0; // no trailer yet

  if(!(fis=fopen(numefis,"r"))){printf("Error: File %s could not be open\r\n",numefis);exit(1);}
s[0]='1';while(s[0]){
	if(!(err=fisgetw(fis,s,&lincr))){afermex(numefis,lincr,s,1);}

	switch(identcom(s)){
	  case 1: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); (*nrtyp)+=atoi(s);
	          for(i=nto+1;i<=(*nrtyp);i++){
                    create_mesh();
	            err=fisgetw(fis,s,&lincr); /*file with triangles*/
	            faces(i,s);
	              err=fisgetw(fis,s,&lincr); /*file with colors*/
	              readcolor(i,s);
                    create_collision_geometry();
	            err=fisgetw(fis,s,&lincr); /*file with reference points*/
	            readref(s);
	              err=fisgetw(fis,s,&lincr); /*file with data for backface culling*/
	              ordercl(i,s);
                    complete_mesh();
	          }
	          break;

	  case 2: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0);
	          car->nob=atoi(s); nob+=car->nob; (*nrobt)=nob;
	          if(!(objs=(sgob**)realloc(objs,(nob+1)*sizeof(sgob*)))){printf("Out of memory");}
	          for(i=(nob-car->nob+1);i<=nob;i++){
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0);
                      objs[i]=(sgob*)malloc(sizeof(sgob));
	              objs[i]->otyp=nto+atoi(s);
                      create_mesh_instance(objs[i]->otyp, &objs[i]->transform);
	              if(objs[i]->otyp>(*nrtyp)){
	                printf("Error: there is no object type '%d'\r\n",objs[i]->otyp-nto);exit(1);
	              }
	              objs[i]->transform.vx[0]=objs[i]->transform.vy[0]=objs[i]->transform.vz[0]=0;
	              objs[i]->transform.vx[1]=objs[i]->transform.vy[2]=objs[i]->transform.vz[3]=1;
	              objs[i]->transform.vx[2]=objs[i]->transform.vx[3]=0;
	              objs[i]->transform.vy[1]=objs[i]->transform.vy[3]=0;
	              objs[i]->transform.vz[1]=objs[i]->transform.vz[2]=0;
	              eval_obj(objs[i]->otyp,objs[i]);

                    k=i-nob+car->nob; /*1...car->nob*/
                    car->oid[k]=i;
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); car->ofc[k]=atoi(s);
	            switch(k){
	              case 1: if(car->ofc[k]!=1){printf("Error: '%s' line %d - second number must be 1\r\n",numefis,lincr); exit(1);}
	                      break;
	              case 2: if(car->ofc[k]==1){printf("Error: '%s' line %d - second number must not be 1\r\n",numefis,lincr); exit(1);}
	                      break;
	              default: if(car->ofc[k]<=2){printf("Error: '%s' line %d - second number must be > 2\r\n",numefis,lincr); exit(1);}
	                      break;
	            }
	            /*^object's function; 1-car; 2-trailer; 3,4,5,6-car wheel; 7-trailer wheel*/
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s);

 	              translat(objs[i],tx,ty,tz);

                    objs[i]->physics_object = create_collision_geometry_instance(objs[i]->otyp, tx, ty, tz, 0, 0, 0);
                    car->parts[k] = objs[i]->physics_object;

	            physics_createBody(objs[i]->physics_object, &objs[i]->transform);

                    if((car->ofc[k])>=2){physics_enableImprovedSpinning(objs[i]->physics_object, 1);}

	            if(!(err=fisgetw(fis,s,&lincr)))
	            {
                      int distribution_type;
                      afermex(numefis,lincr,s,1);
                      distribution_type = identcmg(s);

                      len = 0.0;
	              switch(distribution_type){
	                case 1: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); len=atof(s); /*mass*/
	                        err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
	                        err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
	                        err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s); /*box lengths*/
                                break;

	                case 3: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); len=atof(s); /*mass*/
	                        err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s); /*sphere radius*/
                                break;

                        default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
	              }
                      physics_setBodyMass(car->parts[k],len,distribution_type,tx,ty,tz);
                    }
                    physics_setBodyPosition(car->parts[k],objs[i]->transform.vx[0],objs[i]->transform.vy[0],objs[i]->transform.vz[0]);
	            /*^set mass parameters*/
	          }
	          break;

	  case 5: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->accel=atof(s);
                  break;

          case 6: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->brake=atof(s);
                  break;

          case 7: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); spring=atof(s);
                  break;

          case 8: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); damper=atof(s);
                  break;

          case 9: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); friction = atof(s);
                  break;

          case 10: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
                   err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
                   err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s);
                   if(car->ofc[2]!=2){printf("Error: '%s' line %d - trailer joint without trailer\r\n",numefis,lincr);exit(1);}
                   if(tjflag==1){printf("Error: '%s' line %d - only one such joint allowed\r\n",numefis,lincr);exit(1);}
                   tjflag=1;
                   physics_createUniversalJoint(car->parts[1], car->parts[2],tx,ty,tz);
                   break;

          case 11: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->camh=atof(s);
                   err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->camd=atof(s);
                   break;
	
	  default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
	}
}
fclose(fis);

/*set joints*/
for(i=1;i<=car->nob;i++){
  physics_setBodyFriction(car->parts[i], friction);

  if(car->ofc[i]>=3){

      float* vrx = 0, *af = 0, *bf = &car->bf;
      switch(car->ofc[i])
      {
        case 3:
          af = &car->af;
          break;
        case 4:
          vrx = &car->vrx;
          break;
        case 5:
          vrx = &car->vrx;
          af  = &car->af;
          break;
        default:
          break;
      }
      k=1; if(car->ofc[i]==7){k=2;}

      physics_createHinge2(car->parts[k],car->parts[i],objs[car->oid[i]]->transform.vx[0],objs[car->oid[i]]->transform.vy[0],objs[car->oid[i]]->transform.vz[0], vrx, af, bf, spring, damper);
  }
}
/*^set joints*/

return objs;}


/*function which reads the track; nrobt - number of objects*/
sgob** readtrack(char *numefis,int *nrobt,int *nrtyp,int* background_red, int* background_green, int* background_blue)
{int err,lincr=1; /*lincr-current line*/
char s[MAXWLG]; /*a word*/
FILE *fis;
int i,j,
    nto=0,nob=0, /*number of object types and number of objects; nob=(*nrobt) */
    bred=130,bgreen=160,bblue=200; /*background color*/
sgob** objs = 0;
REALN tx,ty,tz,rx,ry,rz, /*initial translations and rotations of the object*/
      fred=1.0,fgreen=1.0,fblue=1.0, /*color multiplication factors*/
      len;

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
	  case 3: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bred=atoi(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bgreen=atoi(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bblue=atoi(s);
	          break;

	  case 4: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fred=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fgreen=atof(s);
	          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fblue=atof(s);
	          break;

	  case 1: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); (*nrtyp)=nto=atoi(s);
	          for(i=1;i<=nto;i++){
                    create_mesh();
	            err=fisgetw(fis,s,&lincr); /*file with triangles*/
	            faces(i,s);
	              err=fisgetw(fis,s,&lincr); /*file with colors*/
	              readcolor(i,s);
                    create_collision_geometry();
	            err=fisgetw(fis,s,&lincr); /*file with reference points*/
	            readref(s);
	              err=fisgetw(fis,s,&lincr); /*file with data for backface culling*/
	              ordercl(i,s);
                    complete_mesh();
	          }
	          break;

	  case 2: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); (*nrobt)=nob=atoi(s);
	          if(!(objs=(sgob**)malloc((nob+1)*sizeof(sgob*)))){printf("Out of memory");}
	          for(i=1;i<=nob;i++){
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0);
                      objs[i]=(sgob*)malloc(sizeof(sgob));
	              objs[i]->otyp=atoi(s);
                      create_mesh_instance(objs[i]->otyp, &objs[i]->transform);
	              if(objs[i]->otyp>nto){
	                printf("Error: there is no object type '%d'\r\n",objs[i]->otyp);exit(1);
	              }
	              objs[i]->transform.vx[0]=objs[i]->transform.vy[0]=objs[i]->transform.vz[0]=0;
	              objs[i]->transform.vx[1]=objs[i]->transform.vy[2]=objs[i]->transform.vz[3]=1;
	              objs[i]->transform.vx[2]=objs[i]->transform.vx[3]=0;
	              objs[i]->transform.vy[1]=objs[i]->transform.vy[3]=0;
	              objs[i]->transform.vz[1]=objs[i]->transform.vz[2]=0;
	              eval_obj(objs[i]->otyp,objs[i]);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); rz=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ry=atof(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); rx=atof(s);
	              rotatz(objs[i],0,0,rz);
	              rotaty(objs[i],0,0,ry);
	              rotatx(objs[i],0,0,rx);
	              translat(objs[i],tx,ty,tz);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); objs[i]->lev=atoi(s);
	            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); // friction value not used

                    objs[i]->physics_object = create_collision_geometry_instance(objs[i]->otyp, tx, ty, tz, rx, ry, rz);
	          }
	          break;

	  case 12: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_ambient=atof(s); break;

	  case 13: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_headlight=atof(s); break;

	  case 14: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_directional=atof(s); break;

	  case 15: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_dx=atof(s);
	           err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_dy=atof(s);
	           err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light_dz=atof(s);
	           break;
	
	  default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
	}
}
fclose(fis);

for(i=1;i<=nto;i++){
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

*background_red=bred; *background_green=bgreen; *background_blue=bblue;

len=sqrt(light_dx*light_dx+light_dy*light_dy+light_dz*light_dz);
light_dx/=len;
light_dy/=len;
light_dz/=len; /*normalized light direction*/

set_ambient_light(light_ambient);
set_headlight(light_headlight);
set_directional_light(light_directional, light_dx, light_dy, light_dz);

return objs;}
