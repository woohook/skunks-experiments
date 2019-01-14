#include <stdlib.h>

#include "vehicle.h"

struct _vhc* vehicle_create()
{
  struct _vhc* pVehicle = calloc(1, sizeof(struct _vhc));
  pVehicle->dmode = 1;

  return pVehicle;
}

void vehicle_process(struct _vhc* pVehicle)
{
  pVehicle->af   = pVehicle->action_accelerate * pVehicle->accel * (float)pVehicle->dmode;
  pVehicle->bf   = (pVehicle->action_brake + 0.01f) * pVehicle->brake;
  pVehicle->turn = pVehicle->action_right - pVehicle->action_left;
  pVehicle->vrx  = ((float)pVehicle->turn) * 0.36;
  if(pVehicle->action_reverse > 0.0f)
  {
    pVehicle->dmode = -pVehicle->dmode;
    pVehicle->af    = 0;
  }
}
