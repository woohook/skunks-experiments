/*
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

#include "framework.h"
#include "platform.h"
#include "clock.h"
#include "physics.h"
#include "vehicle.h"
#include "render32.h"
#include "surface.h"
#include "input.h"
#include "skunks2.h"

int g_argc = 0;
char** g_argv = 0;
int g_exitcode = 0;
int g_shutdown_request = 0;

int main(int argc,char *argv[])
{
  g_argc = argc;
  g_argv = argv;

  platform_initialize();
  clock_initialize();
  physics_init();
  vehicle_initialize();
  renderer_initialize();
  surface_initialize();
  input_initialize();
  skunks_initialize();

  while(g_shutdown_request == 0)
  {
    platform_process();
    clock_process();
    physics_process();
    vehicle_process();
    renderer_process();
    surface_process();
    input_process();
    skunks_process();
  }


  skunks_release();
  input_release();
  surface_release();
  renderer_release();
  vehicle_release();
  physics_release();
  clock_release();
  platform_release();

  return g_exitcode;
}

void framework_get_args(int* argc, char*** argv)
{
  *argc = g_argc;
  *argv = g_argv;
}

void framework_request_shutdown(int exitcode)
{
  g_exitcode = exitcode;
  g_shutdown_request = 1;
}
