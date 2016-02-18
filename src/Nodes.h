#pragma once
#include <glm\glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Node
{
public:
	vec3 Pos;

	Node(vec3 p)
	{
		Pos = p;
	}

	Node() {}

	~Node() {}
};

class Indice
{

public:

	Node n1;
	Node n2;

	Indice() {}
};

class Triangle
{
public:
	Node points[3];

	Indice indices[3];
	 
	Triangle() {}

	void GenIndices()
	{
		indices[0].n1 = points[0];
		indices[0].n2 = points[1];

		indices[1].n1 = points[1];
		indices[1].n2 = points[2];

		indices[2].n1 = points[2];
		indices[2].n2 = points[0];
	}
};

class Square
{
public:
	Triangle tris[2];

	Square() {};
};

class Plane
{
public:

	int width;
	int height;

	Node nodes[9];
	Square squares[4];

	Plane(int w, int h)
	{
		width = w;
		height = h;
		//nodes = new Node[width * height];
		//Triangles = new Triangle[((width - 1) * (height - 1) * 6)];
	}

	Plane()
	{
		width = 3;
		height = 3;
		//nodes = new Node[width * height];
		//Triangles = new Triangle[((width - 1) * (height - 1) * 6)];
	}

	void SetNodes()
	{
		int i = 0;
		for (int z = 0; z < height; z++)
		{
			for (int x = 0; x < width; x++)
			{
				nodes[i].Pos = vec3(x,0,z);
				i++;
			}
		}
	}

	void SetConnectins()
	{
		int s = 0;
		for (int i = 0; i < 9; i++)
		{
			Triangle Triangle1;
			Triangle Triangle2;
			if (((i % width) < (width - 1)) && (i < (width * height) - height))
			{
				Triangle1.points[0] = nodes[i + width]; //nw
				Triangle1.points[1] = nodes[i]; //sw
				Triangle1.points[2] = nodes[i + width + 1]; //ne

				Triangle1.GenIndices();
				squares[s].tris[0] = Triangle1;

				Triangle2.points[0] = nodes[i + 1]; //se
				Triangle2.points[1] = Triangle1.points[1]; //sw
				Triangle2.points[2] = Triangle1.points[2]; //ne

				Triangle2.GenIndices();
				squares[s].tris[1] = Triangle2;
				s++;
			}
		}
	}
};

