#define GLM_FORCE_PURE
#include <vector>
#include "stdio.h"
#include <gl_core_4_4.h>
#include <GLFW/glfw3.h> 
#include <glm/fwd.hpp>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <iostream>
#include <fstream>
#include <string> 
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using glm::vec3;
using glm::vec4;
using glm::mat4;
using namespace std;




#define BUFFER_OFFSET(i) ((char *)NULL + (i))

unsigned int m_VAO, m_VBO, m_IBO, m_perlin_texture;
unsigned int m_shader;
struct Vertex {
	vec4 position;	 
};

void generateGrid(unsigned int rows, unsigned int cols, std::vector<vec3> &, std::vector<int>&);
// function to create a grid
void generateGrid(unsigned int rows, unsigned int cols, std::vector<vec3> &verts, std::vector<int> &indices)
{
	for (unsigned int r = 0; r < rows; ++r) {
		for (unsigned int c = 0; c < cols; ++c)
		{			
			verts.push_back(vec3((float)c, 0, (float)r));
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			int p0 = r * cols + c;
			int p1 = (r + 1) * cols + c;
			int p2 = (r + 1) * cols + (c + 1);
			int p3 = r * cols + c;
			int p4 = (r + 1) * cols + (c + 1);
			int p5 = r * cols + (c + 1);
			indices.push_back(p0);
			indices.push_back(p1);
			indices.push_back(p2);
			indices.push_back(p3);
			indices.push_back(p4);
			indices.push_back(p5);

		}
	}

}

string LoadShader(std::string file)
{
	string line, shader;
	ifstream inFile(file);
	if (inFile.is_open())
		while (getline(inFile, line))
		{
			shader.append(line);
			shader.append("\n");
		}
	inFile.close();
	return shader;
}

//Setup some geometry to draw
//MyVertex will be the storage container for our vertex information
static const GLfloat cube_vertices[] =
{
	// front
	-1.0, -1.0,  1.0,//0
	1.0, -1.0,  1.0, //1
	1.0,  1.0,  1.0, //2
	-1.0,  1.0,  1.0,//3
	// back
	-1.0, -1.0, -1.0,//4
	1.0, -1.0, -1.0, //5
	1.0,  1.0, -1.0, //6
	-1.0,  1.0, -1.0,//7
};

// RGB color triples for every coordinate above.
static const GLfloat cube_colors[] = {
	// front colors
	1.0, 0.0, 0.0,//0
	0.0, 1.0, 0.0,//1
	0.0, 0.0, 1.0,//2
	1.0, 1.0, 1.0,//3
	// back colors
	1.0, 0.0, 0.0,//4
	0.0, 1.0, 0.0,//5
	0.0, 0.0, 1.0,//6
	1.0, 1.0, 1.0,//7
};

unsigned int cube_elements[] =
{
	// front
	0, 1, 2,
	2, 3, 0,
	// top
	1, 5, 6,
	6, 2, 1,
	// back
	7, 6, 5,
	5, 4, 7,
	// bottom
	4, 0, 3,
	3, 7, 4,
	// left
	4, 5, 1,
	1, 0, 4,
	// right
	3, 2, 6,
	6, 7, 3,
};

int main()
{
	//initialize the opengl window
	if (glfwInit() == false)
		return -1;

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Computer Graphics", nullptr, nullptr);

	if (window == nullptr)
	{
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	//the rest of our code goes here!
	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	//testing what version of OpenGL we are running
	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf_s("GL: %i.%i\n", major, minor);

	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load("crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);
	//done initialize window and OpenGL

	//BEGIN SHADER SETUP
	string vshader = LoadShader("VertexShader.vert");
	string fshader = LoadShader("FragmentShader.frag");
	const char* vsSource = vshader.c_str();
	const char* fsSource = fshader.c_str();
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, (const char**)&vsSource, 0);
	glCompileShader(vs);

	glShaderSource(fs, 1, (const char**)&fsSource, 0);
	glCompileShader(fs);

	m_shader = glCreateProgram();
	glAttachShader(m_shader, vs);
	glAttachShader(m_shader, fs);
	glBindAttribLocation(m_shader, 0, "Position");
	glBindAttribLocation(m_shader, 1, "TexCoord");
	glLinkProgram(m_shader);

	int success = GL_FALSE;
	glGetProgramiv(m_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(m_shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(m_shader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link Gizmo shader program!\n%s\n", infoLog);
		delete[] infoLog;
	}
	//END SHADER SETUP




	//Now we put it on the graphics card
	//generate your buffer on the graphics card
	//this contains all the vertices
	vector<vec3> plane;
	vector<int> indices;



	generateGrid(64, 64, plane, indices);

	 
	int numVertices = plane.size();
	int numColors = sizeof(cube_colors) / sizeof(GLfloat);
	int vertOffset = plane.size() * sizeof(vec3);
	int colorOffset = numColors * sizeof(cube_colors);

	//int numIndices = sizeof(cube_elements) / sizeof(unsigned int);
	int numIndices = indices.size();

	printf("numVertices: %d \n", numVertices);
	printf("numColors: %d \n", numColors);
	printf("numIndices: %d \n", numIndices);

	int dims = 10;
	float perlin_data[100];
	float scale = (1.0f / dims) * 3;
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			perlin_data[y * dims + x] = glm::perlin(glm::vec2(x, y) * scale) * 0.5f + 0.5f;
		}
	}
	
	//Texture tutorial
	float vertexData[] = {
		-5, 0, 5, 1, 0, 1,
		5, 0, 5, 1, 1, 1,
		5, 0, -5, 1, 1, 0,
		-5, 0, -5, 1, 0, 0,
	};
	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};




	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 10, 10, 0, GL_RED, GL_FLOAT, perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//stbi_image_free(data);

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(perlin_data),
		perlin_data,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 
		vertOffset + colorOffset,
		NULL,
		GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 
		0,
		plane.size() * sizeof(vec3),
		plane.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertOffset, colorOffset, &cube_colors[0]);



	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)vertOffset);
	//unbind now that we have generated and populated


	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	


	//setup some matricesa
	mat4 m_model = mat4();
	mat4 m_view = lookAt(vec3(1.0, 5.0, 1.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 1.0, 0.0));
	mat4 m_projection = glm::perspective(glm::pi<float>()*0.25f, 16 / 9.f, 0.1f, 1000.f);
	mat4 m_projectionViewMatrix = m_projection * m_view;
	//end setup matrices

	unsigned int projectionViewUniform = glGetUniformLocation(m_shader, "ProjectionView");
	//start using shader...
	glUseProgram(m_shader);
	//because we are sending it to the uniform with this function
	glUniformMatrix4fv(projectionViewUniform, 1, false, value_ptr(m_projectionViewMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	projectionViewUniform = glGetUniformLocation(m_shader, "diffuse");
	glUniform1i(projectionViewUniform, 0);

	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int modelID = glGetUniformLocation(m_shader, "Model");
		//float time = glfwGetTime();
		//m_model = rotate(mat4(), 5.0f * cos(time), vec3(0, 1, 0));
		glUniformMatrix4fv(modelID, 1, false, value_ptr(m_model));

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

		//draw

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
		//unbind

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}





