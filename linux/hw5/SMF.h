#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include "Angel.h"


class SMF
{
public:
	void loadFile(std::string filePath);
	int numberOfVertices();
	std::vector<vec3> getVertices();
	std::vector<vec3> getFaces();

private:
	void printHeader(std::string);
	void addVertex(std::string line);
	void addFace(std::string line);
	void addHeader(std::string line, std::string& header);
	void printVertices();

	std::vector<vec3> vertices;
	std::vector<vec3> faces;

};


