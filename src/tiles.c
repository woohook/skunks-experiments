// SPDX-License-Identifier: GPL-3.0-or-later

#include "list.h"
#include "config.h"
#include "defstr.h"
#include "entity.h"
#include "render32.h"

int show_all_meshes(struct _list_item* pEntityItem, void* pContext)
{
  pContext = pContext;  // unused parameter

  struct _sgob* pEntity = (struct _sgob*)list_item_get_value(pEntityItem);

  if(pEntity->pMeshInstance == 0)
  {
    pEntity->pMeshInstance = create_mesh_instance(pEntity->pObjectType->pMesh, &pEntity->transform);
  }

  return 0;
}

void tiles_initialize()
{
  entity_apply(&show_all_meshes, 0);
}

void tiles_process()
{
}

void tiles_release()
{
}
