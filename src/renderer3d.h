struct _surface;

struct frustum
{
  float zmin, zmax;
  float tgh, tgv;
};

struct lights
{
  float ambient_light_intensity;
  float directional_light_intensity;
  float directional_light_x;
  float directional_light_y;
  float directional_light_z;
  float head_light_intensity;
};

struct renderer_triangle
{
  float x1, y1, z1;
  float x2, y2, z2;
  float x3, y3, z3;
  int red, green, blue;
};

void renderer3d_initialize(struct _surface* pSurface);
void renderer3d_clear_depth_buffer(struct frustum* pFrustum);
void renderer3d_render_triangle(struct renderer_triangle* pTriangle, struct frustum* pFrustum, struct _surface* pSurface, struct lights* pLights);
