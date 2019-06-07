void setcamg(struct _matrix *camera, struct _matrix* object)
{
float pos[3];
float rot[12];
float x0,y0,z0,a,h,d;

pos[0]  = object->vx[0];
pos[1]  = object->vy[0];
pos[2]  = object->vz[0];

rot[0]  = object->vx[1] - object->vx[0];
rot[1]  = object->vx[2] - object->vx[0];
rot[2]  = object->vx[3] - object->vx[0];

rot[4]  = object->vy[1] - object->vy[0];
rot[5]  = object->vy[2] - object->vy[0];
rot[6]  = object->vy[3] - object->vy[0];

rot[8]  = object->vz[1] - object->vz[0];
rot[9]  = object->vz[2] - object->vz[0];
rot[10] = object->vz[3] - object->vz[0];

          x0=pos[0]+7.0; y0=pos[1]; z0=pos[2]-8.0;
          camera->vx[0]=x0; camera->vy[0]=y0; camera->vz[0]=z0;
          camera->vx[1]=x0+1; camera->vy[1]=y0; camera->vz[1]=z0;
          camera->vx[2]=x0; camera->vy[2]=y0+1; camera->vz[2]=z0;
          camera->vx[3]=x0; camera->vy[3]=y0; camera->vz[3]=z0+1;
          y0=rot[6]; z0=rot[10]; a=sqrt(y0*y0+z0*z0);
          if(fabs(a)>1e-5){y0/=a; z0/=a;}else{y0=0; z0=1;}
          h=camera->vy[3]-camera->vy[0]; d=camera->vz[3]-camera->vz[0];
          a=y0*h+z0*d;
          if(a>1){a=1;}else{if(a<-1){a=-1;}}
          if(y0>0){a=acos(a);}else{a=-acos(a);}
          rotatx(camera,pos[1],pos[2],-a);
          rotab(camera,camera->vx[0],camera->vy[0],camera->vz[0],camera->vx[2],camera->vy[2],camera->vz[2],-0.57);
}
