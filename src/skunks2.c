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

#include "trans.h"
#include "list.h"
#include "vehicle.h"
#include "readfile.h"
#include "camera.h"
#include "participant.h"
#include "tiles.h"


#define SCREENWIDTH 800   // screen width (pixels)
#define SCREENHEIGHT 600  // screen height (pixels)

float action_quit = 0;

void skunks_initialize()
{
  char* vehicleName = "cars/car1";
  char* worldName = "tracks/track1";
  int width  = SCREENWIDTH;
  int height = SCREENHEIGHT;

  int argc = 0;
  char** argv = 0;
  int vehicle_count = 0;
  struct _sgob* pVehicle = 0;

  framework_get_args(&argc,&argv);
  for(int i=1; i<argc; i++)
  {
    if(strncmp("cars/",argv[i],5)==0)
    {
      float dz = 20.0f*(vehicle_count);
      vehicle_count++;

      vehicleName = argv[i];
      pVehicle = readvehicle(vehicleName,0,0,dz);
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

  if(strncmp("tracks/random",worldName,13)==0)
  {
    set_ambient_light(0.7f);
    set_background_color(127,127,255);
    tiles_generate_random(pVehicle);
  }
  else
  {
    readtrack(worldName);
  }

  if(pVehicle == 0)
  {
    pVehicle = readvehicle(vehicleName,0,0,0);
  }

  input_register("Key_Escape", &action_quit);

  struct participant* player = participant_create();
  participant_assign_entity(player, pVehicle);
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
