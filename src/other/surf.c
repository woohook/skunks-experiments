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


/*rezolva un sistem de ecuatii (Ax=b) prin metoda directa*/
/*ac - matrice coeficienti A
  bc - vector liber b
  x - vector necunoscut
  n - numar de ecuatii*/
float sistem(float *ac,float *bc,float *x,int n){
int i,j,k,imax,area,itmp,
    *xin; /*noii indici ai necunoscutelor (x)*/
float max,rap,det=1,tmp,
       *a,*b;

area=n*n;
if(!(a=(float *)malloc((area+1)*sizeof(float)))){printf("Out of memory");return 0;}
if(!(b=(float *)malloc((n+1)*sizeof(float)))){printf("Out of memory");return 0;}
if(!(xin=(int *)malloc((n+1)*sizeof(int)))){printf("Out of memory");return 0;}

/*Copiaza variabilele de intrare, ca sa nu se modifice cele originale*/
for(i=1;i<=n;i++){b[i]=bc[i]; xin[i]=i;}
for(i=1;i<=area;i++){a[i]=ac[i];}


for(i=1;i<=(n-1);i++){max=fabs(a[(i-1)*n+i]);imax=i;
  for(k=i+1;k<=n;k++){
    if(fabs(a[(i-1)*n+k])>max){max=fabs(a[(i-1)*n+k]);imax=k;}}
    if(imax!=i){
      for(j=1;j<=n;j++){
        tmp=a[(j-1)*n+i]; a[(j-1)*n+i]=a[(j-1)*n+imax]; a[(j-1)*n+imax]=tmp;
      } itmp=xin[i]; xin[i]=xin[imax]; xin[imax]=itmp; /*schimbat coloana i cu coloana imax*/
    }

  for(k=i+1;k<=n;k++){rap=a[(k-1)*n+i]/a[(i-1)*n+i]; b[k]=b[k]-b[i]*rap;
    for(j=i+1;j<=n;j++){a[(k-1)*n+j]=a[(k-1)*n+j]-a[(i-1)*n+j]*rap;}}
}

for(i=n;i>=2;i--){
  for(k=i-1;k>=1;k--){rap=a[(k-1)*n+i]/a[(i-1)*n+i]; b[k]=b[k]-b[i]*rap;}
    x[xin[i]]=b[i]/a[(i-1)*n+i];}
    x[xin[1]]=b[1]/a[1];
  /*calculeaza determinantul matricei coeficientilor*/
  /*for(i=1;i<=n;i++){det=det*a[(i-1)*n+i];}*/

  free(a);free(b);free(xin);
/*returneaza determinantul matricei coeficientilor*/
return det;}


float vf1(float *a,float x,float y,float z)
{return (a[10]*x*x+a[9]*y*y+a[8]*z*z+a[7]*y*z+a[6]*x*z+a[5]*x*y);}


float vf(float *a,float x,float y,float z)
{return (vf1(a,x,y,z)+a[4]*x+a[3]*y+a[2]*z+a[1]);}


float findx(float *a,float y,float z)
{float a1,b1,c1,dt;
a1=a[10];
b1=a[6]*z+a[5]*y+a[4];
c1=a[9]*y*y+a[8]*z*z+a[7]*y*z+a[3]*y+a[2]*z+a[1];
dt=b1*b1-4*a1*c1;
if(dt<0){printf("findx(): no solution\r\n"); exit(1);}

return ((-b1+sqrt(dt))/(2*a1));
}


int main()
{int i,j,m,n;
float a[11],ly,lz,x1,x2,x3,x4,
      acs[17],rhs[5],xc[5],
      yij,zij;
FILE *f1;

printf("ly: "); scanf("%f",&ly);
printf("lz: "); scanf("%f",&lz);
printf("m: "); scanf("%d",&m);
printf("n: "); scanf("%d",&n);
printf("x1: "); scanf("%f",&x1);
printf("x2: "); scanf("%f",&x2);
printf("x3: "); scanf("%f",&x3);
printf("x4: "); scanf("%f",&x4);
printf("-----\r\n");
for(i=10;i>=5;i--){printf("a%d: ",i); scanf("%f",&a[i]);}

acs[1]=x1; acs[5]=x2; acs[9]=x3; acs[13]=x4;
acs[2]=acs[14]=-0.5*ly;
acs[6]=acs[10]=0.5*ly;
acs[3]=acs[7]=-0.5*lz;
acs[11]=acs[15]=0.5*lz;
acs[4]=acs[8]=acs[12]=acs[16]=1.0;

rhs[1]=-vf1(a,x1,-0.5*ly,-0.5*lz);
rhs[2]=-vf1(a,x2,0.5*ly,-0.5*lz);
rhs[3]=-vf1(a,x3,0.5*ly,0.5*lz);
rhs[4]=-vf1(a,x4,-0.5*ly,0.5*lz);

sistem(acs,rhs,xc,4);

a[4]=xc[1]; a[3]=xc[2]; a[2]=xc[3]; a[1]=xc[4];

if(!(f1=fopen("surf.geo","w"))){printf("Could not open 'surf.geo'\r\n"); exit(1);}

fprintf(f1,"%d %d\r\n",m*n,2*(m-1)*(n-1));

for(i=1;i<=n;i++){
  for(j=1;j<=m;j++){
    yij=-0.5*ly+(j-1)*ly/(m-1);
    zij=-0.5*lz+(i-1)*lz/(n-1);
    fprintf(f1,"%1.3f %1.3f %1.3f\r\n",findx(a,yij,zij),yij,zij);
  }
}

for(i=1;i<=(n-1);i++){
  for(j=1;j<=(m-1);j++){
    fprintf(f1,"f %d %d %d\r\n",(i-1)*m+j,(i-1)*m+j+1,(i-1)*m+j+m);
    fprintf(f1,"f %d %d %d\r\n",(i-1)*m+j+1,(i-1)*m+j+m+1,(i-1)*m+j+m);
  }
}

fclose(f1);

return 0;}
