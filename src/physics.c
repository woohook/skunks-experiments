#include <ode/ode.h>
#include "config.h"
#include "defstr.h"
#include "physics.h"
#include "trans.h"

typedef struct _refpo
{
  int nref;           //number of reference points
  REALD x[MAXREF];
  REALD y[MAXREF];
  REALD z[MAXREF];    // coordinates of the reference points
  char gtip[MAXGEOM]; // type of geom - 'b', 'c', 's', 't' (box, cylinder, sphere, triangle mesh)
  int ttip[MAXGEOM]; // type of triangle mesh - 1-left 45; 2-right 45; 3-left 22.5; 4-right 22.5
  REALN lx[MAXGEOM]; // geometry parameters; for cyl. - lx = radius, ly = length;
  REALN ly[MAXGEOM]; // for sphere - lx = radius; for box - lx = lx, ly = ly, lz = lz.
  REALN lz[MAXGEOM];
} refpo; // reference points of an object type

struct physics_instance
{
  int gtip;
  // geoms for collision; axis z of geom[i] is defined by
  //   (x[i*2-1],y[i*2-1],z[i*2-1]) and (x[i*2],y[i*2],z[i*2])
  int gid_count;
  dGeomID gid[MAXGEOM];
  dBodyID bodyID;
};

dJointID unijoint = 0; // single universal joint

dWorldID wglob; // world for ODE
refpo *refglob; // array with reference points of object types
int refcount = 0;
int dynStart = 0;

struct physics_instance** physics_instances = 0;
int physics_instance_count = 0;

void physics_init()
{
  dInitODE();
  wglob=dWorldCreate();
}

void physics_release()
{
  free (refglob);
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

void physics_createBody(struct physics_instance* object)
{
  int i;
  dBodyID bid = dBodyCreate(wglob);
  dMatrix3 rotmt; // rotation matrix

  if(dynStart==0) dynStart = physics_instance_count-1;

  for(i=1;i<=refglob[object->gtip].nref/2;i++)
  {
    dGeomSetBody(object->gid[i],bid);
  }

  rotmt[0]=1; rotmt[1]=0; rotmt[2]=0; rotmt[3]=0;
  rotmt[4]=0; rotmt[5]=1; rotmt[6]=0; rotmt[7]=0;
  rotmt[8]=0; rotmt[9]=0; rotmt[10]=1; rotmt[11]=0;
  dBodySetRotation(bid,rotmt);

  object->bodyID = bid;
}

void physics_createUniversalJoint(struct physics_instance* object1, struct physics_instance* object2, float tx, float ty, float tz)
{
  unijoint = dJointCreateUniversal(wglob,0);
  dJointAttach(unijoint,object1->bodyID, object2->bodyID);
  dJointSetUniversalAnchor(unijoint,tx,ty,tz);
  dJointSetUniversalAxis1(unijoint,1.0,0.0,0.0);
  dJointSetUniversalAxis2(unijoint,0.0,1.0,0.0);
  dJointSetUniversalParam(unijoint,dParamLoStop,-2.094);
  dJointSetUniversalParam(unijoint,dParamHiStop,2.094);
}

dJointID physics_createHinge2(struct physics_instance* object1, struct physics_instance* object2, float x, float y, float z)
{
  dJointID jid = dJointCreateHinge2(wglob,0);
  dJointAttach(jid,object1->bodyID,object2->bodyID);
  dJointSetHinge2Anchor(jid,x,y,z);
  dJointSetHinge2Axis1(jid,1,0,0);
  dJointSetHinge2Axis2(jid,0,1,0);
  dJointSetHinge2Param(jid,dParamLoStop,-0.001);
  dJointSetHinge2Param(jid,dParamHiStop,0.001); // for axis 1

  return jid;
}

dJointID physics_createAMotor(struct physics_instance* object1, struct physics_instance* object2, float max_acceleration)
{
  dJointID jid = dJointCreateAMotor(wglob,0);
  dJointAttach(jid,object1->bodyID,object2->bodyID);
  dJointSetAMotorNumAxes(jid,1);
  dJointSetAMotorAxis(jid,0,2,0,1,0);
  dJointSetAMotorParam(jid,dParamVel,0);
  dJointSetAMotorParam(jid,dParamFMax,0.01*max_acceleration);

  return jid;
}

void create_collision_geometry()
{
  refcount++;
  if(0!=(refglob=(refpo *)realloc(refglob,(refcount+1)*sizeof(refpo))))
  {
    refglob[refcount].nref = 0;
  }
  else
  {
    printf("Out of memory");
  }
}

void create_collision_box(float x1, float y1, float z1, float x2, float y2, float z2, float lx, float ly, float lz)
{
  refpo* geom = &refglob[refcount];
  geom->nref++;
  geom->x[geom->nref] = x1;
  geom->y[geom->nref] = y1;
  geom->z[geom->nref] = z1;
  geom->nref++;
  geom->x[geom->nref] = x2;
  geom->y[geom->nref] = y2;
  geom->z[geom->nref] = z2;
  geom->gtip[geom->nref/2] = 'b';
  geom->lx[geom->nref/2] = lx;
  geom->ly[geom->nref/2] = ly;
  geom->lz[geom->nref/2] = lz;
}

void create_collision_cylinder(float x1, float y1, float z1, float x2, float y2, float z2, float radius, float height)
{
  refpo* geom = &refglob[refcount];
  geom->nref++;
  geom->x[geom->nref] = x1;
  geom->y[geom->nref] = y1;
  geom->z[geom->nref] = z1;
  geom->nref++;
  geom->x[geom->nref] = x2;
  geom->y[geom->nref] = y2;
  geom->z[geom->nref] = z2;
  geom->gtip[geom->nref/2] = 'c';
  geom->lx[geom->nref/2] = radius;
  geom->ly[geom->nref/2] = height;
}

void create_collision_sphere(float x1, float y1, float z1, float x2, float y2, float z2, float radius)
{
  refpo* geom = &refglob[refcount];
  geom->nref++;
  geom->x[geom->nref] = x1;
  geom->y[geom->nref] = y1;
  geom->z[geom->nref] = z1;
  geom->nref++;
  geom->x[geom->nref] = x2;
  geom->y[geom->nref] = y2;
  geom->z[geom->nref] = z2;
  geom->gtip[geom->nref/2] = 's';
  geom->lx[geom->nref/2] = radius;
}

void create_collision_mesh(float x1, float y1, float z1, float x2, float y2, float z2, int ttip)
{
  refpo* geom = &refglob[refcount];
  geom->nref++;
  geom->x[geom->nref] = x1;
  geom->y[geom->nref] = y1;
  geom->z[geom->nref] = z1;
  geom->nref++;
  geom->x[geom->nref] = x2;
  geom->y[geom->nref] = y2;
  geom->z[geom->nref] = z2;
  geom->gtip[geom->nref/2] = 't';
  geom->ttip[geom->nref/2] = ttip;
}

// global variables for triangle meshes
dReal vert1glob[12]={0,-12.732,0,0,0,12.5,0.02,0,0,5.10,17.80,0},
      vert2glob[12]={0,12.732,0,0,0,-12.5,0.02,0,0,-5.10,17.80,0},
      vert3glob[12]={0,-25.465,0,0,0,12.5,0.02,0,0,9.61,14.50,0},
      vert4glob[12]={0,25.465,0,0,0,-12.5,0.02,0,0,-9.61,14.50,0};
dTriIndex indexlglob[3]={0,1,2},
          indexrglob[3]={0,2,1};

struct physics_instance* create_collision_geometry_instance(int geomtype, float tx, float ty, float tz, float rx, float ry, float rz)
{
  int j;
  float xref1, yref1, zref1, xref2, yref2, zref2;
  float ix, iy, iz, jx, jy, jz, kx, ky, kz, len;
  dMatrix3 rotmt; // ODE rotation matrix
  struct physics_instance* object = (struct physics_instance*)malloc(sizeof(struct physics_instance));
  object->bodyID = 0;
  object->gid_count = 1;

  // data for triangle meshes used at curved road elements
  dTriMeshDataID trid[5];

  trid[1]=dGeomTriMeshDataCreate();
  trid[2]=dGeomTriMeshDataCreate();
  trid[3]=dGeomTriMeshDataCreate();
  trid[4]=dGeomTriMeshDataCreate();
  dGeomTriMeshDataBuildSimple(trid[1],vert1glob,3,indexlglob,3);
  dGeomTriMeshDataBuildSimple(trid[2],vert2glob,3,indexrglob,3);
  dGeomTriMeshDataBuildSimple(trid[3],vert3glob,3,indexlglob,3);
  dGeomTriMeshDataBuildSimple(trid[4],vert4glob,3,indexrglob,3);

  object->gtip = geomtype;

  // translated and rotated object; set geometry parameters
  refpo* geom = &refglob[geomtype];
  for(j=1;j<=(geom->nref/2);j++)
  {
    xref1=geom->x[2*j-1];
    yref1=geom->y[2*j-1];
    zref1=geom->z[2*j-1];
    xref2=geom->x[2*j];
    yref2=geom->y[2*j];
    zref2=geom->z[2*j];
    if(rz != 0) rotate_vector_z(&xref1,&yref1,0,0,rz);
    if(ry != 0) rotate_vector_y(&xref1,&zref1,0,0,ry);
    if(rx != 0) rotate_vector_x(&yref1,&zref1,0,0,rx);
    translate_vector(&xref1, &yref1, &zref1, tx, ty, tz);
    if(rz != 0) rotate_vector_z(&xref2,&yref2,0,0,rz);
    if(ry != 0) rotate_vector_y(&xref2,&zref2,0,0,ry);
    if(rx != 0) rotate_vector_x(&yref2,&zref2,0,0,rx);
    translate_vector(&xref2, &yref2, &zref2, tx, ty, tz);

    switch(geom->gtip[j])
    {
      case 'b':
        object->gid[j] = dCreateBox(0, geom->lx[j], geom->ly[j], geom->lz[j]);
        break;
      case 'c':
        object->gid[j] = dCreateCCylinder(0, geom->lx[j], geom->ly[j]);
        break;
      case 's':
        object->gid[j] = dCreateSphere(0, geom->lx[j]);
        break;
      case 't':
        object->gid[j] = dCreateTriMesh(0,trid[geom->ttip[j]],0,0,0);
        break;
      default:
        printf("Unknown geometry '%c'\r\n",geom->gtip[j]);
        exit(1);
        break;
    }
    object->gid_count++;

    dGeomSetPosition(object->gid[j], xref1, yref1, zref1);

    kx=xref2-xref1;
    ky=yref2-yref1;
    kz=zref2-zref1;
    len=sqrt(kx*kx+ky*ky+kz*kz);
    kx/=len; ky/=len; kz/=len;

    // beam or column?
    len=sqrt(ky*ky+kz*kz); // (horizontal length)/(length)
    if(len<1e-5)  // column
    {
      jx=0; jy=0; jz=-1;
    }
    else // beam
    {
      jx=0; jy=kz; jz=-ky;
      len=sqrt(jx*jx+jy*jy+jz*jz);
      jx/=len; jy/=len; jz/=len;
    }

    // calculated local system
    ix=jy*kz-jz*ky;
    iy=jz*kx-jx*kz;
    iz=jx*ky-jy*kx;

    rotmt[0]=ix; rotmt[1]=jx; rotmt[2]=kx; rotmt[3]=0;
    rotmt[4]=iy; rotmt[5]=jy; rotmt[6]=ky; rotmt[7]=0;
    rotmt[8]=iz; rotmt[9]=jz; rotmt[10]=kz; rotmt[11]=0;
    dGeomSetRotation(object->gid[j],rotmt);
  }

  physics_instance_count++;
  physics_instances = (struct physics_instance**)realloc(physics_instances, physics_instance_count*sizeof(struct physics_instance*));
  physics_instances[physics_instance_count-1] = object;

  return object;
}

/*run 1 simulation step; tstep - time step; af, bf - acceleration and brake factors*/
void runsim(sgob** objs,vhc *car,float tstep,float vrx,float af,float bf)
{int i,j,k,l,m,n;
const dReal *pos,*rot,*vel;
float x0,y0,z0,pin=0,bkf=0,
      kp,kd, /*suspension coefficients*/
      kps,kds, /*steering coefficients*/
      radius,fx,fy,fz,drg=0.0005; /*drag coefficient*/
dSurfaceParameters surf1;
dContactGeom dcgeom[21];
dContact dcon[21];
int ncj = 0;                // number of contact joints
dJointID cjid[MAXGEOM]; // contact joints

surf1.mode=dContactBounce|dContactSoftERP|dContactSoftCFM|dContactApprox1;
surf1.mu=car->mu;
surf1.bounce=0.5;
surf1.bounce_vel=0.1;
surf1.soft_erp=tstep*10000/(tstep*10000+100);
surf1.soft_cfm=1/(tstep*10000+100);

kp=car->spring;
kd=car->damper;
kps=150; kds=5;

for(i=1;i<=car->nob;i++){
  if((car->ofc[i])>=2){
    rot=dBodyGetRotation(car->parts[i]->bodyID);
    dBodySetFiniteRotationAxis(car->parts[i]->bodyID,rot[1],rot[5],rot[9]);
  }
}

// creating dContactGeom structures
pos=dBodyGetPosition(car->parts[1]->bodyID); x0=pos[0]; y0=pos[1]; z0=pos[2];
for(i=0;i<dynStart;i++)
{
  if(physics_instances[i]->gid_count>1)
  {
    pos = dGeomGetPosition(physics_instances[i]->gid[1]);
    if((fabs(pos[0]-x0)<50) && (fabs(pos[1]-y0)<50) && (fabs(pos[2]-z0)<50))
    {
      for(k=dynStart;k<physics_instance_count;k++)
      {
        for(m=1;m < physics_instances[i]->gid_count;m++)
        {
          for(l=1;l < physics_instances[k]->gid_count;l++)
          {
            n=dCollide(physics_instances[k]->gid[l],physics_instances[i]->gid[m],1,&dcgeom[ncj+1],sizeof(dContactGeom));
            (ncj)+=n;
            if(n)
            {
              dcon[ncj].surface=surf1;
              dcon[ncj].geom=dcgeom[ncj];
              cjid[ncj]=dJointCreateContact(wglob,0,&dcon[ncj]);
              dJointAttach(cjid[ncj],physics_instances[k]->bodyID,0);
            }
          }
        }
      }
    }
  }
}

pin=af*car->accel;

bkf=(bf+0.01)*car->brake;

if(unijoint != 0){
  dJointSetUniversalParam(unijoint,dParamStopERP,tstep*kps/(tstep*kps+kds));
  dJointSetUniversalParam(unijoint,dParamStopCFM,1/(tstep*kps+kds));
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
  vel=dBodyGetLinearVel(car->parts[i]->bodyID);
  if((fabs(vel[0])+fabs(vel[1])+fabs(vel[2]))>1){
    fx=-drg*radius*vel[0]*fabs(vel[0]);
    fy=-drg*radius*vel[1]*fabs(vel[1]);
    fz=-drg*radius*vel[2]*fabs(vel[2]);
    dBodyAddForce(car->parts[i]->bodyID,fx,fy,fz);
  }
} /*air resistance*/

dWorldQuickStep(wglob,tstep);

for(i=1;i<=(ncj);i++){
  dJointDestroy(cjid[i]);
} ncj=0;

for(i=1;i<=car->nob;i++){
  j=car->oid[i];
  pos=dBodyGetPosition(car->parts[i]->bodyID);
  rot=dBodyGetRotation(car->parts[i]->bodyID);

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

}


/*function which determines speed of vehicle*/
void rdspeed(vhc *car,float *speed,float *rotspeed,float *dspeed)
{int i,n=0;
const dReal *spe,*rot;
static float spe0[3]={0,0,0};
float dspe[3];

spe=dBodyGetLinearVel(car->parts[1]->bodyID);

dspe[0]=spe[0]-spe0[0];
dspe[1]=spe[1]-spe0[1];
dspe[2]=spe[2]-spe0[2];

spe0[0]=spe[0]; spe0[1]=spe[1]; spe0[2]=spe[2];

*speed=sqrt(spe[0]*spe[0]+spe[1]*spe[1]+spe[2]*spe[2]);

*dspeed=sqrt(dspe[0]*dspe[0]+dspe[1]*dspe[1]+dspe[2]*dspe[2]);

*rotspeed=0;

for(i=1;i<=car->nob;i++){
  if((car->ofc[i]==3)||(car->ofc[i]==5)){ n++;
    rot=dBodyGetAngularVel(car->parts[i]->bodyID);
    (*rotspeed)+=sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
  }
}
(*rotspeed)/=n; /*average rot. speed of motor wheels*/

}
