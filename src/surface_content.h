struct _surface_content;
struct _surface;
struct _matrix;

struct _surface_content* surface_content_create(struct _matrix* view_transform);
void surface_content_render(struct _surface_content* content, struct _surface* pSurface);
void surface_content_release(struct _surface_content* content);
