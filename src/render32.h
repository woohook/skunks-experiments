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

void create_mesh();

void add_face(int mesh_id, REALN x1, REALN y1, REALN z1, REALN x2, REALN y2, REALN z2, REALN x3, REALN y3, REALN z3);

void set_face_color(int mesh_id, int face_id, int red, int green, int blue);

void get_face_color(int mesh_id, int face_id, int* red, int* green, int* blue);

void set_face_fullbright(int mesh_id, int face_id);

void get_face_vertex(int mesh_id, int face_id, int vertex_id, REALN *x, REALN *y, REALN *z);

void flip_face(int mesh_id, int face_id);

void enable_face_culling(int mesh_id, int face_id);

void set_background_color(int red, int green, int blue);

void set_ambient_light(float ambient_light);

void set_headlight(float headlight);

void set_directional_light(float directional_light, float dx, float dy, float dz);

/*functie care determina ecuatia planului cu numarul de ordine 'i'
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nrfaces - numar total de triunghiuri*/
void findplan(REALN x1, REALN y1, REALN z1, REALN x2, REALN y2, REALN z2, REALN x3, REALN y3, REALN z3, REALN *a,REALN *b,REALN *c,REALN *d);


/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera*/
void odis(SDL_Surface *screen,sgob *objs,int nob,REALN zfog,REALN zmax,sgob *cam);

void renderer_release();
