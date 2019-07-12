/*
Copyright (C) 2007-2013 Victor Matei Petrescu

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <string.h>
#include "config.h"

#include "defstr.h"

#include "framework.h"
#include "surface.h"
#include "surface_content.h"
#include "render32.h"
#include "physics.h"
#include "input.h"
#include "entities.h"

#include "trans.h"
#include "list.h"
#include "vehicle.h"
#include "readfile.h"


#define SCREENWIDTH 800   // screen width (pixels)
#define SCREENHEIGHT 600  // screen height (pixels)

struct _matrix camera;

float action_quit = 0;

void skunks_initialize()
{
  char* vehicleName = "cars/car1";
  char* worldName = "tracks/track1";
  int width  = SCREENWIDTH;
  int height = SCREENHEIGHT;

  int argc = 0;
  char** argv = 0;
  framework_get_args(&argc,&argv);
  for(int i=1; i<argc; i++)
  {
    if(strncmp("cars/",argv[i],5)==0)
    {
      vehicleName = argv[i];
    }
    if(strncmp("tracks/",argv[i],7)==0)
    {
      worldName = argv[i];
    }
    if(strncmp("res=",argv[i],4)==0)
    {
      sscanf(argv[i],"res=%dx%d", &width, &height);
    }
  }

  struct _entity* pTrackEntity = entity_create(0,"track","skunks:track",sizeof(struct _entity));
  entity_create(pTrackEntity,worldName,"skunks:track:modelname",sizeof(struct _entity));
  readtrack(pTrackEntity, worldName);

  struct _entity* pCarEntity = entity_create(0,"car","skunks:car",sizeof(struct _entity));
  readvehicle(pCarEntity, vehicleName);
  sgob* pVehicle = (struct _sgob*)entity_get_by_name("/car/object0000")->value;

  struct _entity* pActorEntity = entity_create(0,"actor","actor",sizeof(struct _entity));
  struct _entity* pCommandEntity = entity_create(pActorEntity,"accelerate","command",sizeof(struct _entity));
  pCommandEntity->value = "Key_Arrow_Up";
  pCommandEntity = entity_create(pActorEntity,"brake","command",sizeof(struct _entity));
  pCommandEntity->value = "Key_Arrow_Down";
  pCommandEntity = entity_create(pActorEntity,"left","command",sizeof(struct _entity));
  pCommandEntity->value = "Key_Arrow_Left";
  pCommandEntity = entity_create(pActorEntity,"right","command",sizeof(struct _entity));
  pCommandEntity->value = "Key_Arrow_Right";
  pCommandEntity = entity_create(pActorEntity,"reverse","command",sizeof(struct _entity));
  pCommandEntity->value = "Key_R";

  struct _list_item* pCommandItem = list_get_first(pActorEntity->children);
  struct _entity* pControls = entity_get_by_name("/car/controls");
  int offsetCommand = strlen(pActorEntity->name);
  int offsetAction = strlen(pControls->name);
  while(pCommandItem != 0)
  {
    struct _entity* pCommand = (struct _entity*)list_item_get_value(pCommandItem);
    struct _list_item* pActionItem = list_get_first(pControls->children);
    while(pActionItem != 0)
    {
      struct _entity* pAction = (struct _entity*)list_item_get_value(pActionItem);
      if(strcmp(pCommand->name+offsetCommand, pAction->name+offsetAction)==0)
      {
        input_register(pCommand->value, (float*)pAction->value);
        break;
      }
      pActionItem = list_item_get_next(pActionItem);
    }
    pCommandItem = list_item_get_next(pCommandItem);
  }

  input_register("Key_Escape", &action_quit);

  // Initialize display
  matrix_identity(&camera);
  struct _surface_content* pContent = surface_content_create(&camera, &pVehicle->transform);
  surface_create(width,height,pContent);
}

void skunks_process()
{
  if(action_quit > 0)
  {
    framework_request_shutdown(0);
  }
}

void skunks_release()
{
}
