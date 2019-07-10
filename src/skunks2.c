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
#include <SDL.h>
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
  vhc* car = pVehicle->vehicle;

  struct _entity* pControls = entity_create(pCarEntity,"controls","skunks:car:controls",sizeof(struct _entity));
  pControls->children = list_create();
  struct _entity* pControlItem = entity_create(pControls,"accelerate","skunks:car:controls:item",sizeof(struct _entity));
  pControlItem->value = &car->action_accelerate;
  *(float*)pControlItem->value = 0.0f;
  list_add_value(pControls->children, pControlItem);
  pControlItem = entity_create(pControls,"brake","skunks:car:controls:item",sizeof(struct _entity));
  pControlItem->value = &car->action_brake;
  *(float*)pControlItem->value = 0.0f;
  list_add_value(pControls->children, pControlItem);
  pControlItem = entity_create(pControls,"left","skunks:car:controls:item",sizeof(struct _entity));
  pControlItem->value = &car->action_left;
  *(float*)pControlItem->value = 0.0f;
  list_add_value(pControls->children, pControlItem);
  pControlItem = entity_create(pControls,"right","skunks:car:controls:item",sizeof(struct _entity));
  pControlItem->value = &car->action_right;
  *(float*)pControlItem->value = 0.0f;
  list_add_value(pControls->children, pControlItem);
  pControlItem = entity_create(pControls,"reverse","skunks:car:controls:item",sizeof(struct _entity));
  pControlItem->value = &car->action_reverse;
  *(float*)pControlItem->value = 0.0f;
  list_add_value(pControls->children, pControlItem);

  input_register(SDLK_UP, (float*)entity_get_by_name("/car/controls/accelerate")->value);
  input_register(SDLK_DOWN, (float*)entity_get_by_name("/car/controls/brake")->value);
  input_register(SDLK_LEFT, (float*)entity_get_by_name("/car/controls/left")->value);
  input_register(SDLK_RIGHT, (float*)entity_get_by_name("/car/controls/right")->value);
  input_register(SDLK_r, (float*)entity_get_by_name("/car/controls/reverse")->value);
  input_register(SDLK_ESCAPE, &action_quit);

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
