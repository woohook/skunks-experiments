/*
Copyright (C) 2007-2011 Victor Matei Petrescu

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

struct _surface;
struct _matrix;
struct _mesh;

struct _mesh* create_mesh();

struct mesh_instance* create_mesh_instance(struct _mesh* pMesh, struct _matrix* transform, int isDynamic);
void complete_mesh(struct _mesh* pMesh);
void add_face(struct _mesh* pMesh, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3);

void add_material(struct _mesh* pMesh, int red, int green, int blue, int fullbright);

void set_face_material(struct _mesh* pMesh, int face_id, int material_id);

void add_material_action(struct _mesh* pMesh, float* input, int material_id);

void get_face_vertex(struct _mesh* pMesh, int face_id, int vertex_id, float *x, float *y, float *z);

void flip_face(struct _mesh* pMesh, int face_id);

void enable_face_culling(struct _mesh* pMesh, int face_id);

int get_face_count(struct _mesh* pMesh);

void adjust_material_colors(float red, float green, float blue);

void set_background_color(int red, int green, int blue);

void set_ambient_light(float ambient_light);

void set_headlight(float headlight);

void set_directional_light(float directional_light, float dx, float dy, float dz);

void set_width_factor(float width_factor);

void set_view_angle(float view_angle);

void set_double_pixel(int double_pixel);

/*functie care determina ecuatia planului cu numarul de ordine 'i'
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nrfaces - numar total de triunghiuri*/
void findplan(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float *a,float *b,float *c,float *d);


/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera*/
void odis(struct _surface* pSurface,struct _matrix* view_transform);

void renderer_initialize();
void renderer_set_surface_camera(struct _surface* pSurface, struct _matrix* view_transform);
void renderer_process();
void renderer_release();
