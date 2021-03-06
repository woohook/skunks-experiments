struct _matrix;
struct physics_instance;
struct _refpo;

void physics_init();
void physics_release();
void physics_process();

void physics_setERP(float erp);
void physics_setCFM(float cfm);
void physics_setGravity(float gravity);

void physics_createBody(struct physics_instance* physics_object, struct _matrix* transform);
void physics_getBodyPosition(struct physics_instance* object, float* x, float* y, float* z);
void physics_setBodyPosition(struct physics_instance* object, float x, float y, float z);
void physics_getBodyRotation(struct physics_instance* object, float* rot0, float* rot1, float* rot2, float* rot3, float* rot4, float* rot5, float* rot6, float* rot7, float* rot8, float* rot9, float* rot10, float* rot11);
void physics_setBodyMass(struct physics_instance* object, float mass, int distribution, float x, float y, float z);
void physics_setBodyFriction(struct physics_instance* object, float friction);
void physics_enableImprovedSpinning(struct physics_instance* object, int enabled);

void physics_createUniversalJoint(struct physics_instance* object1, struct physics_instance* object2, float tx, float ty, float tz);
void physics_createHinge2(struct physics_instance* object1, struct physics_instance* object2, float x, float y, float z, float* steering_torque, float* acceleration_intensity, float* brake_intensity, float spring_coefficient, float damper_coefficient);

struct _refpo* create_collision_geometry();
void create_collision_box(struct _refpo* geom, float x1, float y1, float z1, float x2, float y2, float z2, float lx, float ly, float lz);
void create_collision_cylinder(struct _refpo* geom, float x1, float y1, float z1, float x2, float y2, float z2, float radius, float height);
void create_collision_sphere(struct _refpo* geom, float x1, float y1, float z1, float x2, float y2, float z2, float radius);
void create_collision_mesh(struct _refpo* geom, float x1, float y1, float z1, float x2, float y2, float z2, int ttip);
struct physics_instance* create_collision_geometry_instance(struct _refpo* geom, float tx, float ty, float tz, float rx, float ry, float rz, struct _matrix* transform);
void start_physics_group();
void end_physics_group();

void runsim(float tstep);

void physics_getLinearBodyVelocity(struct physics_instance* object, float* speed, float* speed_delta);
void physics_getAngularBodyVelocity(struct physics_instance* object, float *rotspeed);
