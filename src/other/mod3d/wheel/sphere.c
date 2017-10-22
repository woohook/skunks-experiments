#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main()
{const float pi=3.1415926536;
int i,j,n;
float alf,tt; /*unghiuri*/
float raza,r,h; /*raza=raza sferei*/
int nod1[1001],nod2[1001],nod3[1001];
float x[1001],y[1001],z[1001];
int nrvert,nrfaces; /*numar de noduri, respectiv de triunghiuri*/
FILE *fis;
char numefis[21],numefiscl[21];

strcpy(numefis,"sphere.geo");
strcpy(numefiscl,"sphere.col");

printf("n: "); scanf("%d",&n);
printf("R: "); scanf("%f",&raza);

nrvert=0;

for(i=1;i<=(n-1);i++){
  alf=i*pi/n;
  h=raza*cos(alf);
  r=raza*sin(alf);
    for(j=1;j<=2*n;j++){
      nrvert++;
      tt=j*pi/n;
      x[nrvert]=h;
      y[nrvert]=r*sin(tt);
      z[nrvert]=r*cos(tt);
    }
}

nrfaces=0;

for(i=1;i<=(n-2);i++){
  for(j=1;j<=2*n;j++){
    nrfaces++;
    nod1[nrfaces]=(i-1)*2*n+j;
    nod2[nrfaces]=(i-1)*2*n+j+1;
    nod3[nrfaces]=i*2*n+j;

    if((j<2*n)||(i<(n-2))){nrfaces++;
    nod1[nrfaces]=i*2*n+j;
    nod2[nrfaces]=i*2*n+j+1;
    nod3[nrfaces]=(i-1)*2*n+j+1;}
  }
}
nrfaces++;
nod1[nrfaces]=1;
nod2[nrfaces]=2*n+1;
nod3[nrfaces]=2*n;

nrvert++;
x[nrvert]=raza;y[nrvert]=0;z[nrvert]=0;
for(i=1;i<=2*n-1;i++){
  nrfaces++;
  nod1[nrfaces]=nrvert;nod2[nrfaces]=i;nod3[nrfaces]=i+1;
}
nrfaces++;
nod1[nrfaces]=nrvert;nod2[nrfaces]=1;nod3[nrfaces]=2*n;

nrvert++;
x[nrvert]=-raza;y[nrvert]=0;z[nrvert]=0;
for(i=2;i<=2*n;i++){
  nrfaces++;
  nod1[nrfaces]=nrvert;nod2[nrfaces]=nrvert-i;nod3[nrfaces]=nrvert-i-1;
}
nrfaces++;
nod1[nrfaces]=nrvert;nod2[nrfaces]=nrvert-2;nod3[nrfaces]=nrvert-2*n-1;
/*generat sfera*/


if(!(fis=fopen(numefis,"w"))){printf("Could not open file '%s'\r\n",numefis);exit(1);}

fprintf(fis,"%d %d\r\n",nrvert,nrfaces);

for(i=1;i<=nrvert;i++){fprintf(fis,"%f %f %f\r\n",z[i],y[i],x[i]);} /*inversat z cu x*/
for(i=1;i<=nrfaces;i++){fprintf(fis,"f %d %d %d\r\n",nod1[i],nod2[i],nod3[i]);}

fclose(fis);

return 0;}
