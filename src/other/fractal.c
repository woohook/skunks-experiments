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
#include <time.h>


/*initialize random numbers*/
void inrnd()
{int i,t,r;
t=time(NULL); if(t<0){t=-t;} t=t%16384;
for(i=1;i<=t;i++){
  r=rand();
}
}


int inpow2(int p)
{int i,a=1;
for(i=1;i<=p;i++){
  a*=2;
}
return a;}


float frand1()
{int a;
float b;
a=rand();
a=a%100;
b=-1.0+0.02*a;
return b;}


int main()
{int i,j,k,m,n,n1,step,st2,
     red,green,blue;
long int area;
float height,width,length,rnf,*map;
FILE *fis;

printf("Size: 2^"); scanf("%d",&m);
printf("Height: "); scanf("%f",&height);
printf("Width: "); scanf("%f",&width);
printf("Length: "); scanf("%f",&length);
printf("Random factor: "); scanf("%f",&rnf);
printf("Red: "); scanf("%d",&red);
printf("Green: "); scanf("%d",&green);
printf("Blue: "); scanf("%d",&blue);

inrnd();

n=inpow2(m); area=(n+1)*(n+1);

if(!(map=(float *)malloc(area*sizeof(float)))){printf("Out of memory"); exit(1);}

n1=n+1;

for(i=0;i<=n;i+=(n/2)){
  for(j=0;j<=n;j+=(n/2)){
    map[i*n1+j]=0;
  }
} map[(n/2)*n1+n/2]=height;


for(k=2;k<=m;k++){
  step=n/inpow2(k);
  st2=2*step;
  for(i=0;i<=n;i+=step){
    for(j=step-i%st2;j<=n;j+=(st2-i%st2)){
      if((i%st2)&&(j%st2)){
        map[i*n1+j]=0.25*(map[(i-step)*n1+(j-step)]+map[(i-step)*n1+(j+step)]+map[(i+step)*n1+(j-step)]+map[(i+step)*n1+(j+step)]);
        map[i*n1+j]+=rnf*(((float)step)/n)*frand1();
      }else{
        if(i%st2){
          map[i*n1+j]=0.5*(map[(i-step)*n1+j]+map[(i+step)*n1+j]);
          if((j!=0)&&(j!=n)){map[i*n1+j]+=rnf*(((float)step)/n)*frand1();}
        }else{
          if(j%st2){
            map[i*n1+j]=0.5*(map[i*n1+(j-step)]+map[i*n1+(j+step)]);
            if((i!=0)&&(i!=n)){map[i*n1+j]+=rnf*(((float)step)/n)*frand1();}
          }
        }
      }
    }
  }
}


if(!(fis=fopen("map.geo","w"))){printf("Could not open 'map.geo'\r\n"); exit(1);}

fprintf(fis,"%d %d\r\n",n1*n1,2*n*n);

for(i=0;i<=n;i++){
  for(j=0;j<=n;j++){
    fprintf(fis,"%8.3f %8.3f %8.3f\r\n",map[i*n1+j],(width/n)*i-width/2,(length/n)*j-length/2);
  }
}

for(i=0;i<=n-1;i++){
  for(j=0;j<=n-1;j++){
    k=i*n1+j+1;
    fprintf(fis,"f %d %d %d\r\n",k,k+1,k+n1);
    fprintf(fis,"f %d %d %d\r\n",k+1,k+n1,k+n1+1);
  }
}

fclose(fis);

if(!(fis=fopen("map.col","w"))){printf("Could not open 'map.col'\r\n"); exit(1);}

fprintf(fis,"%d\r\n",2*n*n);

for(i=1;i<=2*n*n;i++){
  k=red+(int)(frand1()*10); if(k<0){k=0;} if(k>255){k=255;}
  j=green+(int)(frand1()*10); if(j<0){j=0;} if(j>255){j=255;}
  m=blue+(int)(frand1()*10); if(m<0){m=0;} if(m>255){m=255;}
  fprintf(fis,"%d %d %d %d %d\r\n",i,i,k,j,m);
}

fclose(fis);


if(!(fis=fopen("map.cld","w"))){printf("Could not open 'map.cld'\r\n"); exit(1);}

fprintf(fis,"%d\r\n",1);
fprintf(fis,"%d %d i -10000.0 %1.3f %1.3f\r\n",1,2*n*n,0.0,0.0);

fclose(fis);


free(map);
return 0;}
