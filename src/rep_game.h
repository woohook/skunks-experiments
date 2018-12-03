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


/*run 1 simulation step; tstep - time step; af, bf - acceleration and brake factors*/
void runsim(sgob** objs,vhc *car,FILE *repf,REALN *timp,REALN *speed,int ff)
{int i,k,cnob;
REALN tmp,rap,spd[3];
static REALN rtim1=0,rtim2=0,*vx1,*vx2,*vy1,*vy2,*vz1,*vz2;
static int sem=0;

cnob=car->nob;

if(sem==0){
  if(!(vx1=(REALN *)malloc((cnob+1)*4*sizeof(REALN)))){printf("Out of memory");}
  if(!(vy1=(REALN *)malloc((cnob+1)*4*sizeof(REALN)))){printf("Out of memory");}
  if(!(vz1=(REALN *)malloc((cnob+1)*4*sizeof(REALN)))){printf("Out of memory");}
  if(!(vx2=(REALN *)malloc((cnob+1)*4*sizeof(REALN)))){printf("Out of memory");}
  if(!(vy2=(REALN *)malloc((cnob+1)*4*sizeof(REALN)))){printf("Out of memory");}
  if(!(vz2=(REALN *)malloc((cnob+1)*4*sizeof(REALN)))){printf("Out of memory");}
  for(i=0;i<=((cnob+1)*4-1);i++){
    vx1[i]=0; vy1[i]=0; vz1[i]=0; vx2[i]=0; vy2[i]=0; vz2[i]=0;
  }
  sem=1;
}

if(rtim2<(*timp)){
rtim1=rtim2;

  for(i=1;i<=car->nob;i++){
    for(k=0;k<=3;k++){
      vx1[(i-1)*4+k]=vx2[(i-1)*4+k];
      vy1[(i-1)*4+k]=vy2[(i-1)*4+k];
      vz1[(i-1)*4+k]=vz2[(i-1)*4+k];
    }
  }
}

while((rtim2<(*timp))&&(!feof(repf))){

  fscanf(repf,"%f",&rtim2);

    for(i=1;i<=car->nob;i++){
      for(k=0;k<=3;k++){
        fscanf(repf,"%f",&tmp); vx2[(i-1)*4+k]=tmp;
        fscanf(repf,"%f",&tmp); vy2[(i-1)*4+k]=tmp;
        fscanf(repf,"%f",&tmp); vz2[(i-1)*4+k]=tmp;
      }
    }
}

rap=(*timp-rtim1)/(rtim2-rtim1);

for(i=1;i<=car->nob;i++){
  sgob* object = objs[car->oid[i]];
  object->transform.vx[0]=vx1[(i-1)*4+0]+rap*(vx2[(i-1)*4+0]-vx1[(i-1)*4+0]);
  object->transform.vy[0]=vy1[(i-1)*4+0]+rap*(vy2[(i-1)*4+0]-vy1[(i-1)*4+0]);
  object->transform.vz[0]=vz1[(i-1)*4+0]+rap*(vz2[(i-1)*4+0]-vz1[(i-1)*4+0]);
    if((*timp-rtim1)>(rtim2-*timp)){
      for(k=1;k<=3;k++){
        object->transform.vx[k]=object->transform.vx[0]+vx2[(i-1)*4+k]-vx2[(i-1)*4+0];
        object->transform.vy[k]=object->transform.vy[0]+vy2[(i-1)*4+k]-vy2[(i-1)*4+0];
        object->transform.vz[k]=object->transform.vz[0]+vz2[(i-1)*4+k]-vz2[(i-1)*4+0];
      }
    }else{
      for(k=1;k<=3;k++){
        object->transform.vx[k]=object->transform.vx[0]+vx1[(i-1)*4+k]-vx1[(i-1)*4+0];
        object->transform.vy[k]=object->transform.vy[0]+vy1[(i-1)*4+k]-vy1[(i-1)*4+0];
        object->transform.vz[k]=object->transform.vz[0]+vz1[(i-1)*4+k]-vz1[(i-1)*4+0];
      }
    }
  }

if((rtim2-rtim1)<1e-5){
  spd[0]=spd[1]=spd[2]=0;
}else{
  spd[0]=(vx2[0]-vx1[0])/(rtim2-rtim1);
  spd[1]=(vy2[0]-vy1[0])/(rtim2-rtim1);
  spd[2]=(vz2[0]-vz1[0])/(rtim2-rtim1);
}
*speed=sqrt(spd[0]*spd[0]+spd[1]*spd[1]+spd[2]*spd[2]);

if(ff==1){
  free(vx1); free(vy1); free(vz1);
  free(vx2); free(vy2); free(vz2);
}

}
