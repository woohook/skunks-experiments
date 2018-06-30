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

#include "util.h"

typedef struct _sgob
{int otyp; /*number of object type in the global array*/
 REALN radius; /*radius of sphere in which the object is included*/
REALN xmin;
REALN ymin;
REALN zmin;
REALN xmax;
REALN ymax;
REALN zmax;
matrix transform; /*directions of local axes in global coordinates - v[1]=i, v[2]=j, v[3]=k
               and origin of local system, in global coord. - v[0];
               in local coord. vx[1]=1, vy[1]=0, vz[1]=0; vx[2]=0, vy[2]=1, vz[2]=0 etc.
               rotation and translation of object only affect vx, vy, vz,
               and centres; triangles are calculated from vx, vy and vz at rendering*/
int lev; /*level*/
struct physics_instance* physics_object;
} sgob; /*3d object*/


typedef struct _vhc
{int nob; /*number of objects in vehicle*/
int nj; /*number of permanent joints*/

int oid[MAXGEOM]; /*number of object (rendering and collision) in 'objs' array from 'simcar.c'*/
struct physics_instance* parts[MAXGEOM];
dJointID jid[MAXGEOM]; /*permanent joints*/

int jfc[MAXGEOM]; /*function of object attached to joint (see 'ofc' below)*/
int ofc[MAXGEOM]; /*object function: 1-car; 2-trailer; 3-motor wheel; 4-steering wheel;
                    5-motor and steering wheel; 6-passive wheel; 7-trailer wheel*/

float vrx; // steering speed
float af;  // acceleration factor
float bf;  // brake factor

REALN accel;
REALN brake; /*acceleration and brake torques/wheel*/
REALN spring;
REALN damper; /*suspension coefficients*/
REALN mu; /*friction*/

REALN camh;
REALN camd; /*camera position*/
} vhc; /*vehicle*/
