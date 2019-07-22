struct _entity;
struct _list;

struct _entity* entity_create(struct _entity* parent, char* name, char* entity_type, void* value);

struct _entity* entity_get_by_name(char* name);

void entity_list_all();

void entity_set_value(struct _entity* pEntity, void* value);

void* entity_get_value(struct _entity* pEntity);

struct _list* entity_get_children(struct _entity* pEntity);

char* entity_get_name(struct _entity* pEntity);
