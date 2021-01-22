#include <stdlib.h>

#include "participant.h"

#include "list.h"

#include "config.h"
#include "defstr.h"
#include "camera.h"
#include "input.h"
#include "surface.h"
#include "surface_content.h"
#include "vehicle.h"
#include "entity.h"

struct _list* g_participants = 0;

struct _sgob* g_current_vehicle = 0;
struct _sgob* g_closest_vehicle = 0;

struct participant
{
  struct _sgob* pEntity;
  struct _surface* pSurface;
  struct _surface_content* pContent;
  struct _camera* pCamera;
  float switch_avatar;
};

void get_closest_vehicle(struct _list_item* pItem)
{
  if(g_closest_vehicle != 0)
  {
    return;
  }

  struct _sgob* pEntity = list_item_get_value(pItem);
  if(pEntity == g_current_vehicle)
  {
    return;
  }

  if(pEntity->vehicle == 0)
  {
    return;
  }

  g_closest_vehicle = pEntity;
}

struct participant* participant_create()
{
  struct participant* pParticipant = (struct participant*)calloc(1, sizeof(struct participant));

  // Initialize display
  pParticipant->pCamera = create_camera();
  camera_set_focus(pParticipant->pCamera, 0);
  pParticipant->pContent = surface_content_create(pParticipant->pCamera);
  pParticipant->pSurface = surface_create(800,600,pParticipant->pContent);

  list_add_value(g_participants, pParticipant);

  return pParticipant;
}

void participant_destroy(struct participant* pParticipant)
{
  struct _list_item* participantNode = list_get_first(g_participants);
  while(participantNode != 0)
  {
    struct participant* participant = list_item_get_value(participantNode);
    struct _list_item* nextParticipantNode = list_item_get_next(participantNode);
    if(participant == pParticipant)
    {
      list_item_remove(participantNode, 1);
      break;
    }
    participantNode = nextParticipantNode;
  }
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
  input_register("Key_F", &pParticipant->switch_avatar);

  pParticipant->pEntity = pEntity;
}

void participant_initialize()
{
  g_participants = list_create();
}

void participant_process()
{
  struct _list_item* participantNode = list_get_first(g_participants);
  while(participantNode != 0)
  {
    struct participant* pParticipant = list_item_get_value(participantNode);

    if(pParticipant->switch_avatar > 0.0f)
    {
      g_current_vehicle = pParticipant->pEntity;
      g_closest_vehicle = 0;
      entity_apply(&get_closest_vehicle);
      if(g_closest_vehicle != 0)
      {
        participant_assign_entity(pParticipant, g_closest_vehicle);
        return;
      }
    }

    participantNode = list_item_get_next(participantNode);
  }
}

void participant_release()
{
  list_release(g_participants, 1);
}
