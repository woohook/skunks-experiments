Skunks-4.2.0 - Car simulation with software 3D rendering (no 3D
               acceleration required)

Copyright 2010-2013 Matei Petrescu


All files here, except for "SDL.dll", "src/COPYING" and those found
in the directory "ODE" are available under the GNU GPL license, version
3 (see the file "src/COPYING" for details). The programs use "SDL 1.2"
library, available at www.libsdl.org, which is distributed
under the GNU LGPL license, version 2 (www.gnu.org) and "Open Dynamics
Engine" (ODE), available at www.ode.org, which is also free
software. More details about ODE and its licenses can be found
in the directory "ODE". It's included in this package, so it
doesn't have to be downloaded separately.

The recommendations below are for GNU/Linux, but should work for any
Unix-type system which has the GNU tools installed.

Some executable files are already available: wskunks.exe (keyboard),
wsk-js.exe (joystick) and wreplay.exe. They were compiled with Wine
(http://winehq.org) and OpenWatcom (http://openwatcom.org).

     1. SOME OBSERVATIONS
     2. INSTALLATION
     3. RUNNING
     4. JOYSTICK / STEERING WHEEL
     5. REPLAY
     6. MAKING NEW TRACKS OR CARS
     7. OPENGL SUPPORT


1. SOME OBSERVATIONS

  * The replay application has no sound. This is a design feature.

  * The OpenGL version is recommended if you have a slow computer
    (ex.: AMD K6-2, 300 MHz) with an old 3D accelerator (ex.: 3Dfx
    Banshee) and an old GNU/Linux distribution (ex.: Fedora Core 1)
    which includes drivers for it.


2. INSTALLATION

SDL library is required. Make sure you have it installed. You should
install it from the sources available at www.libsdl.org or, at
least, get a "sdl-devel" package for your distribution.

Then open a terminal/console window and change the current
directory to "skunks-4.2.0", using the command "cd".

ODE must be installed first; version 0.12 (which I used) is in the
directory "ODE". To install it, type:

./inst-ode

ODE will be installed in "ODE/local". Then type:

./comp

.. if you have SDL 1.2, or:

./comp2

.. if you have SDL 2.

If it doesn't work, try:

chmod 755 *

.. or check the contents of the files "comp", "comp2" and "inst-ode".
You can modify the compiler flags in "comp", according to your system
(for instance, if you have a Pentium 2, you might want to remove
"-mfpmath=sse" and, of course, "-march=pentium3").

For more options, the file "src/config.h" may be edited, which
facilitates the selection of the display resolution, viewing angle
and other parameters. To adjust the viewing distance, modify
variables "zfog" and "zmax" in "skunks.c", "replay.c" and "sk-js.c".


3. RUNNING

First, open a terminal/console window and change the current
directory to "skunks-4.2.0", using the command "cd".

After compilation, you can start the game by typing:

./skunks CARFILE TRACKFILE

Examples:

./skunks cars/car1 tracks/track1

./skunks cars/car3 tracks/track6

11 tracks are available in the directory 'tracks'.

The parameters of the cars can easily be modified. Below are
a few details about the 7 cars:

car1: front wheel drive
car2, car3 and car4: rear wheel drive
car5: 4 wheel drive
car6: bus
car7: truck with trailer

All cars are electric and therefore don't need gearboxes.

The buttons which control the game are:

Q, T or UP - accelerate
A, F or DOWN - brake
O, Y or LEFT - left
P, U or RIGHT - right
R - reverse
C - change camera
N and M - rotate camera
ESCAPE - exit


4. JOYSTICK / STEERING WHEEL

If you want to use a joystick or steering wheel, compile the program
with:

./comp-js

If the compilation succeeds, an executable file called "sk-js" will
be obtained, which can be used like the other one mentioned above:

./sk-js CARFILE TRACKFILE

The joystick configuration is in the file "config/joystick". The
settings there should be appropriate for the case in which you have
a joystick and want to use buttons 1 and 2 for acceleration and
brake. If you have a steering wheel and want to use the pedals, the
following modifications are suggested:

sensx 1
sensy 1
deadx 600
deady 3000
nonlinear 1

I only tested joystick support in Fedora Core 6, where the
following command could be used for calibration:

jscal -c /dev/js0


5. REPLAY

Since version 3.0.0, the game records a replay in 'replays/rep1'. To
keep it from being overwritten, you can rename it to something else.

To watch a replay:

./replay REPLAYFILE

Example:

./replay replays/rep2

Only buttons C, N, M and ESCAPE can be used during replays.

4 replays recorded by me are in the directory 'replays' (rep2 ... rep7).
Watch them.


6. MAKING NEW TRACKS OR CARS

Look in the file "README" from directory "src/other/newtrax" for details
about making new tracks. It's easier than with any track editor of any
other game.

The files which define the parameters of the cars ("cars/car*") can also
be edited. After "objects n", the numbers on each of the next n lines
have the following significances:

1st: object type, from the list above;

2nd: object function (1 - body of vehicle, 2 - trailer, 3 - motor wheel,
     4 - steering wheel, 5 - motor and steering wheel, 6 - wheel,
     7 - wheel attached to the trailer);

3rd, 4th and 5th: coordinates on X, Y and Z.

Axes X, Y and Z are like this (not like in OpenGL):

^ X   __
|     /| Z
|    /
|   /
|  /
| /      Y
|-------->


7. OPENGL SUPPORT

To compile the OpenGL version, look in directory "OpenGL". It probably
won't function. Besides, I find 3D acceleration to be just a nuisance.
Before 1996-1997, when CPUs were too slow for 3D graphics, all games
used the CPU because there were no 3D accelerators. When 3D
accelerators became accessible, they were already unnecessary because
CPUs were more than fast enough to draw 3D graphics on their own. So as
I see it, all these 3D accelerators ever created was lots of problems (Oops,
game doesn't work - needs 3D accelerator; Oops, got 3D accelerator but
doesn't support OpenGL 1.3; Oops, got 3D accelerator with support for
OpenGL 1.3 but can't find driver for my operating system; Oops, got another
accelerator, for which I found and installed the driver, but it still doesn't
work - I wonder why).
