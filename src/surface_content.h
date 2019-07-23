struct _surface_content;
struct _surface;
struct _camera;

struct _surface_content* surface_content_create(struct _camera* camera);
void surface_content_render(struct _surface_content* content, struct _surface* pSurface);
void surface_content_release(struct _surface_content* content);
