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

#include <SDL.h>
#include "surface.h"

struct _surface
{
  SDL_Surface *screen;
  Uint8* current_pixel;
} surface;

SDL_Window *RGLOBwindow = NULL;

const int bits_per_pixel = 32;

struct _surface* surface_create(int width, int height)
{
  struct _surface* pSurface = NULL;

  if(RGLOBwindow==NULL)
  {
    RGLOBwindow = SDL_CreateWindow("Skunks-4.2.0 SDL2",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_SHOWN);
    if(RGLOBwindow==NULL)
    {
      printf("Couldn't create window: %s\n",SDL_GetError());
      SDL_Quit();
      return NULL;
    }
  }
  pSurface = (struct _surface*)malloc(sizeof(struct _surface));
  pSurface->screen = SDL_GetWindowSurface(RGLOBwindow);
  printf("Surface created: %dx%dx%d\n",(pSurface->screen->pitch)/(pSurface->screen->format->BytesPerPixel),height,pSurface->screen->format->BitsPerPixel);

  return pSurface;
}

int surface_get_width(struct _surface* pSurface)
{
  return pSurface->screen->w;
}

int surface_get_height(struct _surface* pSurface)
{
  return pSurface->screen->h;
}

void surface_begin_rendering(struct _surface* pSurface)
{
  if(SDL_MUSTLOCK(pSurface->screen))
  {
    if(SDL_LockSurface(pSurface->screen)<0)
    {
      printf("Can't lock screen: %s\n", SDL_GetError());
      SDL_Quit();
      return;
    }
  }
}

void surface_end_rendering(struct _surface* pSurface)
{
  if(SDL_MUSTLOCK(pSurface->screen))
  {
    SDL_UnlockSurface(pSurface->screen);
  }

  SDL_UpdateWindowSurface(RGLOBwindow);
}

void surface_set_current_pixel(struct _surface* pSurface, int x, int y)
{
  pSurface->current_pixel = (Uint8 *)pSurface->screen->pixels + y*pSurface->screen->pitch + x*(bits_per_pixel/8);
}

void surface_advance_current_pixel(struct _surface* pSurface)
{
  pSurface->current_pixel += (bits_per_pixel/8);
}

void surface_set_current_pixel_color(struct _surface* pSurface, int red, int green, int blue)
{
  pSurface->current_pixel[0] = blue;
  pSurface->current_pixel[1] = green;
  pSurface->current_pixel[2] = red;
}

void surface_get_current_pixel_color(struct _surface* pSurface, int* red, int* green, int* blue)
{
  *blue = pSurface->current_pixel[0];
  *green = pSurface->current_pixel[1];
  *red = pSurface->current_pixel[2];
}

void surface_initialize()
{
  if(SDL_Init(SDL_INIT_VIDEO)<0)
  {
    printf("Couldn't initialize SDL: %s\n", SDL_GetError());
    SDL_Quit();
    exit(-1);
  }
}

void surface_process()
{
}

void surface_release()
{
  SDL_Quit();
}
