#include <stdlib.h>

#include "vehicle.h"
#include "list.h"

struct _list* vehicles = (struct _list*)0;

void vehicle_initialize()
{
  vehicles = list_create();
}

struct _vhc* vehicle_create()
{
  struct _vhc* pVehicle = calloc(1, sizeof(struct _vhc));
  pVehicle->dmode = 1;
  list_add_value(vehicles,pVehicle);

  return pVehicle;
}

void vehicle_process()
{
  struct _list_item* pVehicleNode = list_get_first(vehicles);

  while(pVehicleNode != 0)
  {
    struct _vhc* pVehicle = list_item_get_value(pVehicleNode);

    pVehicle->af   = pVehicle->action_accelerate * pVehicle->accel * (float)pVehicle->dmode;
    pVehicle->bf   = (pVehicle->action_brake + 0.01f) * pVehicle->brake;
    pVehicle->turn = pVehicle->action_right - pVehicle->action_left;
    pVehicle->vrx  = ((float)pVehicle->turn) * 0.36;
    if(pVehicle->action_reverse > 0.0f)
    {
      pVehicle->dmode = -pVehicle->dmode;
      pVehicle->af    = 0;
    }

    pVehicleNode = list_item_get_next(pVehicleNode);
  }
}

void vehicle_release()
{
  list_release(vehicles,1);
}
