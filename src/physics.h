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

extern refpo *refglob; // array with reference points of object types

void physics_init();
void physics_release();

void physics_setERP(float erp);
void physics_setCFM(float cfm);
void physics_setGravity(float gravity);

dBodyID physics_createBody();
dJointID physics_createUniversalJoint();
dJointID physics_createHinge2();
dJointID physics_createAMotor();

void create_collision_geometry();

void runsim(sgob** objs,int nob,vhc *car,float tstep,float vrx,float af,float bf,FILE *repf,float *timp);

void rdspeed(vhc *car,float *speed,float *rotspeed,float *dspeed);
