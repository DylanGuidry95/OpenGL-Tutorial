#include "stdio.h"
#include <gl_core_4_4.h> 
#include <GLFW/glfw3.h> 
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include "MyApplication.h"

//Link to OpenGL 4.5 refrence https://www.opengl.org/sdk/docs/man4/

namespace tinyobj
{

	typedef struct mesh_t
	{
		std::vector<float>			positions;
		std::vector<float>			normals;
		std::vector<float>			texcoords;
		std::vector<unsigned int>	indices;
		std::vector<int>			material_ids;
	};

	typedef struct shape_t
	{
		std::string			name;
		mesh_t				mesh;
	};


}

struct OpenGLInfo
{
	// vertex and index buffers
	unsigned int m_VAO; //Vertex Alligned Object
	unsigned int m_VBO; //Vertex Buffer Object
	unsigned int m_IBO; //Index Buffer Object
	unsigned int m_index_count; //Number of indexes
};

//Represents what makes up a Vertex obj
struct Vertex
{
	vec4 position;
	vec4  color;
};	

std::vector<OpenGLInfo> m_gl_info;

unsigned int programID;

GLFWwindow *window;

//Creates and binds buffers to a vertex array object
void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());
	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); ++mesh_index) 
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO); //N (Specifies num of vertex array object names to gen); Arrays (Specifies an array in which the gen vert array is stored)
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO); //N (Specifies num of buffer obj names to be gen); buffers (Specifies an array in which the gen buffer obj names are stored)
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);  
		glBindVertexArray(m_gl_info[mesh_index].m_VAO); //Array (Specifies the name of the vertex array to bind)

		unsigned int float_count = shapes[mesh_index].mesh.positions.size();  
		float_count += shapes[mesh_index].mesh.normals.size(); 
		float_count += shapes[mesh_index].mesh.texcoords.size();

		std::vector<float> vertex_data;		
		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.positions.begin(), 
			shapes[mesh_index].mesh.positions.end()); 
		
		vertex_data.insert(vertex_data.end(), 
			shapes[mesh_index].mesh.normals.begin(),
			shapes[mesh_index].mesh.normals.end());
		
		m_gl_info[mesh_index].m_index_count = 
			shapes[mesh_index].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO); //Target (Specifies the target to which the buffer obj is bound); buffer (Spcifies the name of a buffer obj) 
		
		//Target (Specifes the target which the buffer obj is bound for glBufferData)
		//Buffer (Specifies the size in bytes of the buffer obj's new data store)
		//Data (Specifies a pointer to data that will be copied into the data store for initilization, or NULL if no data is to be copied)
		//Usage (Specifies the expected usage pattern of the data store. The symbol constant must be 
						//(GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY, GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY, GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, or GL_DYNAMIC_COPY)
		glBufferData(GL_ARRAY_BUFFER, 
					vertex_data.size() * sizeof(float),
					vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
					shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int), 
					shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal data  
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);  
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 
		(void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
		
		glBindVertexArray(0);  
		glBindBuffer(GL_ARRAY_BUFFER, 0);  
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
}

//When called generates a grid in the scene view
void GenGrid(unsigned int rows, unsigned int cols)
{
	mat4 projectionViewMatrix = glm::perspective(glm::pi<float>()*0.25f, 16 / 9.f, 0.1f, 1000.f);
	Vertex aoVertices[2 * 2];
	OpenGLInfo glInfo;

	for (unsigned int r = 0; r < 2; r++)
	{
		for (unsigned int c = 0; c < 2; c++)
		{
			aoVertices[r * 2 + c].position = vec4((float)c, 0, (float)r, 1);
			//Create some arbitrary color based off something
			//that might not be related to tiling or texture
			vec3 color = vec3(sinf((c / (float)(2 - 1)) * (r / (float)(2 - 1))));
			aoVertices[r * 2 + c].color = vec4(color, 1);
		}
	}
	//Creates an index array, as the grid is drawn in a series of quads.
	//defining the index count based off quad count (2 triangles per quad)
	unsigned int auiIndices[(2 - 1) * (2 - 1) * 6];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (2 - 1); r++)
	{
		for (unsigned int c = 0; c < (2 - 1); c++)
		{
			//Triangle 1
			auiIndices[index++] = r * 2 + c;
			auiIndices[index++] = (r + 1) * 2 + c;
			auiIndices[index++] = (r + 1) * 2 + c + 1;
			//Triangle 2
			auiIndices[index++] = r * 2 + c;
			auiIndices[index++] = (r + 1) * 2 + (c + 1);
			auiIndices[index++] = r * 2 + (c + 1);
		}
	}

	///Creating our Vertex Array Objects 
	//Gen our GL Buffers
	glGenBuffers(1, &glInfo.m_VBO); //Vertex Buffer Object
	glGenBuffers(1, &glInfo.m_IBO); //Index Buffer Object

									//Generate Vertex Array Object
	glGenVertexArrays(1, &glInfo.m_VAO);

	glBindVertexArray(glInfo.m_VAO);

	//Binds and fills the Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, glInfo.m_VBO); //Bind names to the buffer obj
	glBufferData(GL_ARRAY_BUFFER, (2 * 2) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW); //Copies the names of the buffers to there new place in memory

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(vec4)));

	//Binds and fills the Index Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glInfo.m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (2 * 2) * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(programID);
	unsigned int projectionViewUniform = glGetUniformLocation(programID, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(projectionViewMatrix));

	glBindVertexArray(glInfo.m_VAO);
	unsigned int indexCount = (2 - 1) * (2 - 1) * 6;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	//delete[] aoVertices;
}

void CreateShaders()
{
	//Creates the shaders
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Color; \
							out vec4 vColor; \
							uniform mat4 ProjectionView; \
							uniform float time; \
							uniform float heightScale; \
							void main() { vColor = Color; vec4 P = Position; \
							P.y += sin(time + Position.x) * heightScale; gl_Position = ProjectionView * P;}" ;

	const char* fsSource = "#version 410\n \
							in vec4 vColor; \
							out vec4 FragColor; \
							void main() {FragColor = vColor;} ";


	//Compiles the shaders
	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	//glLinkProgram(programID);
	glBindAttribLocation(programID, 1,"Position");
	glBindAttribLocation(programID, 1, "Color");

	glGetProgramiv(programID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(programID, infoLogLength, 0, infoLog);

		printf("Error: failed to link shader program! \n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

int StartUp()
{
	if (glfwInit() == false)
		return -1;

	window = glfwCreateWindow(1280,720,"OpenGL", nullptr, nullptr);

	if (window == nullptr)
		glfwTerminate();

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
	{
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf_s("GL: %i.%i\n", major, minor);

	CreateShaders();

	return 1;
}

int Update()
{
	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return 1;
	}

	return 0;
}

int main()
{
	//std::vector<tinyobj::shape_t> shapes;
	//	Application *theApp = new Application();
	//	
	//	if(theApp->startup() == true) 
	//	{
	//		while (theApp->update() == true)
	//		{
	//			theApp->draw();
	//		}
	//		theApp->shutdown();
	//	}
	//	delete theApp;

	if (StartUp() == true)
	{

		while (Update() == true)
		{
			GenGrid(2, 2);
			glfwSwapBuffers(window);
			glfwPollEvents();
			
		}

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	return 0;
}