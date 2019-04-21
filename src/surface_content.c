#include <stdlib.h>
#include "surface_content.h"
#include "render32.h"

struct _surface_content
{
  struct _matrix* view_transform;
} surface_content;

struct _surface_content* surface_content_create(struct _matrix* view_transform)
{
  struct _surface_content* pContent = (struct _surface_content*)malloc(sizeof(struct _surface_content));
  pContent->view_transform = view_transform;

  return pContent;
}

void surface_content_render(struct _surface_content* content, struct _surface* pSurface)
{
  odis(pSurface, content->view_transform);
}

void surface_content_release(struct _surface_content* content)
{
  free(content);
}
