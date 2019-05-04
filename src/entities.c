#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "entities.h"
#include "list.h"

struct _list* g_entities = 0;

struct _entity* entity_create(struct _entity* parent, char* name, char* entity_type, int size)
{
  struct _entity* pEntity = (struct _entity*)malloc(size);
  char* fullname = 0;
  int name_length = strlen(name);
  int parent_length = 0;

  if(g_entities == 0)
  {
    g_entities = list_create();
  }

  if(parent != 0)
  {
    parent_length = strlen(parent->name);
  }
  fullname = malloc(parent_length + 1 + name_length + 1);
  if(parent_length > 0)
  {
    strncpy(fullname, parent->name, parent_length);
  }
  fullname[parent_length] = '/';
  strncpy(fullname + parent_length + 1, name, name_length);
  fullname[parent_length + 1 + name_length] = '\0';

  pEntity->entity_type = entity_type;
  pEntity->name = fullname;
  pEntity->parent = parent;

  list_add_value(g_entities, pEntity);

  return pEntity;
}

void entity_list_all()
{
  struct _list_item* entity_node = list_get_first(g_entities);

  while(entity_node != 0)
  {
    struct _entity* pEntity = list_item_get_value(entity_node);

    printf("Entity name='%s'\n", pEntity->name);

    entity_node = list_item_get_next(entity_node);
  }
}
