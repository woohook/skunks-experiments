#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "entities.h"
#include "list.h"

struct _entity
{
  char* entity_type;
  char* name;
  void* value;
  struct _entity* parent;
  struct _list* children;
};

struct _list* g_entities = 0;

struct _entity* entity_create(struct _entity* parent, char* name, char* entity_type, void* value)
{
  struct _entity* pEntity = (struct _entity*)malloc(sizeof(struct _entity));
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
  pEntity->children = 0;
  pEntity->value = value;

  list_add_value(g_entities, pEntity);
  if(parent != 0)
  {
    if(parent->children == 0)
    {
      parent->children = list_create();
    }
    list_add_value(parent->children, pEntity);
  }

  return pEntity;
}

struct _entity* entity_get_by_name(char* name)
{
  struct _entity* pEntityResult = 0;

  struct _list_item* entity_node = list_get_first(g_entities);

  while(entity_node != 0)
  {
    struct _entity* pEntity = list_item_get_value(entity_node);

    if(strcmp(name, pEntity->name) == 0)
    {
      pEntityResult = pEntity;
      break;
    }

    entity_node = list_item_get_next(entity_node);
  }

  return pEntityResult;
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

void entity_set_value(struct _entity* pEntity, void* value)
{
  pEntity->value = value;
}

void* entity_get_value(struct _entity* pEntity)
{
  return pEntity->value;
}

struct _list* entity_get_children(struct _entity* pEntity)
{
  return pEntity->children;
}

char* entity_get_name(struct _entity* pEntity)
{
  return pEntity->name;
}
