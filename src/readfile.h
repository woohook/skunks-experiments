struct _list;
struct _sgob;

// function which reads the vehicle; must be called AFTER readtrack()
struct _sgob* readvehicle(char *numefis, float dx, float dy, float dz);

// function which reads the track
void readtrack(char *numefis);
