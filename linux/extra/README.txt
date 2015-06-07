
====== CS637 Assignment Extra ======

Rafael Campos - 13890411
rcc62@drexel.edu


APPLICATION
-------------------------------
As per the assignment specifications, the application:
 - displays an arm configuration, composed of 
  base, lower arm and upper arm, with independent 
  movement.

To move each part, it has to be selected using the menu
accessible with a right-click. Each increment in angle
is obtained by a left-click.

Keyboard control can change the view.
The camera uses spherical coordinates.



DEVELOPMENT
-------------------------------
The application was first developed on Mac OS X 10.10 on a text editor and compiled with Clang from the Apple version of the source code on the course website.
In order to compile it, the instruction -stdlib=libstdc++  had to be added to the Makefile.



HOW TO COMPILE
-------------------------------
On the icg VM, the two targets are compiled at the same time and there is no need to specify its name:

 1. make
 2. ./robot_arm
 4. make clean (is supported and can be used if desired)

During execution on the VM, I get the errors below on the terminal, but the application still runs correctly.

libGL error: pci id for fd 4: 80ee:beef, driver (null)
libGL error: core dri or dri2 extension not found
libGL error: failed to load driver: vboxvideo

