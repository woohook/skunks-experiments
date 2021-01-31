struct _list;
struct _sgob;

// function which reads the vehicle; must be called AFTER readtrack()
struct _sgob* readvehicle(char *numefis, float dx, float dy, float dz);

// function which reads the track
void readtrack(char *numefis);

// function which loads a single entity
void loader_load_entity(char* modelFilename, float tx, float ty, float tz, float rx, float ry, float rz);
