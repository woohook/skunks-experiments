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

/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera*/
void odis(SDL_Surface *screen,sgob *objs,int nob,pixcol backcol,REALN zfog,REALN zmax,sgob *cam,lightpr *light)
{int i,j,focal;
unsigned int width,height;
unsigned long int area;
static int sem=0,nrfm=0; /*number of triangles for which memory has been allocated*/
static tria *face,*facedisp; /*triangles and displayed triangles in global system*/
static sgob *obdis; /*displayed objects (copies)*/
static REALN *distmin; /*Zbuffer for sending to displaysdl()*/
int nrfaces,nrdisp,crf, /*number of triangles and of displayed triangles, current triangle*/
    nobdis=0; /*number of displayed objects*/

lightpr rotlight; /*rotated light parameters*/

REALN tgh,tgv,zmin;
REALN x,y,z,ix,iy,iz,jx,jy,jz,kx,ky,kz; /*temporary variables for transformations*/

if(nob==0){free(face); free(facedisp); free(obdis); free(distmin); return;}
/*to free static variables, call odis(0,0,0,backcol,0,0,0,0)*/

width=SCREENWIDTH;
height=SCREENHEIGHT;
focal=(int)(width/(2*tan(FOV*0.008726646)));

area=(width+1)*(height+1);

tgh=(REALN)width/(2*(REALN)focal);
tgv=(REALN)height/(2*(REALN)focal);

#if ASPCOR==1
  focal=(int)((REALN)focal/WIDTHFACTOR);
  tgv*=WIDTHFACTOR;
#elif ASPCOR!=0
  printf("Error: ASPCOR should be set to 0 or 1 in 'src/config.h'\r\n"); exit(1);
#endif

zmin=1e-3;

if(!sem){
  if(!(distmin=(REALN *)malloc(area*sizeof(REALN)))){printf("Out of memory");}
  if(!(face=(tria *)malloc(11*sizeof(tria)))){printf("Out of memory");}
  if(!(facedisp=(tria *)malloc(22*sizeof(tria)))){printf("Out of memory");}
  if(!(obdis=(sgob *)malloc((nob+1001)*sizeof(sgob)))){printf("Out of memory");}
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

x=light->dx;
y=light->dy;
z=light->dz;
rotlight.dx=x*ix+y*iy+z*iz;
rotlight.dy=x*jx+y*jy+z*jz;
rotlight.dz=x*kx+y*ky+z*kz; /*rotated light*/
rotlight.ambient=light->ambient;
rotlight.headlight=light->headlight;
rotlight.directional=light->directional;

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

displaysdl(screen,facedisp,nrdisp,distmin,width,height,focal,backcol,zfog,zmax,&rotlight);

}
