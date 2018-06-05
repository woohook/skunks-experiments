void physics_init();
void physics_release();

void physics_setERP(float erp);
void physics_setCFM(float cfm);
void physics_setGravity(float gravity);

dBodyID physics_createBody();
dJointID physics_createUniversalJoint();
dJointID physics_createHinge2();
dJointID physics_createAMotor();

void runsim(sgob** objs,int nob,vhc *car,float tstep,float vrx,float af,float bf,FILE *repf,float *timp);

void rdspeed(vhc *car,float *speed,float *rotspeed,float *dspeed);
