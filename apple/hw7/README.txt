
====== CS637 Assignment 6 ======

Rafael Campos - 13890411
rcc62@drexel.edu


APPLICATION
-------------------------------
As per the assignment specifications, the application:
 - Loads a SMF file and displays in a scene, containing 2 point light sources.
 - Gives the user the option to choose Phong Shading or Gouraud Shading, and one of 3 different materials for the model
 - One of the light sources can be controlled.


DEVELOPMENT
-------------------------------
The application was first developed on Mac OS X 10.10 on a text editor and compiled with Clang from the Apple version of the source code on the course website.
In order to compile it, the instruction -stdlib=libstdc++  had to be added to the Makefile.


To implement Gouraud Shading, instead of interpolating the normals, the shading calculation is now performed in the vertex shader, where that happens automatically.

To implement Phong Shading, the calculation is performed in the fragment shader, automatically interpolating the normals for the entire fragment.

In order to make the switch of shaders possible, I implemented a series of manager methods to allow reloading the shaders while the application is still running.


HOW TO USE
-------------------------------
The information below is printed to the console when executing the application.

 	-f [file name]. If no file is specified, the bunny model is loaded (bound-bunny_5k.smf).

    r / R  Increase / Decrease camera cylinder radius.
    o / O  Increase / Decrease camera height.
    p / P  Increase / Decrease camera azimuth. 

    q / Q  Increase / Decrease light2 cylinder radius.
    w / W  Increase / Decrease light2 height.
    e / E  Increase / Decrease light2 azimuth. 

    x / X; y / Y; z / z    Increase / Decrease projection's (left+right), (top+bottom), (near+far), resp.
    <space>  Reset all values.
    Press 'v' to alternate between Parallel and Perspective projection.
 Press either 'Esc' or 'Enter' to exit. 

HOW TO COMPILE
-------------------------------
On the icg VM, the two targets are compiled at the same time and there is no need to specify its name:

 1. make
 2. ./icg_hw6 (or ./icg_hw6 -f [file name])
 4. make clean (is supported and can be used if desired)

During execution on the VM, I get the errors below on the terminal, but the application still runs correctly.

libGL error: pci id for fd 4: 80ee:beef, driver (null)
libGL error: core dri or dri2 extension not found
libGL error: failed to load driver: vboxvideo

