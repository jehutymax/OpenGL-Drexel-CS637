GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)


all: 
	g++ SMF.cpp $(OPTIONS) -c
	g++ shapes.cpp Common/InitShader.o SMF.o $(OPTIONS) -o icg_hw9_2d 

clean:
	rm -f *.o icg_hw9_2d
