#include <stdio.h>
#include <stdlib.h>
#include <math.h>


#define PI 3.1415927


int main()
{int i,n;
float r2,t,d2,
      x[1001],y[1001],z[1001],
      alf0,alf;
FILE *f1;

printf("R2: "); scanf("%f",&r2);
printf("t: "); scanf("%f",&t);
printf("d2: "); scanf("%f",&d2);
printf("n: "); scanf("%d",&n);

alf0=2.0*PI/n;

for(i=1;i<=n;i++){
  alf=alf0*i;
  x[i]=(r2-0.5*t)*sin(alf); x[n+i]=(r2+0.5*t)*sin(alf);
  y[i]=(r2-0.5*t)*cos(alf); y[n+i]=(r2+0.5*t)*cos(alf);
  z[i]=-0.5*d2+0.5*t; z[n+i]=-0.5*d2-0.5*t;
}


if(!(f1=fopen("circles.geo","w"))){printf("Could not open 'circles.geo'\r\n"); exit(1);}

fprintf(f1,"%d %d\r\n",4*n,4*n);

for(i=1;i<=2*n;i++){
  fprintf(f1,"%1.3f %1.3f %2.3f\r\n",x[i],y[i],z[i]);
}
for(i=1;i<=2*n;i++){
  fprintf(f1,"%1.3f %1.3f %2.3f\r\n",x[i],y[i],-z[i]);
}

for(i=1;i<=n;i++){
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",i,n+i,n+i%n+1,i,i%n+1,n+i%n+1);
  fprintf(f1,"f %d %d %d\r\nf %d %d %d\r\n",2*n+i,2*n+n+i,2*n+n+i%n+1,2*n+i,2*n+i%n+1,2*n+n+i%n+1);
}


fclose(f1);

return 0;}
