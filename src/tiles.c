// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>

#include "list.h"
#include "config.h"
#include "defstr.h"
#include "entity.h"
#include "render32.h"
#include "readfile.h"


void tiles_initialize()
{
}

void tiles_process()
{
}

void tiles_release()
{
}

#define TURN_LEFT  0
#define TURN_RIGHT 1
#define FORWARD    2
#define DIRECTIONS 3

#define STRAIGHT_LEFT  4
#define STRAIGHT_RIGHT 5
#define STRAIGHT_UP    6

void tiles_generate_random(struct _sgob* pVehicle)
{
  pVehicle = pVehicle;  // prevent warnings about unused parameter

  srand((unsigned int)pVehicle);

  int y = 0;
  int z = 10;
  float yy = 1.0f*y;
  float zz = 1.0f*z;
  int last_direction = STRAIGHT_UP;
  int track_length = 20 + (rand() % 20);
  for(int i=0; i<track_length; ++i)
  {
    int random_direction = rand() % DIRECTIONS;

    if(last_direction==STRAIGHT_UP)
    {
      if(random_direction==TURN_LEFT)
      {
        loader_load_entity("data/tile_left", -1.0f,yy,zz, 0,0,0);
        y = y-20;
        yy = 1.0f*y;
        loader_load_entity("data/tile_straight", -1.0f,yy,zz, 1.5708,0,0);
        y = y-20;
        yy = 1.0f*y;
        last_direction = STRAIGHT_LEFT;
      }
      if(random_direction==TURN_RIGHT)
      {
        loader_load_entity("data/tile_left", -1.0f,yy,zz, 1.5708,0,0);
        y = y+20;
        yy = 1.0f*y;
        loader_load_entity("data/tile_straight", -1.0f,yy,zz, 1.5708,0,0);
        y = y+20;
        yy = 1.0f*y;
        last_direction = STRAIGHT_RIGHT;
      }
      if(random_direction==FORWARD)
      {
        loader_load_entity("data/tile_straight", -1.0f,yy,zz, 0,0,0);
        z = z+20;
        zz = 1.0f*z;
        last_direction = STRAIGHT_UP;
      }
    }

    if(last_direction==STRAIGHT_LEFT)
    {
      if(random_direction==TURN_LEFT)
      {
        --i;
        continue;
      }
      if(random_direction==TURN_RIGHT)
      {
        loader_load_entity("data/tile_left", -1.0f,yy,zz, 3.1416,0,0);
        z = z+20;
        zz = 1.0f*z;
        loader_load_entity("data/tile_straight", -1.0f,yy,zz, 0,0,0);
        z = z+20;
        zz = 1.0f*z;
        last_direction = STRAIGHT_UP;
      }
      if(random_direction==FORWARD)
      {
        loader_load_entity("data/tile_straight", -1.0f,yy,zz, 1.5708,0,0);
        y = y-20;
        yy = 1.0f*y;
        last_direction = STRAIGHT_LEFT;
      }
    }

    if(last_direction==STRAIGHT_RIGHT)
    {
      if(random_direction==TURN_LEFT)
      {
        loader_load_entity("data/tile_left", -1.0f,yy,zz, -1.5708,0,0);
        z = z+20;
        zz = 1.0f*z;
        loader_load_entity("data/tile_straight", -1.0f,yy,zz, 0,0,0);
        z = z+20;
        zz = 1.0f*z;
        last_direction = STRAIGHT_UP;
      }
      if(random_direction==TURN_RIGHT)
      {
        --i;
        continue;
      }
      if(random_direction==FORWARD)
      {
        loader_load_entity("data/tile_straight", -1.0f,yy,zz, 1.5708,0,0);
        y = y+20;
        yy = 1.0f*y;
        last_direction = STRAIGHT_RIGHT;
      }
    }
  }
}
