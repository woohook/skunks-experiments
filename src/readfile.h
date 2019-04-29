struct _list;
struct _vhc;

// function which reads the vehicle; must be called AFTER readtrack()
struct _list* readvehicle(char *numefis, struct _vhc *car);

// function which reads the track
struct _list* readtrack(char *numefis);
