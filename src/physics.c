#include <ode/ode.h>
#include "config.h"
#include "defstr.h"
#include "physics.h"

dWorldID wglob; // world for ODE
refpo *refglob; // array with reference points of object types

void physics_init()
{
  dInitODE();
  wglob=dWorldCreate();
}

void physics_release()
{
  dWorldDestroy(wglob);
  dCloseODE();
}

void physics_setERP(float erp)
{
  dWorldSetERP(wglob,erp);
}

void physics_setCFM(float cfm)
{
  dWorldSetCFM(wglob,cfm);
}

void physics_setGravity(float gravity)
{
  dWorldSetGravity(wglob,gravity,0,0);
}

dBodyID physics_createBody()
{
  return dBodyCreate(wglob);
}

dJointID physics_createUniversalJoint()
{
  return dJointCreateUniversal(wglob,0);
}

dJointID physics_createHinge2()
{
  return dJointCreateHinge2(wglob,0);
}

dJointID physics_createAMotor()
{
  return dJointCreateAMotor(wglob,0);
}

/*run 1 simulation step; tstep - time step; af, bf - acceleration and brake factors*/
void runsim(sgob** objs,int nob,vhc *car,float tstep,float vrx,float af,float bf,FILE *repf,float *timp)
{int i,j,k,l,m,n,nobtr, /*nobtr-number of objects in the track*/
     bcj[21]; /*bcj[i]-vehicle object to which contact joint 'i' is attached*/
const dReal *pos,*rot,*vel;
float x0,y0,z0,pin=0,bkf=0,
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
  if((fabs(objs[i]->transform.vx[0]-x0)<50) && (fabs(objs[i]->transform.vy[0]-y0)<50) && (fabs(objs[i]->transform.vz[0]-z0)<50)){
    for(j=1;j<=car->nob;j++){
      k=car->oid[j];
      for(l=1;l<=(objs[k]->nref/2);l++){
        for(m=1;m<=(objs[i]->nref/2);m++){
          n=dCollide(objs[k]->gid[l],objs[i]->gid[m],1,&dcgeom[car->ncj+1],sizeof(dContactGeom));
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
  radius=objs[car->oid[i]]->radius; radius*=radius;
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

  objs[j]->transform.vx[0]=pos[0];
  objs[j]->transform.vy[0]=pos[1];
  objs[j]->transform.vz[0]=pos[2];

  objs[j]->transform.vx[1]=objs[j]->transform.vx[0]+rot[0];
  objs[j]->transform.vy[1]=objs[j]->transform.vy[0]+rot[4];
  objs[j]->transform.vz[1]=objs[j]->transform.vz[0]+rot[8];

  objs[j]->transform.vx[2]=objs[j]->transform.vx[0]+rot[1];
  objs[j]->transform.vy[2]=objs[j]->transform.vy[0]+rot[5];
  objs[j]->transform.vz[2]=objs[j]->transform.vz[0]+rot[9];

  objs[j]->transform.vx[3]=objs[j]->transform.vx[0]+rot[2];
  objs[j]->transform.vy[3]=objs[j]->transform.vy[0]+rot[6];
  objs[j]->transform.vz[3]=objs[j]->transform.vz[0]+rot[10];
}

*timp+=tstep;


#if REPLAY==1
rsem++;
if(rsem>=REPSTEPS){
  fprintf(repf,"%1.3f ",*timp);
  for(i=1;i<=car->nob;i++){
    j=car->oid[i];
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j]->transform.vx[0],objs[j]->transform.vy[0],objs[j]->transform.vz[0]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j]->transform.vx[1],objs[j]->transform.vy[1],objs[j]->transform.vz[1]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j]->transform.vx[2],objs[j]->transform.vy[2],objs[j]->transform.vz[2]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j]->transform.vx[3],objs[j]->transform.vy[3],objs[j]->transform.vz[3]);
  }
  fprintf(repf,"\r\n");
  rsem=0;
}
#endif

}


/*function which determines speed of vehicle*/
void rdspeed(vhc *car,float *speed,float *rotspeed,float *dspeed)
{int i,n=0;
const dReal *spe,*rot;
static float spe0[3]={0,0,0};
float dspe[3];

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
