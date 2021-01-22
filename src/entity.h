struct _sgob;
struct _list_item;

struct _sgob* entity_create();
void entity_destroy(struct _sgob* pEntity);

int entity_apply(int (*applyFunction)(struct _list_item*, void*), void* pContext);
