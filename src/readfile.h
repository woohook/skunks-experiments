struct _list;
struct _entity;

// function which reads the vehicle; must be called AFTER readtrack()
struct _list* readvehicle(char *numefis);

// function which reads the track
void readtrack(struct _entity* parent, char *numefis);
