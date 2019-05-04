struct _list;

struct _entity
{
  char* entity_type;
  char* name;
  void* value;
  struct _entity* parent;
  struct _list* children;
};

struct _entity* entity_create(struct _entity* parent, char* name, char* entity_type, int size);

struct _entity* entity_get_by_name(char* name);

void entity_list_all();
