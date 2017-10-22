#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415927


int main()
{int i,n;
float r2,alf0,alf;
FILE *f1;

printf("R2: "); scanf("%f",&r2);
printf("n: "); scanf("%d",&n);

alf0=2.0*PI/n;

if(!(f1=fopen("wheel1","w"))){printf("Could not open 'wheel1'\r\n"); exit(1);}

fprintf(f1,"objtypes 3\r\n");
fprintf(f1,"wheel/ray.geo wheel/ray.col wheel/nocl.cld\r\n");
fprintf(f1,"wheel/circles.geo wheel/circles.col wheel/nocl.cld\r\n");
fprintf(f1,"wheel/sphere.geo wheel/sphere.col wheel/sphere.cld\r\n");
fprintf(f1,"objects %d\r\n",n+2);
fprintf(f1,"1 0 0 0\r\n");
fprintf(f1,"2 0 0 0\r\n");

for(i=1;i<=n;i++){
  alf=alf0*i;
  fprintf(f1,"3 %1.3f %1.3f %1.3f\r\n",r2*sin(alf),r2*cos(alf),0.0);
}

fclose(f1);

return 0;}
