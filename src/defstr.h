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

typedef struct _refpo
{int nref; /*number of reference points*/
REALD x[MAXREF];
REALD y[MAXREF];
REALD z[MAXREF]; /*coordinates of the reference points*/
char gtip[MAXGEOM]; /*type of geom - 'b', 'c', 's', 't' (box, cylinder, sphere, triangle mesh)*/
int ttip[MAXGEOM]; /*type of triangle mesh - 1-left 45; 2-right 45; 3-left 22.5; 4-right 22.5*/
REALN lx[MAXGEOM]; /*geometry parameters; for cyl. - lx = radius, ly = length;*/
REALN ly[MAXGEOM]; /*for sphere - lx = radius; for box - lx = lx, ly = ly, lz = lz.*/
REALN lz[MAXGEOM];
int nfa; /*number of triangles (for rendering)*/
} refpo; /*reference points of an object type*/


typedef struct _sgob
{int otyp; /*number of object type in the global array*/
 int nref; /*number of reference points - max. MAXREF*/
 int nfa; /*number of triangles*/
 REALN radius; /*radius of sphere in which the object is included*/
REALD xref[MAXREF];
REALD yref[MAXREF];
REALD zref[MAXREF]; /*coordinates of the reference points in global coordinates*/
REALD xcen;
REALD ycen;
REALD zcen; /*coordinates of the object's centre in global coordinates*/
REALN xmin;
REALN ymin;
REALN zmin;
REALN xmax;
REALN ymax;
REALN zmax;
REALD vx[4];
REALD vy[4];
REALD vz[4]; /*directions of local axes in global coordinates - v[1]=i, v[2]=j, v[3]=k
               and origin of local system, in global coord. - v[0];
               in local coord. vx[1]=1, vy[1]=0, vz[1]=0; vx[2]=0, vy[2]=1, vz[2]=0 etc.
               rotation and translation of object only affect vx, vy, vz, ref. points
               and centres; triangles are calculated from vx, vy and vz at rendering*/
int lev; /*level*/
dGeomID gid[MAXGEOM]; /*geoms for collision; axis z of geom[i] is defined by
                         (x[i*2-1],y[i*2-1],z[i*2-1]) and (x[i*2],y[i*2],z[i*2])*/
REALN mu; /*friction*/
} sgob; /*3d object*/


typedef struct _vhc
{int nob; /*number of objects in vehicle*/
int nj; /*number of permanent joints*/
int ncj; /*number of contact joints*/

int oid[MAXGEOM]; /*number of object (rendering and collision) in 'objs' array from 'simcar.c'*/
dMass mass[MAXGEOM];
dBodyID bid[MAXGEOM];
dJointID jid[MAXGEOM]; /*permanent joints*/
dJointID bkm[MAXGEOM]; /*brake motors*/

int jfc[MAXGEOM]; /*function of object attached to joint (see 'ofc' below)*/
int ofc[MAXGEOM]; /*object function: 1-car; 2-trailer; 3-motor wheel; 4-steering wheel;
                    5-motor and steering wheel; 6-passive wheel; 7-trailer wheel*/
dJointID cjid[MAXGEOM]; /*contact joints*/

short int tjflag; /*0-no trailer joint; 1-trailer joint*/
dJointID tjid; /*trailer joint*/

REALN accel;
REALN brake; /*acceleration and brake torques/wheel*/
REALN spring;
REALN damper; /*suspension coefficients*/
REALN mu; /*friction*/

REALN camh;
REALN camd; /*camera position*/
} vhc; /*vehicle*/


tria **fceglob; /*array with triangles and colors of object types*/
refpo *refglob; /*array with reference points of object types*/
dWorldID wglob; /*world for ODE*/
