/*
Copyright (C) 2012 Victor Matei Petrescu

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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "readfile.h"


typedef struct _mtriangle
{int vert[3];} mtriangle;


typedef struct _mvertex
{REALN x; REALN y; REALN z;} mvertex;


typedef struct _mcolor
{int start; int end; int red; int green; int blue;} mcolor;


typedef struct _mbfcl
{int start; int end; char flag; REALN x; REALN y; REALN z;} mbfcl;


typedef struct _mobject
{int type; REALN tx; REALN ty; REALN tz; int addv; int addt;} mobject;


int main(int argc,char *argv[])
{mtriangle **mtri,*mtritot;
mvertex **mvert,*mverttot;
mcolor **mcol;
mbfcl **mbf;
mobject *mobj;
char numefis[MAXWLG],s[MAXWLG];
int nob,nco, /*number of objects*/
    *nrtr,*nrv,*nrcl,*nrbc, /*nr. of vertices, triangles and colors for each object type*/
    nrtrtot,nrvtot,nrcltot,nrbctot,
    i,j,
    err,lincr=1;
FILE *f1,*f2,*fgeo,*fcol,*fcld;

if(argc!=2){printf("Wrong number of arguments\r\n"); exit(1);}

strcpy(numefis,argv[1]);

if(!(f1=fopen(numefis,"r"))){printf("Could not open file '%s'\r\n",numefis); exit(1);}

s[0]='1';while(s[0]){
  if(!(err=fisgetw(f1,s,&lincr))){afermex(numefis,lincr,s,1);}

  switch(identcom(s)){
    case 1: err=fisgetw(f1,s,&lincr);afermex(numefis,lincr,s,0); nob=atoi(s);

            if(!(mtri=(mtriangle **)malloc((nob+1)*sizeof(mtriangle *)))){printf("Out of memory"); exit(1);}
            if(!(mvert=(mvertex **)malloc((nob+1)*sizeof(mvertex *)))){printf("Out of memory"); exit(1);}
            if(!(mcol=(mcolor **)malloc((nob+1)*sizeof(mcolor *)))){printf("Out of memory"); exit(1);}
            if(!(mbf=(mbfcl **)malloc((nob+1)*sizeof(mbfcl *)))){printf("Out of memory"); exit(1);}
            if(!(nrtr=(int *)malloc((nob+1)*sizeof(int)))){printf("Out of memory"); exit(1);}
            if(!(nrv=(int *)malloc((nob+1)*sizeof(int)))){printf("Out of memory"); exit(1);}
            if(!(nrcl=(int *)malloc((nob+1)*sizeof(int)))){printf("Out of memory"); exit(1);}
            if(!(nrbc=(int *)malloc((nob+1)*sizeof(int)))){printf("Out of memory"); exit(1);}

            for(i=1;i<=nob;i++){
              err=fisgetw(f1,s,&lincr); /*file with triangles*/
              if(!(f2=fopen(s,"r"))){printf("Could not open file '%s'\r\n",s); exit(1);}
              fscanf(f2,"%d %d",&nrv[i],&nrtr[i]);
              if(!(mvert[i]=(mvertex *)malloc((nrv[i]+1)*sizeof(mvertex)))){printf("Out of memory"); exit(1);}
              if(!(mtri[i]=(mtriangle *)malloc((nrtr[i]+1)*sizeof(mtriangle)))){printf("Out of memory"); exit(1);}
              for(j=1;j<=nrv[i];j++){
                fscanf(f2,"%f %f %f",&(mvert[i][j].x),&(mvert[i][j].y),&(mvert[i][j].z));
              }
              for(j=1;j<=nrtr[i];j++){
                err=fisgetw(f2,s,&lincr); /*skip "f"*/
                fscanf(f2,"%d %d %d",&(mtri[i][j].vert[0]),&(mtri[i][j].vert[1]),&(mtri[i][j].vert[2]));
              }
              fclose(f2);

              err=fisgetw(f1,s,&lincr); /*file with colors*/
              if(!(f2=fopen(s,"r"))){printf("Could not open file '%s'\r\n",s); exit(1);}
              fscanf(f2,"%d",&nrcl[i]);
              if(!(mcol[i]=(mcolor *)malloc((nrcl[i]+1)*sizeof(mcolor)))){printf("Out of memory"); exit(1);}
              for(j=1;j<=nrcl[i];j++){
                fscanf(f2,"%d %d %d %d %d",&(mcol[i][j].start),&(mcol[i][j].end),&(mcol[i][j].red),&(mcol[i][j].green),&(mcol[i][j].blue));
              }
              fclose(f2);

              err=fisgetw(f1,s,&lincr); /*file with points used for backface culling*/
              if(!(f2=fopen(s,"r"))){printf("Could not open file '%s'\r\n",s); exit(1);}
              fscanf(f2,"%d",&nrbc[i]);
              if(!(mbf[i]=(mbfcl *)malloc((nrbc[i]+1)*sizeof(mbfcl)))){printf("Out of memory"); exit(1);}
              for(j=1;j<=nrbc[i];j++){
                fscanf(f2,"%d %d %c %f %f %f",&(mbf[i][j].start),&(mbf[i][j].end),&(mbf[i][j].flag),&(mbf[i][j].x),&(mbf[i][j].y),&(mbf[i][j].z));
              }
              fclose(f2);
            }
            break; /*object types read*/

    case 2: err=fisgetw(f1,s,&lincr);afermex(numefis,lincr,s,0); nco=atoi(s);
            if(!(mobj=(mobject *)malloc((nco+1)*sizeof(mobject)))){printf("Out of memory"); exit(1);}
            nrtrtot=nrvtot=nrcltot=nrbctot=mobj[0].addv=mobj[0].addt=0;
            for(i=1;i<=nco;i++){
              fscanf(f1,"%d %f %f %f",&(mobj[i].type),&(mobj[i].tx),&(mobj[i].ty),&(mobj[i].tz));
              nrvtot+=nrv[mobj[i].type]; nrtrtot+=nrtr[mobj[i].type];
              nrcltot+=nrcl[mobj[i].type]; nrbctot+=nrbc[mobj[i].type];
              mobj[i].addv=mobj[i-1].addv+nrv[mobj[i].type];
              mobj[i].addt=mobj[i-1].addt+nrtr[mobj[i].type];
            }

            if(!(mtritot=(mtriangle *)malloc((nrtrtot+1)*sizeof(mtriangle)))){printf("Out of memory"); exit(1);}
            if(!(mverttot=(mvertex *)malloc((nrvtot+1)*sizeof(mvertex)))){printf("Out of memory"); exit(1);}

            if(!(fgeo=fopen("out.geo","w"))){printf("Could not open file 'out.geo'\r\n"); exit(1);}
            if(!(fcol=fopen("out.col","w"))){printf("Could not open file 'out.col'\r\n"); exit(1);}
            if(!(fcld=fopen("out.cld","w"))){printf("Could not open file 'out.cld'\r\n"); exit(1);}
            fprintf(fgeo,"%d %d\r\n",nrvtot,nrtrtot);
            fprintf(fcol,"%d\r\n",nrcltot);
            fprintf(fcld,"%d\r\n",nrbctot);

            for(i=1;i<=nco;i++){
              for(j=1;j<=nrv[mobj[i].type];j++){
                fprintf(fgeo,"%1.4f %1.4f %1.4f\r\n",mvert[mobj[i].type][j].x+mobj[i].tx,mvert[mobj[i].type][j].y+mobj[i].ty,mvert[mobj[i].type][j].z+mobj[i].tz);
              }
            }

            for(i=1;i<=nco;i++){
              for(j=1;j<=nrtr[mobj[i].type];j++){
                fprintf(fgeo,"f %d %d %d\r\n",mtri[mobj[i].type][j].vert[0]+mobj[i-1].addv,mtri[mobj[i].type][j].vert[1]+mobj[i-1].addv,mtri[mobj[i].type][j].vert[2]+mobj[i-1].addv);
              }
            }

            for(i=1;i<=nco;i++){
              for(j=1;j<=nrcl[mobj[i].type];j++){
                fprintf(fcol,"%d %d %d %d %d\r\n",mcol[mobj[i].type][j].start+mobj[i-1].addt,mcol[mobj[i].type][j].end+mobj[i-1].addt,mcol[mobj[i].type][j].red,mcol[mobj[i].type][j].green,mcol[mobj[i].type][j].blue);
              }

              for(j=1;j<=nrbc[mobj[i].type];j++){
                fprintf(fcld,"%d %d %c %1.4f %1.4f %1.4f\r\n",mbf[mobj[i].type][j].start+mobj[i-1].addt,mbf[mobj[i].type][j].end+mobj[i-1].addt,mbf[mobj[i].type][j].flag,mbf[mobj[i].type][j].x+mobj[i].tx,mbf[mobj[i].type][j].y+mobj[i].ty,mbf[mobj[i].type][j].z+mobj[i].tz);
              }
            }

            fclose(fgeo); fclose(fcol); fclose(fcld);
            break; /*objects read*/

    default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
  }
}

fclose(f1);

for(i=1;i<=nob;i++){free(mtri[i]);free(mvert[i]);free(mcol[i]);free(mbf[i]);}
free(mtri);free(mvert);free(mcol);free(mbf);
free(nrtr);free(nrv);free(nrcl);free(nrbc);
free(mobj);
free(mtritot);free(mverttot);
return 0;}
