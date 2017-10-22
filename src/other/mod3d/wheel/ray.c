#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define PI 3.1415927


int main()
{int i,n;
float r0,r2,d1,d2,w,v,
      x[1001],y[1001],z[1001],
      alf0,s,c;
FILE *f1;

printf("R2: "); scanf("%f",&r2);
printf("w: "); scanf("%f",&w); v=0.5*w;
printf("d1: "); scanf("%f",&d1);
printf("d2: "); scanf("%f",&d2);
printf("n: "); scanf("%d",&n);

alf0=2.0*PI/n;
r0=w*cos(0.5*alf0)/sin(alf0);

for(i=1;i<=n;i++){
  s=sin(alf0*i); c=cos(alf0*i);
  x[i]=r0*s-v*c; y[i]=r0*c+v*s; z[i]=-0.5*d1;
  x[n+i]=r2*s-v*c; y[n+i]=r2*c+v*s; z[n+i]=-0.5*d2;
  x[2*n+i]=r2*s+v*c; y[2*n+i]=r2*c-v*s; z[2*n+i]=-0.5*d2;
}

if(!(f1=fopen("ray.geo","w"))){printf("Could not open 'ray.geo'\r\n"); exit(1);}

fprintf(f1,"%d %d\r\n",6*n,6*n);

for(i=1;i<=3*n;i++){
  fprintf(f1,"%1.3f %1.3f %1.3f\r\n",x[i],y[i],z[i]);
}
for(i=1;i<=3*n;i++){
  fprintf(f1,"%1.3f %1.3f %1.3f\r\n",x[i],y[i],-z[i]);
}


for(i=1;i<=n;i++){
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",i,i%n+1,2*n+i,i,n+i,2*n+i);
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",3*n+i,3*n+i%n+1,3*n+2*n+i,3*n+i,3*n+n+i,3*n+2*n+i);
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",i,i%n+1,3*n+i%n+1,i,3*n+i,3*n+i%n+1);
}

fclose(f1);


return 0;}
