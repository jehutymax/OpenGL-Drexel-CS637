
#include "SMF.h"

void SMF::loadFile(std::string filePath)
{
	std::ifstream fs(filePath.c_str());
	std::string line;
	std::string header;

	addHeader(" File loaded: " + filePath, header);

	while (std::getline(fs, line))
	{
		if (line.size() > 0)
		{
			// 4 line types: comment/header, vertice, face and others; we'll ignore others.
			if (line[0] == '#') addHeader(line, header);
			else if (line[0] == 'v') addVertex(line);
			else if (line[0] == 'f') addFace(line);
		}
			
	}

	fs.close();

	printHeader(header);
}

void SMF::printVertices()
{
	/*
	for (int i = 0; i < vertices.size(); ++i)
	    {
	        std::cout << vertices[i][0] << " "
	        << vertices[i][1] << " "
	        << vertices[i][2] << std::endl;

	    }

	    */

	std::vector<vec3>::iterator it;
	
	for (it = vertices.begin(); it < vertices.end(); ++it)
	    {
	    	std::cout << "VEC3: " << *it << std::endl;

	    	std::cout << (*it)[0] << " "
	        << (*it)[1] << " "
	        << (*it)[2] << std::endl;
	    }    
}

void SMF::addVertex(std::string line)
{
	float x, y, z; std::string v;
	std::istringstream iss(line);
	iss >> v >> x >> y >> z;
	vertices.push_back(vec3(x, y, z));
}

void SMF::addFace(std::string line)
{
	int a, b, c; std::string f;
	std::istringstream iss(line);
	iss >> f >> a >> b >> c;
	faces.push_back(vec3(a, b, c));
}

void SMF::addHeader(std::string line, std::string& header)
{
	line[0] = ' ';
	header += "\n" + line;
}

void SMF::printHeader(std::string header)
{
	std::cout << header << std::endl << std::endl;
	std::cout << "Vertices: " << vertices.size() << "  Faces: " << faces.size() << std::endl;

//	printVertices(); std::cout << std::endl << std::endl;
}

int SMF::numberOfVertices()
{
	return vertices.size();
}

std::vector<vec3> SMF::getVertices()
{
	return vertices;
}

std::vector<vec3> SMF::getFaces()
{
	return faces;
}

