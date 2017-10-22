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


int main()
{int i,nw,nv,nt;
float x[2000],y[2000],z[2000],
      l1,l2,l3,h1,lw,t,sa,ca,ta,w1,zg,xg,lf,l4,l5,l6,l7,w2,w3,h2;
FILE *f1;

printf("l1: "); scanf("%f",&l1);
printf("l2: "); scanf("%f",&l2);
printf("l3: "); scanf("%f",&l3);
printf("h1: "); scanf("%f",&h1); /*height of roof*/
printf("h2: "); scanf("%f",&h2);
printf("t: "); scanf("%f",&t);
printf("nw: "); scanf("%d",&nw); /*number of windows on side*/
printf("w1: "); scanf("%f",&w1); /*width of roof*/
printf("zg: "); scanf("%f",&zg);
printf("xg: "); scanf("%f",&xg);
printf("lf: "); scanf("%f",&lf);
printf("l4: "); scanf("%f",&l4);
printf("l5: "); scanf("%f",&l5);
printf("l6: "); scanf("%f",&l6);
printf("l7: "); scanf("%f",&l7);
printf("w2: "); scanf("%f",&w2);
printf("w3: "); scanf("%f",&w3);


for(i=1;i<=(4*nw+4);i++){y[i]=-0.5*w1;}
for(i=4*nw+5;i<=(8*nw+8);i++){y[i]=0.5*w1;}

x[1]=h2-xg; z[1]=l4+l5+l6+l7-zg-lf;

x[2]=x[1]; x[4]=x[3]=x[1]+h1;
for(i=1;i<=nw;i++){
  x[4*i+1]=x[4*i+2]=x[1]+t;
  x[4*i+3]=x[4*i+4]=x[4]-t;
}
for(i=1;i<=(4*nw+4);i++){x[4*nw+4+i]=x[i];}

z[4]=z[1]-l1; z[3]=z[4]-l2; z[2]=z[3]-l3;
ta=h1/l1;
z[5]=z[1]-(ta/(sqrt(1+ta*ta)-1))*t;
z[8]=z[4]-((sqrt(1+ta*ta)-1)/ta)*t;
ta=h1/l3;
z[4*nw+2]=z[2]+(ta/(sqrt(1+ta*ta)-1))*t;
z[4*nw+3]=z[3]+((sqrt(1+ta*ta)-1)/ta)*t;
lw=(z[8]-z[4*nw+3]-(nw-1)*t)/nw;
z[6]=z[7]=z[8]-lw;
z[4*nw+1]=z[4*nw+4]=z[4*nw+3]+lw;
for(i=2;i<=(nw-1);i++){
  z[4*i+1]=z[4*i+4]=z[4*i-1]-t;
  z[4*i+2]=z[4*i+3]=z[4*i+1]-lw;
}
for(i=1;i<=(4*nw+4);i++){z[4*nw+4+i]=z[i];}

y[8*nw+9]=y[8*nw+10]=y[8*nw+13]=y[8*nw+14]=-0.5*w1+t;
y[8*nw+12]=y[8*nw+11]=y[8*nw+16]=y[8*nw+15]=0.5*w1-t;
sa=h1/(sqrt(h1*h1+l1*l1));
ca=l1/(sqrt(h1*h1+l1*l1));
z[8*nw+9]=z[8*nw+12]=z[1]-t*ca;
x[8*nw+9]=x[8*nw+12]=x[1]+t*sa;
z[8*nw+10]=z[8*nw+11]=z[4]+t*ca;
x[8*nw+10]=x[8*nw+11]=x[4]-t*sa;
sa=h1/(sqrt(h1*h1+l3*l3));
ca=l3/(sqrt(h1*h1+l3*l3));
z[8*nw+14]=z[8*nw+15]=z[2]+t*ca;
x[8*nw+14]=x[8*nw+15]=x[2]+t*sa;
z[8*nw+13]=z[8*nw+16]=z[3]-t*ca;
x[8*nw+13]=x[8*nw+16]=x[3]-t*sa;

for(i=8*nw+17;i<=8*nw+29;i++){x[i]=h2-xg;}
for(i=8*nw+30;i<=8*nw+42;i++){x[i]=-xg;}
y[8*nw+17]=y[8*nw+30]=y[8*nw+18]=y[8*nw+31]=-0.5*w3;
y[8*nw+24]=y[8*nw+37]=y[8*nw+23]=y[8*nw+36]=0.5*w3;
y[8*nw+28]=y[8*nw+41]=y[8*nw+29]=y[8*nw+42]=y[8*nw+19]=y[8*nw+32]=y[8*nw+20]=y[8*nw+33]=-0.5*w2;
y[8*nw+26]=y[8*nw+39]=y[8*nw+25]=y[8*nw+38]=y[8*nw+22]=y[8*nw+35]=y[8*nw+21]=y[8*nw+34]=0.5*w2;
y[8*nw+27]=y[8*nw+40]=0.0;
z[8*nw+20]=z[8*nw+33]=z[8*nw+21]=z[8*nw+34]=-zg;
z[8*nw+18]=z[8*nw+31]=z[8*nw+19]=z[8*nw+32]=z[8*nw+22]=z[8*nw+35]=z[8*nw+23]=z[8*nw+36]=-zg+l7;
z[8*nw+17]=z[8*nw+30]=z[8*nw+29]=z[8*nw+42]=z[8*nw+25]=z[8*nw+38]=z[8*nw+24]=z[8*nw+37]=-zg+l7+l6;
z[8*nw+28]=z[8*nw+41]=z[8*nw+26]=z[8*nw+39]=-zg+l7+l6+l5;
z[8*nw+27]=z[8*nw+40]=-zg+l7+l6+l5+l4;

x[8*nw+43]=x[8*nw+44]=x[8*nw+47]=x[8*nw+48]=x[8*nw+51]=x[8*nw+52]=x[8*nw+55]=x[8*nw+56]=0.5*h2-xg-0.25*h2;
x[8*nw+46]=x[8*nw+45]=x[8*nw+50]=x[8*nw+49]=x[8*nw+54]=x[8*nw+53]=x[8*nw+58]=x[8*nw+57]=0.5*h2-xg+0.25*h2;
y[8*nw+43]=y[8*nw+46]=y[8*nw+47]=y[8*nw+50]=0.25*w2+0.1*w2;
y[8*nw+44]=y[8*nw+45]=y[8*nw+48]=y[8*nw+49]=0.25*w2-0.1*w2;
y[8*nw+51]=y[8*nw+54]=y[8*nw+55]=y[8*nw+58]=-0.25*w2+0.1*w2;
y[8*nw+52]=y[8*nw+53]=y[8*nw+56]=y[8*nw+57]=-0.25*w2-0.1*w2;
for(i=8*nw+43;i<=(8*nw+46);i++){z[i]=z[i+8]=l4+l5+l6+l7-zg+0.01;}
for(i=8*nw+47;i<=(8*nw+50);i++){z[i]=z[i+8]=l5+l6+l7-zg;}

x[8*nw+59]=x[8*nw+60]=x[8*nw+63]=x[8*nw+64]=x[8*nw+67]=x[8*nw+68]=x[8*nw+71]=x[8*nw+72]=0.5*h2-xg-0.1*h2;
x[8*nw+62]=x[8*nw+61]=x[8*nw+66]=x[8*nw+65]=x[8*nw+70]=x[8*nw+69]=x[8*nw+74]=x[8*nw+73]=0.5*h2-xg+0.1*h2;
y[8*nw+59]=y[8*nw+62]=y[8*nw+63]=y[8*nw+66]=0.25*w2+0.1*w2;
y[8*nw+60]=y[8*nw+61]=y[8*nw+64]=y[8*nw+65]=0.25*w2-0.1*w2;
y[8*nw+67]=y[8*nw+70]=y[8*nw+71]=y[8*nw+74]=-0.25*w2+0.1*w2;
y[8*nw+68]=y[8*nw+69]=y[8*nw+72]=y[8*nw+73]=-0.25*w2-0.1*w2;
for(i=8*nw+59;i<=(8*nw+62);i++){z[i]=z[i+8]=-zg-0.03;}
for(i=8*nw+63;i<=(8*nw+66);i++){z[i]=z[i+8]=-zg;}

nv=8*nw+74; /*nr. of vertices*/
nt=12+4*nw+18+4*nw+4+33+7+20+20; /*nr. of triangles*/


if(!(f1=fopen("car.geo","w"))){printf("Could not open 'car.geo'\r\n"); exit(1);}

fprintf(f1,"%d %d\r\n",nv,nt);

for(i=1;i<=nv;i++){
  fprintf(f1,"%1.3f %1.3f %1.3f\r\n",x[i],y[i],z[i]);
}

fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",1,4,8,1,5,8);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",1,2,4*nw+2,1,5,4*nw+2);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",2,3,4*nw+3,2,4*nw+2,4*nw+3);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4,3,4*nw+3,4,8,4*nw+3);
for(i=2;i<=nw;i++){
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*i-2,4*i+1,4*i+4,4*i-2,4*i-1,4*i+4);
}
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*nw+4+1,4*nw+4+4,4*nw+4+8,4*nw+4+1,4*nw+4+5,4*nw+4+8);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*nw+4+1,4*nw+4+2,4*nw+4+4*nw+2,4*nw+4+1,4*nw+4+5,4*nw+4+4*nw+2);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*nw+4+2,4*nw+4+3,4*nw+4+4*nw+3,4*nw+4+2,4*nw+4+4*nw+2,4*nw+4+4*nw+3);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*nw+4+4,4*nw+4+3,4*nw+4+4*nw+3,4*nw+4+4,4*nw+4+8,4*nw+4+4*nw+3);
for(i=2;i<=nw;i++){
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*nw+4+4*i-2,4*nw+4+4*i+1,4*nw+4+4*i+4,4*nw+4+4*i-2,4*nw+4+4*i-1,4*nw+4+4*i+4);
}

fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",1,4,8*nw+10,1,8*nw+9,8*nw+10);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",1,4*nw+5,8*nw+12,1,8*nw+9,8*nw+12);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*nw+5,4*nw+8,8*nw+11,4*nw+5,8*nw+12,8*nw+11);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4,4*nw+8,8*nw+11,4,8*nw+10,8*nw+11);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",3,2,8*nw+14,3,8*nw+13,8*nw+14);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",3,4*nw+7,8*nw+16,3,8*nw+13,8*nw+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*nw+6,4*nw+7,8*nw+16,4*nw+6,8*nw+15,8*nw+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",2,4*nw+6,8*nw+15,2,8*nw+14,8*nw+15);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",3,4,4*nw+8,3,4*nw+7,4*nw+8);

for(i=1;i<=nw;i++){ /*windows*/
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*i+1,4*i+2,4*i+3,4*i+1,4*i+4,4*i+3);
}
for(i=nw+2;i<=2*nw+1;i++){
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",4*i+1,4*i+2,4*i+3,4*i+1,4*i+4,4*i+3);
}
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+9,8*nw+10,8*nw+11,8*nw+9,8*nw+12,8*nw+11);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+13,8*nw+14,8*nw+15,8*nw+13,8*nw+16,8*nw+15);

fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+17,8*nw+18,8*nw+23,8*nw+17,8*nw+24,8*nw+23);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+28,8*nw+29,8*nw+25,8*nw+28,8*nw+26,8*nw+25);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+19,8*nw+20,8*nw+21,8*nw+19,8*nw+22,8*nw+21);
fprintf(f1,"f %d %d %d\r\n",8*nw+26,8*nw+27,8*nw+28);

fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+17,8*nw+18,8*nw+31,8*nw+17,8*nw+30,8*nw+31);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+18,8*nw+19,8*nw+32,8*nw+18,8*nw+31,8*nw+32);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+19,8*nw+20,8*nw+33,8*nw+19,8*nw+32,8*nw+33);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+20,8*nw+21,8*nw+34,8*nw+20,8*nw+33,8*nw+34);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+21,8*nw+22,8*nw+35,8*nw+21,8*nw+34,8*nw+35);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+22,8*nw+23,8*nw+36,8*nw+22,8*nw+35,8*nw+36);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+23,8*nw+24,8*nw+37,8*nw+23,8*nw+36,8*nw+37);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+24,8*nw+25,8*nw+38,8*nw+24,8*nw+37,8*nw+38);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+25,8*nw+26,8*nw+39,8*nw+25,8*nw+38,8*nw+39);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+26,8*nw+27,8*nw+40,8*nw+26,8*nw+39,8*nw+40);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+27,8*nw+28,8*nw+41,8*nw+27,8*nw+40,8*nw+41);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+28,8*nw+29,8*nw+42,8*nw+28,8*nw+41,8*nw+42);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+29,8*nw+17,8*nw+30,8*nw+29,8*nw+42,8*nw+30);

fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+30,8*nw+31,8*nw+36,8*nw+30,8*nw+37,8*nw+36);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+41,8*nw+42,8*nw+38,8*nw+41,8*nw+39,8*nw+38);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+32,8*nw+33,8*nw+34,8*nw+32,8*nw+35,8*nw+34);
fprintf(f1,"f %d %d %d\r\n",8*nw+39,8*nw+40,8*nw+41);

fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+43,8*nw+44,8*nw+45,8*nw+43,8*nw+46,8*nw+45);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+43,8*nw+46,8*nw+50,8*nw+43,8*nw+47,8*nw+50);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+46,8*nw+45,8*nw+49,8*nw+46,8*nw+50,8*nw+49);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+44,8*nw+45,8*nw+49,8*nw+44,8*nw+48,8*nw+49);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+43,8*nw+44,8*nw+48,8*nw+43,8*nw+47,8*nw+48);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+51,8*nw+52,8*nw+53,8*nw+51,8*nw+54,8*nw+53);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+51,8*nw+54,8*nw+58,8*nw+51,8*nw+55,8*nw+58);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+54,8*nw+53,8*nw+57,8*nw+54,8*nw+58,8*nw+57);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+52,8*nw+53,8*nw+57,8*nw+52,8*nw+56,8*nw+57);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+51,8*nw+55,8*nw+56,8*nw+51,8*nw+52,8*nw+56);

fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+43+16,8*nw+44+16,8*nw+45+16,8*nw+43+16,8*nw+46+16,8*nw+45+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+43+16,8*nw+46+16,8*nw+50+16,8*nw+43+16,8*nw+47+16,8*nw+50+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+46+16,8*nw+45+16,8*nw+49+16,8*nw+46+16,8*nw+50+16,8*nw+49+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+44+16,8*nw+45+16,8*nw+49+16,8*nw+44+16,8*nw+48+16,8*nw+49+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+43+16,8*nw+44+16,8*nw+48+16,8*nw+43+16,8*nw+47+16,8*nw+48+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+51+16,8*nw+52+16,8*nw+53+16,8*nw+51+16,8*nw+54+16,8*nw+53+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+51+16,8*nw+54+16,8*nw+58+16,8*nw+51+16,8*nw+55+16,8*nw+58+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+54+16,8*nw+53+16,8*nw+57+16,8*nw+54+16,8*nw+58+16,8*nw+57+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+52+16,8*nw+53+16,8*nw+57+16,8*nw+52+16,8*nw+56+16,8*nw+57+16);
fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",8*nw+51+16,8*nw+55+16,8*nw+56+16,8*nw+51+16,8*nw+52+16,8*nw+56+16);

fclose(f1);

if(!(f1=fopen("car.col","w"))){printf("Could not open 'car.col'\r\n"); exit(1);}

fprintf(f1,"6\r\n");
fprintf(f1,"1 %d 210 80 50\r\n",12+4*nw+18); /*roof*/
fprintf(f1,"%d %d 50 100 250\r\n",12+4*nw+18+1,12+4*nw+18+4*nw+4); /*windows*/
fprintf(f1,"%d %d 250 100 50\r\n",12+4*nw+18+4*nw+5,12+4*nw+18+4*nw+4+33);
fprintf(f1,"%d %d 70 70 70\r\n",12+4*nw+18+4*nw+4+34,12+4*nw+18+4*nw+4+33+7);
fprintf(f1,"%d %d 255 255 230\r\n",12+4*nw+18+4*nw+4+33+8,12+4*nw+18+4*nw+4+33+7+20);
fprintf(f1,"%d %d 255 150 0\r\n",12+4*nw+18+4*nw+4+33+7+21,12+4*nw+18+4*nw+4+33+7+20+20);

fclose(f1);

if(!(f1=fopen("car.cld","w"))){printf("Could not open 'car.cld'\r\n"); exit(1);}

fprintf(f1,"6\r\n");
fprintf(f1,"%d %d i %1.3f %1.3f %1.3f\r\n",12+4*nw+18+1,12+4*nw+18+4*nw+4,h2-xg+0.5*h1,0.0,l4+l5+l6+l7-zg-lf-l1-0.5*l2); /*windows*/
fprintf(f1,"%d %d i %1.3f %1.3f %1.3f\r\n",12+4*nw+18+4*nw+5,12+4*nw+18+4*nw+4+33+7,0.5*h2-xg,0.0,l7+0.5*l6-zg);
fprintf(f1,"%d %d i %1.3f %1.3f %1.3f\r\n",12+4*nw+18+4*nw+4+33+8,12+4*nw+18+4*nw+4+33+7+10,0.5*h2-xg,0.25*w2,0.0);
fprintf(f1,"%d %d i %1.3f %1.3f %1.3f\r\n",12+4*nw+18+4*nw+4+33+7+11,12+4*nw+18+4*nw+4+33+7+20,0.5*h2-xg,-0.25*w2,0.0);
fprintf(f1,"%d %d i %1.3f %1.3f %1.3f\r\n",12+4*nw+18+4*nw+4+33+7+21,12+4*nw+18+4*nw+4+33+7+20+10,0.5*h2-xg,0.25*w2,0.0);
fprintf(f1,"%d %d i %1.3f %1.3f %1.3f\r\n",12+4*nw+18+4*nw+4+33+7+20+11,12+4*nw+18+4*nw+4+33+7+20+20,0.5*h2-xg,-0.25*w2,0.0);
fclose(f1);

return 0;}
