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


/*set camera*/
void setcamg(sgob *camera,vhc *car,int flag)
{const dReal *pos,*rot;
REALN x0,y0,z0,a,h,d;

pos=dBodyGetPosition(car->bid[1]);
rot=dBodyGetRotation(car->bid[1]);
camera->nref=0;

switch(flag){
  case 1: a=0.57;
          x0=pos[0]+12.0; y0=pos[1]; z0=pos[2]-12.0;
          camera->vx[0]=x0; camera->vy[0]=y0; camera->vz[0]=z0;
          camera->vx[1]=x0+cos(a); camera->vy[1]=y0; camera->vz[1]=z0+sin(a);
          camera->vx[2]=x0; camera->vy[2]=y0+1; camera->vz[2]=z0;
          camera->vx[3]=x0-sin(a); camera->vy[3]=y0; camera->vz[3]=z0+cos(a);
          break;

  case 2: h=car->camh; d=car->camd;
          x0=pos[0]; y0=pos[1]; z0=pos[2];
          camera->vx[0]=x0; camera->vy[0]=y0; camera->vz[0]=z0;
          camera->vx[1]=x0+rot[0]; camera->vy[1]=y0+rot[4]; camera->vz[1]=z0+rot[8];
          camera->vx[2]=x0+rot[1]; camera->vy[2]=y0+rot[5]; camera->vz[2]=z0+rot[9];
          camera->vx[3]=x0+rot[2]; camera->vy[3]=y0+rot[6]; camera->vz[3]=z0+rot[10];
          translat(camera,h*rot[0]+d*rot[2],h*rot[4]+d*rot[6],h*rot[8]+d*rot[10]);
          break;

  case 3: x0=pos[0]+7.0; y0=pos[1]; z0=pos[2]-8.0;
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
          break;

  default: break;
}
}


/*run 1 simulation step; tstep - time step; af, bf - acceleration and brake factors*/
void runsim(sgob *objs,int nob,vhc *car,REALN tstep,REALN vrx,REALN af,REALN bf,FILE *repf,REALN *timp)
{int i,j,k,l,m,n,nobtr, /*nobtr-number of objects in the track*/
     bcj[21]; /*bcj[i]-vehicle object to which contact joint 'i' is attached*/
const dReal *pos,*rot,*vel;
REALN x0,y0,z0,pin=0,bkf=0,
      kp,kd, /*suspension coefficients*/
      kps,kds, /*steering coefficients*/
      radius,fx,fy,fz,drg=0.0005; /*drag coefficient*/
static int rsem=0; /*when rsem==REPSTEPS, save replay data*/
dSurfaceParameters surf1;
dContactGeom dcgeom[21];
dContact dcon[21];

surf1.mode=dContactBounce|dContactSoftERP|dContactSoftCFM|dContactApprox1;
surf1.mu=car->mu;
surf1.bounce=0.5;
surf1.bounce_vel=0.1;
surf1.soft_erp=tstep*10000/(tstep*10000+100);
surf1.soft_cfm=1/(tstep*10000+100);

nobtr=nob-car->nob;
car->ncj=0; /*number of contact joints*/

kp=car->spring;
kd=car->damper;
kps=150; kds=5;

for(i=1;i<=car->nob;i++){
  if((car->ofc[i])>=2){
    rot=dBodyGetRotation(car->bid[i]);
    dBodySetFiniteRotationAxis(car->bid[i],rot[1],rot[5],rot[9]);
  }
}

pos=dBodyGetPosition(car->bid[1]); x0=pos[0]; y0=pos[1]; z0=pos[2];

for(i=1;i<=nobtr;i++){
  if((fabs(objs[i].vx[0]-x0)<50)&&(fabs(objs[i].vy[0]-y0)<50)&&(fabs(objs[i].vz[0]-z0)<50)){
    for(j=1;j<=car->nob;j++){
      k=car->oid[j];
      for(l=1;l<=(objs[k].nref/2);l++){
        for(m=1;m<=(objs[i].nref/2);m++){
          n=dCollide(objs[k].gid[l],objs[i].gid[m],1,&dcgeom[car->ncj+1],sizeof(dContactGeom));
          (car->ncj)+=n;
          if(n){bcj[car->ncj]=j;}
        }
      }
    }
  }
} /*created dContactGeom structures*/

for(i=1;i<=(car->ncj);i++){
  dcon[i].surface=surf1;
  dcon[i].geom=dcgeom[i];
  car->cjid[i]=dJointCreateContact(wglob,0,&dcon[i]);
  dJointAttach(car->cjid[i],car->bid[bcj[i]],0);
}

pin=af*car->accel;

bkf=(bf+0.01)*car->brake;

if(car->tjflag){
  dJointSetUniversalParam(car->tjid,dParamStopERP,tstep*kps/(tstep*kps+kds));
  dJointSetUniversalParam(car->tjid,dParamStopCFM,1/(tstep*kps+kds));
}

for(i=1;i<=car->nj;i++){
  dJointSetHinge2Param(car->jid[i],dParamSuspensionERP,tstep*kp/(tstep*kp+kd));
  dJointSetHinge2Param(car->jid[i],dParamSuspensionCFM,1/(tstep*kp+kd));
  dJointSetHinge2Param(car->jid[i],dParamStopERP,tstep*kps/(tstep*kps+kds));
  dJointSetHinge2Param(car->jid[i],dParamStopCFM,1/(tstep*kps+kds));
  switch(car->jfc[i]){
    case 3: dJointAddHinge2Torques(car->jid[i],0,pin); /*motor wheel*/
            break;
    case 4: dJointAddHinge2Torques(car->jid[i],360*vrx,0); /*steering wheel*/
            break;
    case 5: dJointAddHinge2Torques(car->jid[i],360*vrx,pin); /*motor and steering wheel*/
            break;
    default: break;
  }

  dJointSetAMotorParam(car->bkm[i],dParamFMax,bkf);
}

for(i=1;i<=car->nob;i++){
  radius=objs[car->oid[i]].radius; radius*=radius;
  vel=dBodyGetLinearVel(car->bid[i]);
  if((fabs(vel[0])+fabs(vel[1])+fabs(vel[2]))>1){
    fx=-drg*radius*vel[0]*fabs(vel[0]);
    fy=-drg*radius*vel[1]*fabs(vel[1]);
    fz=-drg*radius*vel[2]*fabs(vel[2]);
    dBodyAddForce(car->bid[i],fx,fy,fz);
  }
} /*air resistance*/

dWorldQuickStep(wglob,tstep);

for(i=1;i<=(car->ncj);i++){
  dJointDestroy(car->cjid[i]);
} car->ncj=0;

for(i=1;i<=car->nob;i++){
  j=car->oid[i];
  pos=dBodyGetPosition(car->bid[i]);
  rot=dBodyGetRotation(car->bid[i]);

  objs[j].vx[0]=objs[j].xcen=pos[0];
  objs[j].vy[0]=objs[j].ycen=pos[1];
  objs[j].vz[0]=objs[j].zcen=pos[2];

  objs[j].vx[1]=objs[j].vx[0]+rot[0];
  objs[j].vy[1]=objs[j].vy[0]+rot[4];
  objs[j].vz[1]=objs[j].vz[0]+rot[8];

  objs[j].vx[2]=objs[j].vx[0]+rot[1];
  objs[j].vy[2]=objs[j].vy[0]+rot[5];
  objs[j].vz[2]=objs[j].vz[0]+rot[9];

  objs[j].vx[3]=objs[j].vx[0]+rot[2];
  objs[j].vy[3]=objs[j].vy[0]+rot[6];
  objs[j].vz[3]=objs[j].vz[0]+rot[10];
}

*timp+=tstep;


#if REPLAY==1
rsem++;
if(rsem>=REPSTEPS){
  fprintf(repf,"%1.3f ",*timp);
  for(i=1;i<=car->nob;i++){
    j=car->oid[i];
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j].vx[0],objs[j].vy[0],objs[j].vz[0]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j].vx[1],objs[j].vy[1],objs[j].vz[1]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j].vx[2],objs[j].vy[2],objs[j].vz[2]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j].vx[3],objs[j].vy[3],objs[j].vz[3]);
  }
  fprintf(repf,"\r\n");
  rsem=0;
}
#endif

}


/*function which determines speed of vehicle*/
void rdspeed(vhc *car,REALN *speed,REALN *rotspeed,REALN *dspeed)
{int i,n=0;
const dReal *spe,*rot;
static REALN spe0[3]={0,0,0};
REALN dspe[3];

spe=dBodyGetLinearVel(car->bid[1]);

dspe[0]=spe[0]-spe0[0];
dspe[1]=spe[1]-spe0[1];
dspe[2]=spe[2]-spe0[2];

spe0[0]=spe[0]; spe0[1]=spe[1]; spe0[2]=spe[2];

*speed=sqrt(spe[0]*spe[0]+spe[1]*spe[1]+spe[2]*spe[2]);

*dspeed=sqrt(dspe[0]*dspe[0]+dspe[1]*dspe[1]+dspe[2]*dspe[2]);

*rotspeed=0;

for(i=1;i<=car->nob;i++){
  if((car->ofc[i]==3)||(car->ofc[i]==5)){ n++;
    rot=dBodyGetAngularVel(car->bid[i]);
    (*rotspeed)+=sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
  }
}
(*rotspeed)/=n; /*average rot. speed of motor wheels*/

}


#if SOUND==1

REALN sintr(REALN x)
{const REALN pi=3.1415926536;
REALN y;
x=(x/(2*pi)-floor(x/(2*pi)))*2*pi;
if(x<=pi/2){y=x/(pi/2);return y;}
if((x>pi/2)&&(x<=3*pi/2)){y=1-(x-pi/2)/(pi/2);return y;}
if(x>3*pi/2){y=-1+(x-3*pi/2)/(pi/2);return y;}
return 0;}


REALN sinst(REALN x)
{const REALN pi=3.1415926536;
REALN y;
x=(x/(2*pi)-floor(x/(2*pi)))*2*pi;
if(x<=pi){y=x/(pi/2);return y;}
if(x>pi){y=-1+(x-3*pi/2)/(pi/2);return y;}
return 0;}


REALN sinrd()
{REALN x;
x=rand();
x=(REALN)(((int)x)%100)/50-1;
return x;
}


/*audio callback function for SDL*/
void my_audio_callback(void *userdata, Uint8 *stream, int len)
{static REALD cr=0;
static int sem=0;
int freqtr,i,j1,j2,n=40;
REALN *volum,a,c,tmp;
Uint16 format;
Uint8 channels;
REALN coef;

static REALN freq[41]; /*frequencies of all sounds*/
REALN freqdo=0.3*261.6255653; /*first frequency*/



  if(sem==0){
    sem=1;
    tmp=1.06;
    freq[1]=freqdo;
    for(i=2;i<=40;i++){
      freq[i]=freq[i-1]*tmp;
    }
  } /*set frequencies*/

volum=(REALN *)userdata;

format=(Uint16)volum[2];
channels=(Uint8)volum[3];
freqtr=(int)volum[4];

switch(format){
	case AUDIO_U8: coef=1; break;
	case AUDIO_U16: coef=0.5; break;
	case AUDIO_S8: coef=1; break;
	case AUDIO_S16: coef=0.5; break;
	default: coef=0.5; break;
}
switch(channels){
	case 1: break;
	case 2: coef*=0.5;break;
	default: printf("Unsupported number of channels\r\n");exit(1);
}
coef=coef/freqtr;

/*SDL_Delay(10);*/

a=volum[1]/200; j1=(int)floor(a*(n-2)+1); j2=(int)floor(a*(n-2)+2);
a=a*(n-2)+1-j1;

  for(i=0;i<=len-1;i++){
    stream[i]=63;
    cr++;if(cr>1e6){cr=0;}
      c=0.85+0.15*sin(0.005*cr);
      stream[i]+=(Uint8)(10.0*c*(1.0-a)*sinst((freq[j1])*6.283185307*cr*coef));
      stream[i]+=(Uint8)(10.0*c*a*sinst((freq[j2])*6.283185307*cr*coef));
      if(volum[5]>20){
        stream[i]+=(Uint8)(40.0*c*a*sinrd());
      }

  }

}

#endif
