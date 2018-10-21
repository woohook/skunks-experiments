struct _list;
struct _list_item;

void list_initialize(struct _list* list);

void list_add(struct _list* list, void* item);

struct _list_item* list_get_first(struct _list* list);

struct _list_item* list_get_next(struct _list_item* item);

void* list_get_item(struct _list_item* item);
