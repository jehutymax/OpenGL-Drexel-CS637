GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-lGLEW -lGL -lglut
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)


all: 
	g++ shapes.cpp Common/InitShader.o $(OPTIONS) -o shapes 

clean:
	rm -f *.o shapes
