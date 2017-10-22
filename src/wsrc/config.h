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

#define REALN float
/*this can be "float" or "double"*/

#define REALD float
/*this 2*/

int SDLSCREENFLAG=SDL_SWSURFACE;
/*SDL_FULLSCREEN or SDL_SWSURFACE*/

#define CLBITS 32
/*number of colour bits; can be 16, 24 or 32*/

#define DOUBLEPIX 0
/*1 - large pixels; 0 - normal pixels*/

#define FOV 90
/*horizontal viewing angle (in degrees)*/

#define ASPCOR 1
/*1 - aspect correction; 0 - no aspect correction (WIDTHFACTOR ignored)*/

float WIDTHFACTOR=1.0;
/*0.78125 recommended for monitor with 1024x600 resolution, set at 800x600
(if the video driver doesn't support 1024x600)*/

#define MAXREF 109
/*maximum number of reference points for 1 object*/

#define MAXGEOM 55
/*maximum number of geoms (ODE) for 1 object (=MAXREF/2)*/

#define MAXWLG 41
/*maximum word length*/

#define STIMESTEP 0.005
/*desired simulation time step (seconds)*/

#define GRAVITY -9.81
/*gravitational acceleration (m/s2)*/

#define REPSTEPS 8
/*save replay data every REPSTEPS simulation steps*/

#define REPLAY 1
/*1 - record replay; 0 - don't record*/

#define SOUND 1
/*1 - sound; 0 - no sound*/
