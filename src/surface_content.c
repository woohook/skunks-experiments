#include <stdlib.h>
#include <math.h>

#include "surface_content.h"
#include "trans.h"
#include "render32.h"
#include "camera.h"

struct _surface_content
{
  struct _matrix* view_transform;
  struct _matrix* object;
} surface_content;

struct _surface_content* surface_content_create(struct _matrix* view_transform, struct _matrix* object)
{
  struct _surface_content* pContent = (struct _surface_content*)malloc(sizeof(struct _surface_content));
  pContent->view_transform = view_transform;
  pContent->object = object;

  return pContent;
}

void surface_content_render(struct _surface_content* content, struct _surface* pSurface)
{
  setcamg(content->view_transform, content->object);

  odis(pSurface, content->view_transform);
}

void surface_content_release(struct _surface_content* content)
{
  free(content);
}
