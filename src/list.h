struct _list;
struct _list_item;

struct _list* list_create();
void list_release(struct _list* list, int release_items);

int list_get_size(struct _list* list);

void list_add_value(struct _list* list, void* value);
void list_item_remove(struct _list_item* item, int release_value);

struct _list_item* list_get_first(struct _list* list);
struct _list_item* list_get_last(struct _list* list);

struct _list_item* list_item_get_next(struct _list_item* item);

void* list_item_get_value(struct _list_item* item);
void* list_get_value(struct _list* list, int item_index);

int list_apply(struct _list* pList, int (*applyFunction)(struct _list_item*, void*), void* pContext);
