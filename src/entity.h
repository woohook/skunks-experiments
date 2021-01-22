struct _sgob;
struct _list_item;

struct _sgob* entity_create();
void entity_destroy(struct _sgob* pEntity);

void entity_apply(void (*applyFunction)(struct _list_item*));
