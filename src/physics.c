#include <ode/ode.h>
#include "trans.h"
#include "physics.h"
#include "list.h"
#include "clock.h"

#define PHYS_MAXREF 109  //maximum number of reference points for 1 object

#define PHYS_MAXGEOM 55  //maximum number of geoms (ODE) for 1 object (=PHYS_MAXREF/2)

#define GRAVITY -9.81  // gravitational acceleration in m/(s*s)

#define STIMESTEP 0.005 // desired simulation time step (seconds)

typedef struct _refpo
{
  int nref;           //number of reference points
  float x[PHYS_MAXREF];
  float y[PHYS_MAXREF];
  float z[PHYS_MAXREF];    // coordinates of the reference points
  char gtip[PHYS_MAXGEOM]; // type of geom - 'b', 'c', 's', 't' (box, cylinder, sphere, triangle mesh)
  int ttip[PHYS_MAXGEOM]; // type of triangle mesh - 1-left 45; 2-right 45; 3-left 22.5; 4-right 22.5
  float lx[PHYS_MAXGEOM]; // geometry parameters; for cyl. - lx = radius, ly = length;
  float ly[PHYS_MAXGEOM]; // for sphere - lx = radius; for box - lx = lx, ly = ly, lz = lz.
  float lz[PHYS_MAXGEOM];
} refpo; // reference points of an object type

struct physics_instance
{
  refpo* geom;
  // geoms for collision; axis z of geom[i] is defined by
  //   (x[i*2-1],y[i*2-1],z[i*2-1]) and (x[i*2],y[i*2],z[i*2])
  int gid_count;
  dGeomID gid[PHYS_MAXGEOM];
  dBodyID bodyID;
  dMass mass;
  struct _matrix* transform;
  float radius;  // radius of sphere in which the object is included
  float friction;
  int isSpinningImproved;
};

dJointID unijoint = 0; // single universal joint

struct hinge2
{
  dJointID jid;
  float* steering_torque;
  float* acceleration_intensity;
  float* brake_intensity;
  float spring_coefficient;
  float damper_coefficient;
};
int hinge2_count = 0;
struct hinge2* hinge2s[PHYS_MAXGEOM];

dWorldID wglob; // world for ODE
int dynStart = 0;

struct _list* instances_static = 0;
struct _list* instances_dynamic = 0;
struct _list* g_geometries = 0;

void physics_init()
{
  dInitODE();
  wglob=dWorldCreate();
  physics_setERP(0.2);
  physics_setCFM(1e-5);
  physics_setGravity(GRAVITY);
}

void physics_release()
{
  list_release(g_geometries,1);
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

void physics_createBody(struct physics_instance* object, struct _matrix* transform)
{
  int i;
  dBodyID bid = dBodyCreate(wglob);
  dMatrix3 rotmt; // rotation matrix

  refpo* geom = object->geom;
  for(i=1;i<=geom->nref/2;i++)
  {
    dGeomSetBody(object->gid[i],bid);
    float lx = 0, ly = 0, lz = 0, radius = 0.0f;
    switch(geom->gtip[i])
    {
      case 'b':
        lx = geom->lx[i];
        ly = geom->ly[i];
        lz = geom->lz[i];
        break;
      case 'c':
        lx = geom->lx[i]*2;
        ly = geom->ly[i];
        lz = geom->lx[i]*2;
        break;
      case 's':
        radius = geom->lx[i];
        break;
      default:
        break;
    }
    if(radius==0.0f)
    {
      radius = sqrt(lx*lx + ly*ly + lz*lz)/2;
    }
    if(radius > object->radius)
    {
      object->radius = radius;
    }
  }

  rotmt[0]=1; rotmt[1]=0; rotmt[2]=0; rotmt[3]=0;
  rotmt[4]=0; rotmt[5]=1; rotmt[6]=0; rotmt[7]=0;
  rotmt[8]=0; rotmt[9]=0; rotmt[10]=1; rotmt[11]=0;
  dBodySetRotation(bid,rotmt);

  object->bodyID = bid;
  object->transform = transform;
  object->friction = 0;
  object->isSpinningImproved = 0;
}

void physics_getBodyPosition(struct physics_instance* object, float* x, float* y, float* z)
{
  const dReal* pos;

  pos = dBodyGetPosition(object->bodyID);

  *x = pos[0];
  *y = pos[1];
  *z = pos[2];
}

void physics_setBodyPosition(struct physics_instance* object, float x, float y, float z)
{
  dBodySetPosition(object->bodyID, x,y,z);
}

void physics_getBodyRotation(struct physics_instance* object, float* rot0, float* rot1, float* rot2, float* rot3, float* rot4, float* rot5, float* rot6, float* rot7, float* rot8, float* rot9, float* rot10, float* rot11)
{
  const dReal* rot;

  rot = dBodyGetRotation(object->bodyID);

  *rot0 = rot[0]; *rot1 = rot[1]; *rot2  = rot[2];  *rot3  = rot[3];
  *rot4 = rot[4]; *rot5 = rot[5]; *rot6  = rot[6];  *rot7  = rot[7];
  *rot8 = rot[8]; *rot9 = rot[9]; *rot10 = rot[10]; *rot11 = rot[11];
}

void physics_setBodyMass(struct physics_instance* object, float mass, int distribution, float x, float y, float z)
{
  switch(distribution)
  {
    case 1:
      dMassSetBoxTotal(&object->mass,mass,x,y,z);
      break;
    case 3:
      dMassSetSphereTotal(&object->mass,mass,x);
      break;
    default:
      return;
      break;
  }
  dBodySetMass(object->bodyID,&object->mass);
}

void physics_setBodyFriction(struct physics_instance* object, float friction)
{
  object->friction = friction;
}

void physics_enableImprovedSpinning(struct physics_instance* object, int enabled)
{
  object->isSpinningImproved = enabled;
  dBodySetFiniteRotationMode(object->bodyID,1);
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

void physics_createHinge2(struct physics_instance* object1, struct physics_instance* object2, float x, float y, float z, float* steering_torque, float* acceleration_intensity, float* brake_intensity, float spring_coefficient, float damper_coefficient)
{
  dJointID jid = dJointCreateHinge2(wglob,0);
  dJointAttach(jid,object1->bodyID,object2->bodyID);
  dJointSetHinge2Anchor(jid,x,y,z);
  dJointSetHinge2Axis1(jid,1,0,0);
  dJointSetHinge2Axis2(jid,0,1,0);
  dJointSetHinge2Param(jid,dParamLoStop,-0.001);
  dJointSetHinge2Param(jid,dParamHiStop,0.001); // for axis 1

  hinge2s[hinge2_count] = (struct hinge2*)malloc(sizeof(struct hinge2));
  hinge2s[hinge2_count]->jid                    = jid;
  hinge2s[hinge2_count]->steering_torque        = steering_torque;
  hinge2s[hinge2_count]->acceleration_intensity = acceleration_intensity;
  hinge2s[hinge2_count]->brake_intensity        = brake_intensity;
  hinge2s[hinge2_count]->spring_coefficient     = spring_coefficient;
  hinge2s[hinge2_count]->damper_coefficient     = damper_coefficient;
  hinge2_count++;
}

struct _refpo* create_collision_geometry()
{
  if(g_geometries == 0)
  {
    g_geometries = list_create();
  }

  refpo* geom = (refpo*)malloc(sizeof(refpo));
  if(0!=geom)
  {
    geom->nref = 0;
  }
  else
  {
    printf("Out of memory");
  }
  list_add_value(g_geometries, geom);

  return geom;
}

void create_collision_box(struct _refpo* geom, float x1, float y1, float z1, float x2, float y2, float z2, float lx, float ly, float lz)
{
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

void create_collision_cylinder(struct _refpo* geom, float x1, float y1, float z1, float x2, float y2, float z2, float radius, float height)
{
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

void create_collision_sphere(struct _refpo* geom, float x1, float y1, float z1, float x2, float y2, float z2, float radius)
{
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

void create_collision_mesh(struct _refpo* geom, float x1, float y1, float z1, float x2, float y2, float z2, int ttip)
{
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

struct physics_instance* create_collision_geometry_instance(struct _refpo* geom, float tx, float ty, float tz, float rx, float ry, float rz, struct _matrix* transform)
{
  int j;
  float xref1, yref1, zref1, xref2, yref2, zref2;
  float ix, iy, iz, jx, jy, jz, kx, ky, kz, len;
  dMatrix3 rotmt; // ODE rotation matrix
  struct physics_instance* object = (struct physics_instance*)malloc(sizeof(struct physics_instance));
  object->bodyID = 0;
  object->gid_count = 1;
  object->transform = 0;
  object->radius = 0.0f;

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

  object->geom = geom;

  // translated and rotated object; set geometry parameters
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


  if(transform != 0)
  {
    struct _list_item* current_group_node = list_get_last(instances_dynamic);
    struct _list* current_group = list_item_get_value(current_group_node);
    list_add_value(current_group, object);
    physics_createBody(object, transform);
  }
  else
  {
    if(instances_static == 0)
    {
      instances_static = list_create();
    }
    list_add_value(instances_static, object);
  }

  return object;
}

void start_physics_group()
{
  if(instances_dynamic == 0)
  {
    instances_dynamic = list_create();
  }
  struct _list* instance_dynamic = list_create();
  list_add_value(instances_dynamic, instance_dynamic);
}

void end_physics_group()
{
}

/*run 1 simulation step; tstep - time step; af, bf - acceleration and brake factors*/
void runsim(float tstep)
{int i,l,m,n;
const dReal *pos,*rot,*vel;
float x0,y0,z0,pin=0,bkf=0,
      kps,kds, /*steering coefficients*/
      radius,fx,fy,fz,drg=0.0005; /*drag coefficient*/
dContactGeom dcgeom[21];
dContact dcon[21];
int ncj = 0;                // number of contact joints
dJointID cjid[PHYS_MAXGEOM]; // contact joints


kps=150; kds=5;

// creating dContactGeom structures
struct _list_item* dynamic_group_node = list_get_first(instances_dynamic);
while(dynamic_group_node != 0)
{
  struct _list* pGroupInst = (struct _list*)list_item_get_value(dynamic_group_node);
  struct _list_item* dynamic_group_node_next = list_item_get_next(dynamic_group_node);

struct _list_item* dynamic_instance_node = list_get_first(pGroupInst);
while(dynamic_instance_node != 0)
{
  struct physics_instance* pDynInst = list_item_get_value(dynamic_instance_node);
  struct _list_item* dynamic_instance_node_next = list_item_get_next(dynamic_instance_node);

  if(pDynInst->isSpinningImproved == 1)
  {
    rot=dBodyGetRotation(pDynInst->bodyID);
    dBodySetFiniteRotationAxis(pDynInst->bodyID,rot[1],rot[5],rot[9]);
  }

  pos=dBodyGetPosition(pDynInst->bodyID); x0=pos[0]; y0=pos[1]; z0=pos[2];

  struct _list_item* static_instance_node = list_get_first(instances_static);
  struct _list_item* group_node = 0;
  while(static_instance_node != 0)
  {
    struct physics_instance* pStatInst = list_item_get_value(static_instance_node);
    struct _list_item* static_instance_node_next = 0;
    if(static_instance_node == list_get_last(instances_static))
    {
      group_node = list_get_first(instances_dynamic);
      if(group_node != 0)
      {
        if(group_node == dynamic_group_node)
        {
          group_node = list_item_get_next(group_node);
        }
        if(group_node != 0)
        {
          struct _list* group = list_item_get_value(group_node);
          static_instance_node_next = list_get_first(group);
        }
      }
    }
    else
    {
      static_instance_node_next = list_item_get_next(static_instance_node);
    }

    if(static_instance_node_next == dynamic_instance_node)
    {
      static_instance_node_next = list_item_get_next(static_instance_node_next);
    }

    if( (static_instance_node_next == 0) && (group_node != 0) && (list_item_get_next(group_node) != 0) )
    {
      group_node = list_item_get_next(group_node);
      if(group_node != 0)
      {
        if(group_node == dynamic_group_node)
        {
          group_node = list_item_get_next(group_node);
        }
        if(group_node != 0)
        {
          struct _list* group = list_item_get_value(group_node);
          static_instance_node_next = list_get_first(group);
        }
      }
    }

    if(pStatInst->gid_count>1)
    {
      pos = dGeomGetPosition(pStatInst->gid[1]);
      if((fabs(pos[0]-x0)<50) && (fabs(pos[1]-y0)<50) && (fabs(pos[2]-z0)<50))
      {
        for(m=1;m < pStatInst->gid_count;m++)
        {
          for(l=1;l < pDynInst->gid_count;l++)
          {
            n=dCollide(pDynInst->gid[l],pStatInst->gid[m],1,&dcgeom[ncj+1],sizeof(dContactGeom));
            (ncj)+=n;
            if(n)
            {
              dSurfaceParameters surf1;
              surf1.mode=dContactBounce|dContactSoftERP|dContactSoftCFM|dContactApprox1;
              surf1.mu=pDynInst->friction;
              surf1.bounce=0.5;
              surf1.bounce_vel=0.1;
              surf1.soft_erp=tstep*10000/(tstep*10000+100);
              surf1.soft_cfm=1/(tstep*10000+100);

              dcon[ncj].surface=surf1;
              dcon[ncj].geom=dcgeom[ncj];
              cjid[ncj]=dJointCreateContact(wglob,0,&dcon[ncj]);
              dJointAttach(cjid[ncj],pDynInst->bodyID,0);
            }
          }
        }
      }
    }
    static_instance_node = static_instance_node_next;
  }
  dynamic_instance_node = dynamic_instance_node_next;
  }
  dynamic_group_node = dynamic_group_node_next;
}

if(unijoint != 0){
  dJointSetUniversalParam(unijoint,dParamStopERP,tstep*kps/(tstep*kps+kds));
  dJointSetUniversalParam(unijoint,dParamStopCFM,1/(tstep*kps+kds));
}

for(i=0;i<hinge2_count;i++){
  dJointID jid = hinge2s[i]->jid;
  float kp = hinge2s[i]->spring_coefficient;
  float kd = hinge2s[i]->damper_coefficient;
  dJointSetHinge2Param(jid,dParamSuspensionERP,tstep*kp/(tstep*kp+kd));
  dJointSetHinge2Param(jid,dParamSuspensionCFM,1/(tstep*kp+kd));
  dJointSetHinge2Param(jid,dParamStopERP,tstep*kps/(tstep*kps+kds));
  dJointSetHinge2Param(jid,dParamStopCFM,1/(tstep*kps+kds));
  if(hinge2s[i]->steering_torque != 0)
  {
    if(hinge2s[i]->acceleration_intensity != 0)
    {
      pin=*hinge2s[i]->acceleration_intensity;
      dJointAddHinge2Torques(jid,360*(*hinge2s[i]->steering_torque),pin); // motor and steering wheel
    }
    else
    {
      dJointAddHinge2Torques(jid,360*(*hinge2s[i]->steering_torque),0); // steering wheel
    }
  }
  else
  {
    if(hinge2s[i]->acceleration_intensity != 0)
    {
      pin=*hinge2s[i]->acceleration_intensity;
      dJointAddHinge2Torques(jid,0,pin); // motor wheel
    }
  }


  if(hinge2s[i]->brake_intensity != 0)
  {
    bkf=*hinge2s[i]->brake_intensity;
    dJointSetHinge2Param(jid,dParamFMax2,bkf);
  }
}

dynamic_group_node = list_get_first(instances_dynamic);
while(dynamic_group_node != 0)
{
  struct _list* pGroupInst = list_item_get_value(dynamic_group_node);
  dynamic_group_node = list_item_get_next(dynamic_group_node);

struct _list_item* dynamic_instance_node = list_get_first(pGroupInst);
while(dynamic_instance_node != 0)
{
  struct physics_instance* pDynInst = list_item_get_value(dynamic_instance_node);
  dynamic_instance_node = list_item_get_next(dynamic_instance_node);

  radius=pDynInst->radius; radius*=radius;
  vel=dBodyGetLinearVel(pDynInst->bodyID);
  if((fabs(vel[0])+fabs(vel[1])+fabs(vel[2]))>1){
    fx=-drg*radius*vel[0]*fabs(vel[0]);
    fy=-drg*radius*vel[1]*fabs(vel[1]);
    fz=-drg*radius*vel[2]*fabs(vel[2]);
    dBodyAddForce(pDynInst->bodyID,fx,fy,fz);
  }
}
} /*air resistance*/

dWorldQuickStep(wglob,tstep);

for(i=1;i<=(ncj);i++){
  dJointDestroy(cjid[i]);
} ncj=0;

dynamic_group_node = list_get_first(instances_dynamic);
while(dynamic_group_node != 0)
{
  struct _list* pGroupInst = list_item_get_value(dynamic_group_node);
  dynamic_group_node = list_item_get_next(dynamic_group_node);

struct _list_item* dynamic_instance_node = list_get_first(pGroupInst);
while(dynamic_instance_node != 0)
{
  struct physics_instance* object = list_item_get_value(dynamic_instance_node);
  dynamic_instance_node = list_item_get_next(dynamic_instance_node);

  pos=dBodyGetPosition(object->bodyID);
  rot=dBodyGetRotation(object->bodyID);

  object->transform->vx[0] = pos[0];
  object->transform->vy[0] = pos[1];
  object->transform->vz[0] = pos[2];

  object->transform->vx[1] = object->transform->vx[0] + rot[0];
  object->transform->vy[1] = object->transform->vy[0] + rot[4];
  object->transform->vz[1] = object->transform->vz[0] + rot[8];

  object->transform->vx[2] = object->transform->vx[0] + rot[1];
  object->transform->vy[2] = object->transform->vy[0] + rot[5];
  object->transform->vz[2] = object->transform->vz[0] + rot[9];

  object->transform->vx[3] = object->transform->vx[0] + rot[2];
  object->transform->vy[3] = object->transform->vy[0] + rot[6];
  object->transform->vz[3] = object->transform->vz[0] + rot[10];
}
}

}


void physics_getLinearBodyVelocity(struct physics_instance* object, float* speed, float* speed_delta)
{
  const dReal *spe;
  static float spe0[3]={0,0,0};
  float dspe[3];

  spe=dBodyGetLinearVel(object->bodyID);

  dspe[0]=spe[0]-spe0[0];
  dspe[1]=spe[1]-spe0[1];
  dspe[2]=spe[2]-spe0[2];

  spe0[0]=spe[0]; spe0[1]=spe[1]; spe0[2]=spe[2];

  *speed=sqrt(spe[0]*spe[0]+spe[1]*spe[1]+spe[2]*spe[2]);

  *speed_delta=sqrt(dspe[0]*dspe[0]+dspe[1]*dspe[1]+dspe[2]*dspe[2]);
}

void physics_getAngularBodyVelocity(struct physics_instance* object, float *rotspeed)
{
  const dReal *rot;

  rot=dBodyGetAngularVel(object->bodyID);
  *rotspeed=sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
}

void physics_process()
{
  float realstep,  // real time step (s)
        sim_speed;
  int   nstepsf;   // number of simulation steps/frame
  float tframe = clock_get_frametime() / 1000.0f;

  nstepsf=(int)(tframe/STIMESTEP)+1;
  realstep=tframe/nstepsf;           // simulation time step

  sim_speed=0.1/realstep;            // decrease simulation speed if < 10fps
  if(nstepsf>(int)sim_speed)
  {
    nstepsf=(int)sim_speed;
  }

  for(int i=1;i<=nstepsf;i++)
  {
    runsim(realstep);
  }
}
