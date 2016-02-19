#pragma once
#include <glm\glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Vert
{
	vec4 Pos;
	vec4 Color;
};

class Plane
{
public:
	unsigned int row;
	unsigned int cols;

	unsigned int *indicies;
	Vert *vertices;
	
	Plane(unsigned int r, unsigned int c) 
	{
		row = r;
		cols = c;
		vertices = new Vert[row * cols];
		indicies = new unsigned int [(row - 1) * (cols - 1) * 6];
	}

	Plane()
	{
		row = 5;
		cols = 5;
		vertices = new Vert[row * cols];
		indicies = new unsigned int[(row - 1) * (cols - 1) * 6];
	}

	void GenIndicies()
	{
		int index = 0;
		for (unsigned int r = 0; r < (row- 1); r++)
		{
			for (unsigned int c = 0; c < (cols - 1); c++)
			{
				indicies[index++] = r * cols + c;
				indicies[index++] = (r + 1) * cols + c;
				indicies[index++] = (r + 1) * cols + (c + 1);

				indicies[index++] = r * cols + c;
				indicies[index++] = (r + 1) * cols + (c + 1);
				indicies[index++] = r * cols + (c + 1);
			}
		}
	}
};

