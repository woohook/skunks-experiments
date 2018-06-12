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

struct physics_instance;

extern refpo *refglob; // array with reference points of object types

void physics_init();
void physics_release();

void physics_setERP(float erp);
void physics_setCFM(float cfm);
void physics_setGravity(float gravity);

dBodyID physics_createBody();
void physics_createUniversalJoint(dBodyID body1, dBodyID body2, float tx, float ty, float tz);
dJointID physics_createHinge2(dBodyID body1, dBodyID body2, float x, float y, float z);
dJointID physics_createAMotor(dBodyID body1, dBodyID body2, float max_acceleration);

void create_collision_geometry();
void create_collision_box(float x1, float y1, float z1, float x2, float y2, float z2, float lx, float ly, float lz);
void create_collision_cylinder(float x1, float y1, float z1, float x2, float y2, float z2, float radius, float height);
void create_collision_sphere(float x1, float y1, float z1, float x2, float y2, float z2, float radius);
void create_collision_mesh(float x1, float y1, float z1, float x2, float y2, float z2, int ttip);
struct physics_instance* create_collision_geometry_instance(int geomtype, float tx, float ty, float tz, float rx, float ry, float rz);
void attach_body(struct physics_instance* object, dBodyID bid);

void runsim(sgob** objs,int nob,vhc *car,float tstep,float vrx,float af,float bf,FILE *repf,float *timp);

void rdspeed(vhc *car,float *speed,float *rotspeed,float *dspeed);
