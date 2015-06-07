GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)


all: 
	g++ robot_arm.cpp Common/InitShader.o $(OPTIONS) -o robot_arm 

clean:
	rm -f *.o robot_arm
