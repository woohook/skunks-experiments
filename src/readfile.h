struct _list;
struct _entity;

// function which reads the vehicle; must be called AFTER readtrack()
void readvehicle(struct _entity* parent, char *numefis);

// function which reads the track
void readtrack(struct _entity* parent, char *numefis);
