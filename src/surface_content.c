#include <stdlib.h>

#include "surface_content.h"
#include "render32.h"
#include "camera.h"

struct _surface_content
{
  struct _camera* camera;
} surface_content;

struct _surface_content* surface_content_create(struct _camera* camera)
{
  struct _surface_content* pContent = (struct _surface_content*)malloc(sizeof(struct _surface_content));
  pContent->camera = camera;

  return pContent;
}

void surface_content_render(struct _surface_content* content, struct _surface* pSurface)
{
  camera_update(content->camera);

  odis(pSurface, camera_get_view_transform(content->camera));
}

void surface_content_release(struct _surface_content* content)
{
  free(content);
}
