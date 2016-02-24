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
using glm::vec2;
using glm::mat4;
using namespace std;




#define BUFFER_OFFSET(i) ((char *)NULL + (i))

unsigned int m_VAO, m_VBO, m_IBO, m_perlin_texture;
unsigned int m_shader;
struct Vertex {
	vec4 position;
	vec2 texCoord;
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

	//int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	//unsigned char* data = stbi_load("crate.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);
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

	int dims = 64;
	float *perlin_data = new float[dims * dims];
	float scale = (1.0f / dims) * 3;
	for (int x = 0; x < 64; ++x)
	{
		for (int y = 0; y < 64; ++y)
		{
			perlin_data[y* dims + x] = glm::perlin(vec2(x, y) * scale) * 0.5f + 0.5f;
		}
	}
	//PLANE GENERATION
	int rows = 64;
	int cols = 64;

	// create opengl data for a grid
	Vertex* vertices = new Vertex[rows * cols];
	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {

			// offset position so that the terrain is centered
			vertices[r * cols + c].position = vec4(c - cols * 0.5f, 0, r - rows * 0.5f, 1);

			// setting up UVs
			vertices[r * cols + c].texCoord = vec2(c * (1.f / cols), r * (1.f / rows));
		}
	}

	// keep track of number of indices for rendering
	int m_indexCount = (rows - 1) * (cols - 1) * 6;

	unsigned int* indices = new unsigned int[m_indexCount];

	unsigned int index = 0;
	for (int r = 0; r < (rows - 1); ++r) {
		for (int c = 0; c < (cols - 1); ++c) {
			// triangle 1
			indices[index++] = r * cols + c;
			indices[index++] = (r + 1) * cols + c;
			indices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2
			indices[index++] = r * cols + c;
			indices[index++] = (r + 1) * cols + (c + 1);
			indices[index++] = r * cols + (c + 1);
		}
	}

	//BUFFER GENERATION
	// generate buffers
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	// generate vertex array object (descriptors)
	glGenVertexArrays(1, &m_VAO);

	// all changes will apply to this handle
	glBindVertexArray(m_VAO);

	// set vertex buffer data
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(
		GL_ARRAY_BUFFER, //type of buffer to bind to
		(rows * cols) * sizeof(Vertex), //how large should the buffer be
									   //in this example we have 64 * 64 elements
									   //they are of size (Vertex) which is the size of a glm::vec4 and a glm::vec2
		vertices, //the actual data
		GL_STATIC_DRAW);

	// index data
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	// texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	// safety
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	/*	In order to see our generated data, we are going to create a texture,
	fill it with the noise data, and display it on our quad*/

	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	// bind data as float for a single channel
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
		dims, dims, 0, GL_RED, GL_FLOAT, perlin_data);

	// enable blending else samples must be "exact" centre of texels
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// set wrap to stop errors at edge of noise sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	//setup some matricesa
	mat4 m_model = mat4();
	mat4 m_view = lookAt(vec3(60.0, 30.0, 60.0), vec3(0.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0));
	mat4 m_projection = glm::perspective(glm::pi<float>()*0.25f, 16 / 9.f, 0.1f, 1000.f);
	mat4 m_projectionViewMatrix = m_projection * m_view;
	//end setup matrices

	unsigned int projectionViewUniform = glGetUniformLocation(m_shader, "ProjectionViewModel");
	//start using shader...
	glUseProgram(m_shader);
	//because we are sending it to the uniform with this function
	glUniformMatrix4fv(projectionViewUniform, 1, false, value_ptr(m_projectionViewMatrix));

	int texUniform = glGetUniformLocation(m_shader, "noiseTexture");
	glUniform1i(texUniform, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		unsigned int modelID = glGetUniformLocation(m_shader, "Model");
		////float time = glfwGetTime();
		////m_model = rotate(mat4(), 5.0f * cos(time), vec3(0, 1, 0));
		glUniformMatrix4fv(modelID, 1, false, value_ptr(m_model));

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

		//draw

		glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
		//unbind

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}