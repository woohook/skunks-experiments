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
void translat(struct _sgob *objs,REALD x,REALD y,REALD z);

// function which translates a vector (x,y,z) by dx,dy and dz
void translate_vector(REALD* x,REALD* y,REALD* z, REALD dx,REALD dy,REALD dz);

/*functie care roteste un obiect in jurul unei axe paralele cu z care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatz(struct _sgob *objs,REALD x,REALD y,REALD tt);

void rotate_vector_z(REALD* x,REALD* y, REALD rx, REALD ry, REALD tt);

/*functie care roteste un obiect in jurul unei axe paralele cu y care intersecteaza
planul xOz intr-un punct de coordonate x si z*/
void rotaty(struct _sgob *objs,REALD x,REALD z,REALD tt);

void rotate_vector_y(REALD* x,REALD* z, REALD rx, REALD rz, REALD tt);

/*functie care roteste toate triunghiurile in jurul unei axe paralele cu x care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatx(struct _sgob *objs,REALD y,REALD z,REALD tt);

void rotate_vector_x(REALD* y,REALD* z, REALD ry, REALD rz, REALD tt);

/*functie care roteste un obiect in jurul unei axe oarecare care trece prin A1(x,y,z) si B(xb,yb,zb)*/
void rotab(struct _sgob *objs,REALD x,REALD y,REALD z,REALD xb,REALD yb,REALD zb,REALD tt);

void rotate_vector_ab(REALD* x,REALD* y,REALD* z, REALD xa, REALD ya, REALD za, REALD xb, REALD yb, REALD zb, REALD tt);
