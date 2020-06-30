#include <stdlib.h>

#include "entity.h"

#include "config.h"
#include "defstr.h"
#include "list.h"
#include "entity.h"

struct _list* g_entities = 0;

struct _sgob* entity_create()
{
  struct _sgob* pObject = 0;

  if(g_entities == 0)
  {
    g_entities = list_create();
  }

  pObject = (sgob*)calloc(1, sizeof(sgob));

  list_add_value(g_entities, pObject);

  return pObject;
}

void entity_destroy(struct _sgob* pEntity)
{
  free(pEntity);
}

struct _sgob* entity_get_closest_vehicle(struct _sgob* pCurrentVehicle)
{
  struct _list_item* entityNode = list_get_first(g_entities);
  while(entityNode != 0)
  {
    struct _sgob* pEntity = list_item_get_value(entityNode);
    if(pEntity != pCurrentVehicle)
    {
      if(pEntity->vehicle != 0)
      {
        return pEntity;
      }
    }
    entityNode = list_item_get_next(entityNode);
  }

  return 0;
}
