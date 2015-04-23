GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-framework OpenGL -framework GLUT 
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)


#.cpp: 
#	g++ $@.cpp Common/initShader.o $(OPTIONS) -o $@  

all:
	g++ -stdlib=libstdc++ -c -I include Common/InitShader.cpp
	g++ -stdlib=libstdc++ shapes.cpp initShader.o $(OPTIONS) -o shapes

clean:
	rm -f *.o shapes