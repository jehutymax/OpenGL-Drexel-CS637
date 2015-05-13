
====== CS637 Assignment 4 ======

Rafael Campos - 13890411
rcc62@drexel.edu


APPLICATION
-------------------------------
As per the assignment specifications, the application:
 - Displays a 3d cube that can be rotated, translated and scaled by the user.



DEVELOPMENT
-------------------------------
The application was first developed on Mac OS X 10.10 on a text editor and compiled with Clang from the Apple version of the source code on the course website.
In order to compile it, the instruction -stdlib=libstdc++  had to be added to the Makefile.

The changes done to red_triangle.cpp had to be re-applied to the tux version of the source code since there were too many differences. The file was renamed to shapes.cpp.


HOW TO USE
-------------------------------
The information below is out printed to the console when executing the application.

 Right-click anywhere on the window to select a transformation. 
	 = / - 	 Increase / Decrease the delta for the selected transformation.
	 a / z 	 Apply / Inverse Apply the current transformation on the X axis.
	 s / x 	 Apply / Inverse Apply the current transformation on the Y axis.
	 d / c 	 Apply / Inverse Apply the current transformation on the Z axis.
	 r 	 Reset all values.
 Press either 'Esc' or 'Enter' to exit. 

HOW TO COMPILE
-------------------------------
On the icg VM, the two targets are compiled at the same time and there is no need to specify its name:

 1. make
 2. ./shapes
 4. make clean (is supported and can be used if desired)

During execution on the VM, I get the errors below on the terminal, but the application still runs correctly.

libGL error: pci id for fd 4: 80ee:beef, driver (null)
libGL error: core dri or dri2 extension not found
libGL error: failed to load driver: vboxvideo

