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
  char buffer[1024];
  char* vehicleName = "cars/car1";
  char* worldName = "tracks/track1";
  int width  = SCREENWIDTH;
  int height = SCREENHEIGHT;

  int argc = 0;
  char** argv = 0;

  struct _entity* pVehiclesEntity = entity_create(0,"vehicles","vehicles",0);

  buffer[0] = '\0';

  framework_get_args(&argc,&argv);
  for(int i=1; i<argc; i++)
  {
    if(strncmp("cars/",argv[i],5)==0)
    {
      int vehicle_count = 0;
      if(entity_get_children(pVehiclesEntity) != 0) vehicle_count = list_get_size(entity_get_children(pVehiclesEntity));
      vehicle_count++;
      float dz = 20.0f*(vehicle_count-1);

      vehicleName = argv[i];
      strcpy(buffer, "car0000");
      buffer[strlen(buffer)-1] = (char)(vehicle_count % 10 + '0'); vehicle_count /= 10;
      buffer[strlen(buffer)-2] = (char)(vehicle_count % 10 + '0'); vehicle_count /= 10;
      buffer[strlen(buffer)-3] = (char)(vehicle_count % 10 + '0'); vehicle_count /= 10;
      buffer[strlen(buffer)-4] = (char)(vehicle_count % 10 + '0'); vehicle_count /= 10;
      struct _entity* pCarEntity = entity_create(pVehiclesEntity, buffer, "skunks:car", 0);
      readvehicle(pCarEntity, vehicleName,0,0,dz);
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

  struct _entity* pTrackEntity = entity_create(0,"track","skunks:track",0);
  entity_create(pTrackEntity,worldName,"skunks:track:modelname",0);
  readtrack(pTrackEntity, worldName);

  if( (entity_get_children(pVehiclesEntity) == 0) || (list_get_size(entity_get_children(pVehiclesEntity)) == 0) )
  {
    struct _entity* pCarEntity = entity_create(pVehiclesEntity,"car0001","skunks:car",0);
    readvehicle(pCarEntity, vehicleName,0,0,0);
  }

  struct _entity* pActorEntity = entity_create(0,"actor","actor",0);
  entity_create(pActorEntity,"accelerate","command","Key_Arrow_Up");
  entity_create(pActorEntity,"brake","command","Key_Arrow_Down");
  entity_create(pActorEntity,"left","command","Key_Arrow_Left");
  entity_create(pActorEntity,"right","command","Key_Arrow_Right");
  entity_create(pActorEntity,"reverse","command","Key_R");

  struct _entity* pFirstVehicleEntity = (struct _entity*)list_get_value(entity_get_children(pVehiclesEntity), 0);
  strcpy(buffer, entity_get_name(pFirstVehicleEntity));
  strcpy(buffer + strlen(buffer), "/controls");

  struct _list_item* pCommandItem = list_get_first(entity_get_children(pActorEntity));
  struct _entity* pControls = entity_get_by_name(buffer);
  int offsetCommand = strlen(entity_get_name(pActorEntity));
  int offsetAction = strlen(entity_get_name(pControls));
  while(pCommandItem != 0)
  {
    struct _entity* pCommand = (struct _entity*)list_item_get_value(pCommandItem);
    struct _list_item* pActionItem = list_get_first(entity_get_children(pControls));
    while(pActionItem != 0)
    {
      struct _entity* pAction = (struct _entity*)list_item_get_value(pActionItem);
      if(strcmp(entity_get_name(pCommand)+offsetCommand, entity_get_name(pAction)+offsetAction)==0)
      {
        input_register(entity_get_value(pCommand), (float*)entity_get_value(pAction));
        break;
      }
      pActionItem = list_item_get_next(pActionItem);
    }
    pCommandItem = list_item_get_next(pCommandItem);
  }

  input_register("Key_Escape", &action_quit);

  strcpy(buffer, entity_get_name(pFirstVehicleEntity));
  strcpy(buffer + strlen(buffer), "/object0000");
  struct _entity* pVehicleObjectEntity = entity_get_by_name(buffer);
  sgob* pVehicle = (struct _sgob*)entity_get_value(pVehicleObjectEntity);

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
