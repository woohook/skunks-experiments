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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main()
{int i,n;
float r,tt,w,t,s,c,d,rb,l,
      x0,y0,z0;
FILE *fis;

printf("Number of segments: "); scanf("%d",&n);
printf("Width: "); scanf("%f",&w);
printf("Radius: "); scanf("%f",&r);
t=1;

tt=6.283185/n; d=w/n;

if(!(fis=fopen("loop.geo","w"))){printf("Could not open file\r\n");exit(1);}

fprintf(fis,"%d  %d\r\n",4*n+4+8,8*n+4);

for(i=0;i<=n;i++){
  s=sin(i*tt); c=cos(i*tt);
  fprintf(fis,"%10.3f %10.3f %10.3f\r\n",r*(1-c),r*s,i*d+w/2);
  fprintf(fis,"%10.3f %10.3f %10.3f\r\n",r-(r+t)*c,(r+t)*s,i*d+w/2);
  fprintf(fis,"%10.3f %10.3f %10.3f\r\n",r-(r+t)*c,(r+t)*s,i*d-w/2);
  fprintf(fis,"%10.3f %10.3f %10.3f\r\n",r*(1-c),r*s,i*d-w/2);
}
fprintf(fis,"%10.3f %10.3f %10.3f\r\n",-1.5,0.0,-w/2);
fprintf(fis,"%10.3f %10.3f %10.3f\r\n",-1.5,0.0,-w/2-10);
fprintf(fis,"%10.3f %10.3f %10.3f\r\n",1.5,0.0,-w/2-10);
fprintf(fis,"%10.3f %10.3f %10.3f\r\n",1.5,0.0,-w/2);
fprintf(fis,"%10.3f %10.3f %10.3f\r\n",-1.5,0.0,3*w/2+10);
fprintf(fis,"%10.3f %10.3f %10.3f\r\n",-1.5,0.0,3*w/2);
fprintf(fis,"%10.3f %10.3f %10.3f\r\n",1.5,0.0,3*w/2);
fprintf(fis,"%10.3f %10.3f %10.3f\r\n",1.5,0.0,3*w/2+10);
 fprintf(fis,"\r\n");

for(i=0;i<=(n-1);i++){
  fprintf(fis,"f  %d  %d  %d\r\n",i*4+2,i*4+1,i*4+5);
  fprintf(fis,"f  %d  %d  %d\r\n",i*4+2,i*4+6,i*4+5);
  fprintf(fis,"f  %d  %d  %d\r\n",i*4+3,i*4+4,i*4+8);
  fprintf(fis,"f  %d  %d  %d\r\n",i*4+3,i*4+7,i*4+8);
  fprintf(fis,"f  %d  %d  %d\r\n",i*4+1,i*4+4,i*4+8);
  fprintf(fis,"f  %d  %d  %d\r\n",i*4+1,i*4+5,i*4+8);
  fprintf(fis,"f  %d  %d  %d\r\n",i*4+2,i*4+3,i*4+7);
  fprintf(fis,"f  %d  %d  %d\r\n",i*4+2,i*4+6,i*4+7);
}
fprintf(fis,"f  %d  %d  %d\r\n",4*n+4+2,4*n+4+1,4*n+4+4);
fprintf(fis,"f  %d  %d  %d\r\n",4*n+4+2,4*n+4+3,4*n+4+4);
fprintf(fis,"f  %d  %d  %d\r\n",4*n+4+6,4*n+4+5,4*n+4+8);
fprintf(fis,"f  %d  %d  %d\r\n",4*n+4+6,4*n+4+7,4*n+4+8);

fclose(fis);


rb=r*cos(tt/2)+t/2; l=0.96*2*r*sin(tt/2);

if(!(fis=fopen("loop.ref","w"))){printf("Could not open file\r\n");exit(1);}

fprintf(fis,"%d\r\n",n);

for(i=1;i<=n;i++){
  s=sin(tt*(i-0.5)); c=cos(tt*(i-0.5));
  x0=r-rb*c; y0=d*(i-0.5); z0=rb*s;
  fprintf(fis,"box %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f %8.3f\r\n",t,w,l,x0,z0,y0,x0+s,z0+c,y0);
}

fclose(fis);


if(!(fis=fopen("loop.cld","w"))){printf("Could not open file\r\n");exit(1);}

fprintf(fis,"%d\r\n",n);

for(i=1;i<=n;i++){
  s=sin(tt*(i-0.5)); c=cos(tt*(i-0.5));
  x0=r-rb*c; y0=d*(i-0.5); z0=rb*s;
  fprintf(fis,"%d %d i %8.3f %8.3f %8.3f\r\n",(i-1)*8+1,i*8,x0,z0,y0);
}

fclose(fis);


if(!(fis=fopen("loop.col","w"))){printf("Could not open file\r\n");exit(1);}

fprintf(fis,"5\r\n1  %d  40 40 200\r\n",8*n);
fprintf(fis,"%d  %d  230 230 130\r\n",8*n+1,8*n+1);
fprintf(fis,"%d  %d  170 30 30\r\n",8*n+2,8*n+2);
fprintf(fis,"%d  %d  230 230 130\r\n",8*n+3,8*n+3);
fprintf(fis,"%d  %d  170 30 30\r\n",8*n+4,8*n+4);

fclose(fis);

return 0;}
