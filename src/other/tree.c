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


float frand1()
{int a;
float b;
a=rand();
a=a%100;
b=0.0+0.01*a;
return b;}


int main()
{int i,n,nt,red,green,blue,red1,green1,blue1;
float h1,h2,dh,r,t,r0,tt0,x1,x2,x3,y1,y2,y3,z1,z2,z3;
FILE *f;

printf("h1: "); scanf("%f",&h1);
printf("h2: "); scanf("%f",&h2);
printf("dh: "); scanf("%f",&dh);
printf("r: "); scanf("%f",&r);
printf("t: "); scanf("%f",&t);
printf("n: "); scanf("%d",&n);
printf("red: "); scanf("%d",&red);
printf("green: "); scanf("%d",&green);
printf("blue: "); scanf("%d",&blue);

inrnd();

if(!(f=fopen("tree.geo","w"))){printf("Could not open 'tree.geo'\r\n"); exit(1);}

nt=7;
fprintf(f,"%d %d\r\n",3*n+2*nt,n+2*nt);

r0=0.25;
for(i=1;i<=nt;i++){
  x1=0; tt0=6.283*i/nt; y1=r0*cos(tt0); z1=r0*sin(tt0);
  fprintf(f,"%1.3f %1.3f %1.3f\r\n",x1,y1,z1);
}
for(i=1;i<=nt;i++){
  x1=h1+0.3*h2; tt0=6.283*i/nt; y1=r0*cos(tt0); z1=r0*sin(tt0);
  fprintf(f,"%1.3f %1.3f %1.3f\r\n",x1,y1,z1);
}

for(i=1;i<=n;i++){
  r0=r*frand1(); tt0=6.283*frand1();
  x1=h1+(h2-t)*frand1(); x2=x1+t; y1=y2=z1=z2=0.0;
  x3=0.5*(x1+x2)-0.5*dh+dh*frand1();
  y3=r0*cos(tt0); z3=r0*sin(tt0);
  fprintf(f,"%1.3f %1.3f %1.3f\r\n",x1,y1,z1);
  fprintf(f,"%1.3f %1.3f %1.3f\r\n",x2,y2,z2);
  fprintf(f,"%1.3f %1.3f %1.3f\r\n",x3,y3,z3);
}

for(i=1;i<=(nt-1);i++){
  fprintf(f,"f %d %d %d\r\nf %d %d %d\r\n",i,i+1,i+1+nt,i,i+nt,i+1+nt);
}
fprintf(f,"f %d %d %d\r\nf %d %d %d\r\n",nt,1,nt+1,nt,2*nt,nt+1);

for(i=1;i<=n;i++){
  fprintf(f,"f %d %d %d\r\n",2*nt+i*3-2,2*nt+i*3-1,2*nt+i*3);
}

fclose(f);


if(!(f=fopen("tree.col","w"))){printf("Could not open 'tree.col'\r\n"); exit(1);}

fprintf(f,"%d\r\n",1+n);
fprintf(f,"1 %d 100 70 40\r\n",2*nt);
for(i=1;i<=n;i++){
  red1=red+80*frand1();
  green1=green+80*frand1();
  blue1=blue+80*frand1();
  fprintf(f,"%d %d %d %d %d\r\n",2*nt+i,2*nt+i,red1,green1,blue1);
}

fclose(f);


if(!(f=fopen("tree.cld","w"))){printf("Could not open 'tree.cld'\r\n"); exit(1);}

fprintf(f,"1\r\n1 %d i 0.0 0.0 0.0\r\n",2*nt);

fclose(f);


return 0;}
