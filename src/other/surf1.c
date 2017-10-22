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


float findx(float *a,float y,float z)
{
return (a[6]*y*y+a[5]*z*z+a[4]*y*z+a[3]*y+a[2]*z+a[1]);
}


void addf(FILE *f,int n1,int n2,int n3,int n4,int n5,int n6,int n7,int n8)
{fprintf(f,"f %d %d %d\r\nf %d %d %d\r\n",n1,n2,n6,n1,n5,n6);
 fprintf(f,"f %d %d %d\r\nf %d %d %d\r\n",n2,n3,n7,n2,n6,n7);
 fprintf(f,"f %d %d %d\r\nf %d %d %d\r\n",n3,n4,n8,n3,n7,n8);
 fprintf(f,"f %d %d %d\r\nf %d %d %d\r\n",n4,n1,n5,n4,n8,n5);
}


int main()
{int i,j,m,n;
float a[7],ly,lz,x[7],y[7],z[7],
      acs[37],
      yij,zij,
      xb,yb[17],zb[17];
FILE *f1;

printf("ly: "); scanf("%f",&ly);
printf("lz: "); scanf("%f",&lz);
printf("m: "); scanf("%d",&m);
printf("n: "); scanf("%d",&n);
for(i=1;i<=6;i++){printf("x%d: ",i); scanf("%f",&x[i]);}
printf("xb: "); scanf("%f",&xb);

if(m%2!=0){m+=1;}
if(n%2!=0){n+=1;}

y[1]=y[4]=-0.5*ly; y[2]=y[3]=0.5*ly; y[5]=y[6]=0.0;
z[1]=z[2]=z[5]=-0.5*lz; z[3]=z[4]=0.5*lz; z[6]=0.0;

for(i=1;i<=6;i++){
  acs[(i-1)*6+1]=1.0;
  acs[(i-1)*6+2]=z[i];
  acs[(i-1)*6+3]=y[i];
  acs[(i-1)*6+4]=y[i]*z[i];
  acs[(i-1)*6+5]=z[i]*z[i];
  acs[(i-1)*6+6]=y[i]*y[i];
}

/*for(i=1;i<=36;i++){acs[i]+=0.001*i;}*/


/*VERIFICARE*/
printf("Matrice coeficienti:\r\n");
for(i=1;i<=6;i++){
  for(j=1;j<=6;j++){
    printf("%8.3f ",acs[(i-1)*6+j]);
  } printf("\r\n");
}
printf("m = %d, n = %d\r\n",m,n);
/*VERIFICARE*/

sistem(acs,x,a,6);


if(!(f1=fopen("surf.geo","w"))){printf("Could not open 'surf.geo'\r\n"); exit(1);}

fprintf(f1,"%d %d\r\n",m*n+16,2*(m-1)*(n-1)-(m-2)*(n-2)/2+32);

for(i=1;i<=n;i++){
  for(j=1;j<=m;j++){
    yij=-0.5*ly+(j-1)*ly/(m-1);
    zij=-0.5*lz+(i-1)*lz/(n-1);
    if(j%2==0){yij-=0.3*ly/(m-1);}else{yij+=0.3*ly/(m-1);}
    if(i%2==0){zij-=0.3*lz/(n-1);}else{zij+=0.3*lz/(n-1);}
    fprintf(f1,"%1.3f %1.3f %1.3f\r\n",findx(a,yij,zij),yij,zij);
  }
}

i=1; zb[1]=zb[2]=zb[3]=zb[4]=-0.5*lz+(i-1)*lz/(n-1)+0.3*lz/(n-1);
i=2; zb[5]=zb[6]=zb[7]=zb[8]=-0.5*lz+(i-1)*lz/(n-1)-0.3*lz/(n-1);
i=n-1; zb[9]=zb[10]=zb[11]=zb[12]=-0.5*lz+(i-1)*lz/(n-1)+0.3*lz/(n-1);
i=n; zb[13]=zb[14]=zb[15]=zb[16]=-0.5*lz+(i-1)*lz/(n-1)-0.3*lz/(n-1);

j=1; yb[1]=yb[5]=yb[9]=yb[13]=-0.5*ly+(j-1)*ly/(m-1)+0.3*ly/(m-1);
j=2; yb[2]=yb[6]=yb[10]=yb[14]=-0.5*ly+(j-1)*ly/(m-1)-0.3*ly/(m-1);
j=m-1; yb[3]=yb[7]=yb[11]=yb[15]=-0.5*ly+(j-1)*ly/(m-1)+0.3*ly/(m-1);
j=m; yb[4]=yb[8]=yb[12]=yb[16]=-0.5*ly+(j-1)*ly/(m-1)-0.3*ly/(m-1);

for(i=1;i<=16;i++){fprintf(f1,"%1.3f %1.3f %1.3f\r\n",xb,yb[i],zb[i]);}

for(i=1;i<=(n-1);i++){
  for(j=1;j<=(m-1);j++){
    if((i%2!=0)||(j%2!=0)){
      fprintf(f1,"f %d %d %d\r\n",(i-1)*m+j,(i-1)*m+j+1,(i-1)*m+j+m);
      fprintf(f1,"f %d %d %d\r\n",(i-1)*m+j+1,(i-1)*m+j+m+1,(i-1)*m+j+m);
    }
  }
}

addf(f1,n*m+1,n*m+2,n*m+6,n*m+5,1,2,m+2,m+1);
addf(f1,n*m+3,n*m+4,n*m+8,n*m+7,m-1,m,2*m,2*m-1);
addf(f1,n*m+9,n*m+10,n*m+14,n*m+13,(n-2)*m+1,(n-2)*m+2,(n-1)*m+2,(n-1)*m+1);
addf(f1,n*m+11,n*m+12,n*m+16,n*m+15,(n-1)*m-1,(n-1)*m,n*m,n*m-1);

fclose(f1);


if(!(f1=fopen("surf.col","w"))){printf("Could not open 'surf.col'\r\n"); exit(1);}
fprintf(f1,"2\r\n");
fprintf(f1,"1 %d 170 10 10\r\n",2*(m-1)*(n-1)-(m-2)*(n-2)/2);
fprintf(f1,"%d %d 170 140 140\r\n",2*(m-1)*(n-1)-(m-2)*(n-2)/2+1,2*(m-1)*(n-1)-(m-2)*(n-2)/2+32);
fclose(f1);


if(!(f1=fopen("surf.cld","w"))){printf("Could not open 'surf.cld'\r\n"); exit(1);}
fprintf(f1,"0\r\n");
fclose(f1);


return 0;}
