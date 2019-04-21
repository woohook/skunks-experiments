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

struct _surface* surface_create(int width, int height);

int surface_get_width(struct _surface* pSurface);

int surface_get_height(struct _surface* pSurface);

void surface_begin_rendering(struct _surface* pSurface);

void surface_end_rendering(struct _surface* pSurface);

void surface_set_current_pixel(struct _surface* pSurface, int x, int y);

void surface_advance_current_pixel(struct _surface* pSurface);

void surface_set_current_pixel_color(struct _surface* pSurface, int red, int green, int blue);

void surface_get_current_pixel_color(struct _surface* pSurface, int* red, int* green, int* blue);

void surface_initialize();

void surface_process();

void surface_release();
