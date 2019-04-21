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
#include "render32.h"
#include "physics.h"
#include "input.h"

#include "trans.h"
#include "list.h"
#include "vehicle.h"
#include "readfile.h"
#include "camera.h"


#define SCREENWIDTH 800   // screen width (pixels)
#define SCREENHEIGHT 600  // screen height (pixels)

struct _list* objs = 0;
struct _list* parts = 0;
struct _matrix camera;
struct _surface* pSurface = NULL;

float action_quit = 0;

void skunks_initialize()
{
  vhc* car = vehicle_create();

  char* vehicleName = "cars/car5";
  char* worldName = "tracks/track10";
  int width  = SCREENWIDTH;
  int height = SCREENHEIGHT;

  // Initialize display
  pSurface = surface_create(width,height);

  matrix_identity(&camera);

  renderer_set_surface_camera(pSurface, &camera);

  objs = readtrack(worldName);

  parts = readvehicle(vehicleName,car);

  input_register(SDLK_UP, &car->action_accelerate);
  input_register(SDLK_DOWN, &car->action_brake);
  input_register(SDLK_LEFT, &car->action_left);
  input_register(SDLK_RIGHT, &car->action_right);
  input_register(SDLK_r, &car->action_reverse);
  input_register(SDLK_ESCAPE, &action_quit);
}

void skunks_process()
{
  sgob* pVehicle = list_get_value(parts,0);
  setcamg(&camera,&pVehicle->transform);

  if(action_quit > 0)
  {
    framework_request_shutdown(0);
  }
}

void skunks_release()
{
  list_release(objs,1);
  list_release(parts,1);
}
