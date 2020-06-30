struct _sgob;

struct _sgob* entity_create();
void entity_destroy(struct _sgob* pEntity);

struct _sgob* entity_get_closest_vehicle(struct _sgob* pCurrentVehicle);
