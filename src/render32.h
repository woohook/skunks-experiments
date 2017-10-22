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

extern char textglob[MAXWLG];

typedef struct _pixcol
{int red;int green;int blue; /*culoarea pixelului*/
} pixcol;

typedef struct _tria
{REALN x1; REALN y1;REALN z1;
REALN x2; REALN y2;REALN z2;
REALN x3; REALN y3;REALN z3;
int red; int green; int blue; /*culoarea triunghiului*/
int redd; int greend; int blued;
unsigned char cull; /*first bit(&1):1-cull;0-no cull;second bit(&2):1-fullbright;0-usual*/
} tria;

typedef struct _tripf
{REALN x1; REALN y1;
REALN x2; REALN y2;
REALN x3; REALN y3;
} tripf;

typedef struct _trilim
{int imin;int imax;} trilim;

typedef struct _lightpr
{REALN ambient;
REALN headlight;
REALN directional;
REALN dx;
REALN dy;
REALN dz;
} lightpr; /*light parameters*/

/*functie care elimina triunghiurile care sunt in plus*/
int fclip(tria *face,int nrfaces,REALN zmin,tria *facedisp,REALN zmax,REALN tgh,REALN tgv);


/*functie care determina ecuatia planului cu numarul de ordine 'i'
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nrfaces - numar total de triunghiuri*/
void findplan(tria *face,int i,REALN *a,REALN *b,REALN *c,REALN *d);


void displaysdl(SDL_Surface *screen,tria *face,int nrfaces,REALN *distmin,unsigned int width,unsigned int height,REALN focal,pixcol backcol,REALN zfog,REALN zmax,lightpr *light);
