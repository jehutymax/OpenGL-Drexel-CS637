GCC_OPTIONS=-Wall -pedantic -I include
GL_OPTIONS=-framework OpenGL -framework GLUT 
OPTIONS=$(GCC_OPTIONS) $(GL_OPTIONS)


#.cpp: 
#	g++ $@.cpp Common/initShader.o $(OPTIONS) -o $@  

all:
	g++ -stdlib=libstdc++ -c -I include Common/InitShader.cpp
	g++ -stdlib=libstdc++ -c -I include SMF.cpp
	g++ -stdlib=libstdc++ shapes.cpp initShader.o SMF.o $(OPTIONS) -o icg_hw6

nowarning: 
	g++ -stdlib=libstdc++ -c -I include Common/InitShader.cpp
	g++ -stdlib=libstdc++ -c -I include -w -g SMF.cpp
	g++ -stdlib=libstdc++ shapes.cpp initShader.o SMF.o -w -g -I include -framework OpenGL -framework GLUT -o icg_hw6

clean:
	rm -f *.o icg_hw6