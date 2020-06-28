#include <stdlib.h>

#include "participant.h"

#include "config.h"
#include "defstr.h"
#include "camera.h"
#include "input.h"
#include "surface.h"
#include "surface_content.h"
#include "vehicle.h"

struct participant
{
  struct _sgob* pEntity;
  struct _surface* pSurface;
  struct _surface_content* pContent;
  struct _camera* pCamera;
};

struct participant* participant_create()
{
  struct participant* pParticipant = (struct participant*)calloc(1, sizeof(struct participant));

  // Initialize display
  pParticipant->pCamera = create_camera();
  camera_set_focus(pParticipant->pCamera, 0);
  pParticipant->pContent = surface_content_create(pParticipant->pCamera);
  pParticipant->pSurface = surface_create(800,600,pParticipant->pContent);

  return pParticipant;
}

void participant_destroy(struct participant* pParticipant)
{
  free(pParticipant);
}

void participant_assign_entity(struct participant* pParticipant, struct _sgob* pEntity)
{
  camera_set_focus(pParticipant->pCamera, &pEntity->transform);

  input_register("Key_Arrow_Up", &pEntity->vehicle->action_accelerate);
  input_register("Key_Arrow_Down", &pEntity->vehicle->action_brake);
  input_register("Key_Arrow_Left", &pEntity->vehicle->action_left);
  input_register("Key_Arrow_Right", &pEntity->vehicle->action_right);
  input_register("Key_R", &pEntity->vehicle->action_reverse);
  input_register("Key_L", &pEntity->vehicle->action_lights);
}
