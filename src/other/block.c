/*
Copyright (C) 2013 Victor Matei Petrescu

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
#include <math.h>

#define PI 3.1415927


int main()
{int i,j,k,n,nf,night=0;
float tt,tt0,h,hw,ry,rz,d,hk,ryk,rzk;
FILE *f;

printf("Ry: "); scanf("%f",&ry);
printf("Rz: "); scanf("%f",&rz);
printf("d: "); scanf("%f",&d);
printf("Start angle: PI/"); scanf("%d",&i); tt0=PI/i;
printf("Nr. of sides: "); scanf("%d",&n);
printf("Nr. of floors: "); scanf("%d",&nf); nf+=1;
printf("h: "); scanf("%f",&h);
printf("hw: "); scanf("%f",&hw);
printf("Night (0/1): "); scanf("%d",&night);

if(!(f=fopen("block.geo","w"))){printf("Could not open file 'block.geo'\r\n"); exit(1);}

fprintf(f,"%d %d\r\n",5*n*nf,8*n*nf);

for(j=1;j<=nf;j++){
  for(k=1;k<=5;k++){
    switch(k){
      case 1: hk=(j-1)*(h+hw); if(j==1){hk=-20;} ryk=ry; rzk=rz; break;
      case 2: hk=(j-1)*(h+hw); if(j==1){hk=-20;} ryk=ry+d; rzk=rz+d; break;
      case 3: hk=(j-1)*(h+hw)+h; ryk=ry+d; rzk=rz+d; break;
      case 4: hk=(j-1)*(h+hw)+h; ryk=ry; rzk=rz; break;
      case 5: hk=j*(h+hw); if(j==nf){hk=(j-1)*(h+hw)+1.1*h;} ryk=ry; rzk=rz; break;
      default: break;
    }
    tt=tt0;
    for(i=1;i<=n;i++){
      if((k==5)&&(j==nf)){fprintf(f,"%1.3f 0.0 0.0\r\n",hk);}
      else{fprintf(f,"%1.3f %1.3f %1.3f\r\n",hk,ryk*cos(tt),rzk*sin(tt));}
      tt+=2*PI/n;
    }
  }
}


for(j=1;j<=nf;j++){
  for(k=1;k<=4;k++){
    for(i=1;i<=(n-1);i++){
      fprintf(f,"f %d %d %d\r\n",(j-1)*5*n+(k-1)*n+i,(j-1)*5*n+k*n+i,(j-1)*5*n+k*n+i+1);
      fprintf(f,"f %d %d %d\r\n",(j-1)*5*n+(k-1)*n+i,(j-1)*5*n+(k-1)*n+i+1,(j-1)*5*n+k*n+i+1);
    }
    fprintf(f,"f %d %d %d\r\n",(j-1)*5*n+(k-1)*n+i,(j-1)*5*n+k*n+i,(j-1)*5*n+k*n+1);
    fprintf(f,"f %d %d %d\r\n",(j-1)*5*n+(k-1)*n+i,(j-1)*5*n+(k-1)*n+1,(j-1)*5*n+k*n+1);
  }
}

fclose(f);


if(!(f=fopen("block.col","w"))){printf("Could not open file 'block.col'\r\n"); exit(1);}

fprintf(f,"%d\r\n",2*nf);

for(j=1;j<=(nf-1);j++){
  fprintf(f,"%d %d 150 170 200\r\n",(j-1)*8*n+1,(j-1)*8*n+6*n);
  if(night){fprintf(f,"%d %d 255 255 200\r\n",(j-1)*8*n+6*n+1,(j-1)*8*n+8*n);}
  else{fprintf(f,"%d %d 20 20 70\r\n",(j-1)*8*n+6*n+1,(j-1)*8*n+8*n);}
}
  fprintf(f,"%d %d 150 170 200\r\n",(j-1)*8*n+1,(j-1)*8*n+6*n);
  fprintf(f,"%d %d 150 170 200\r\n",(j-1)*8*n+6*n+1,(j-1)*8*n+8*n);

if(night){
  fprintf(f,"fullbright %d\r\n",nf);
  for(j=1;j<=nf;j++){
    fprintf(f,"%d %d\r\n",(j-1)*8*n+6*n+1,(j-1)*8*n+8*n);
  }
}

fclose(f);


if(!(f=fopen("block.cld","w"))){printf("Could not open file 'block.cld'\r\n"); exit(1);}

fprintf(f,"%d\r\n",nf);

for(j=1;j<=nf;j++){
  fprintf(f,"%d %d i %1.3f %1.3f %1.3f\r\n",(j-1)*8*n+1,(j-1)*8*n+8*n,(j-1)*(h+hw)+0.5*h,0.0,0.0);
}

fclose(f);

return 0;}
