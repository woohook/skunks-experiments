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

struct _sgob;

/*function which translates an object by x,y and z*/
void translat(struct _sgob *objs,float x,float y,float z);

// function which translates a vector (x,y,z) by dx,dy and dz
void translate_vector(float* x,float* y,float* z, float dx,float dy,float dz);

/*functie care roteste un obiect in jurul unei axe paralele cu z care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatz(struct _sgob *objs,float x,float y,float tt);

void rotate_vector_z(float* x,float* y, float rx, float ry, float tt);

/*functie care roteste un obiect in jurul unei axe paralele cu y care intersecteaza
planul xOz intr-un punct de coordonate x si z*/
void rotaty(struct _sgob *objs,float x,float z,float tt);

void rotate_vector_y(float* x,float* z, float rx, float rz, float tt);

/*functie care roteste toate triunghiurile in jurul unei axe paralele cu x care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatx(struct _sgob *objs,float y,float z,float tt);

void rotate_vector_x(float* y,float* z, float ry, float rz, float tt);

/*functie care roteste un obiect in jurul unei axe oarecare care trece prin A1(x,y,z) si B(xb,yb,zb)*/
void rotab(struct _sgob *objs,float x,float y,float z,float xb,float yb,float zb,float tt);

void rotate_vector_ab(float* x,float* y,float* z, float xa, float ya, float za, float xb, float yb, float zb, float tt);
