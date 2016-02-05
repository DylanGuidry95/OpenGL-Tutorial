#include "stdio.h"
#include <gl_core_4_4.h> 
#include <GLFW/glfw3.h> 
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include "MyApplication.h"
#include <fstream>

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

//Defines what makes up a Vertex obj
struct Vertex
{
	vec3 position;
	vec3  color;
};	

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

std::vector<OpenGLInfo> m_gl_info;

//Screen Resoulution
float WINDOW_WIDTH = 1600;
float WINDOW_HEIGHT = 900;

unsigned int programID; //Value that represent our application

GLFWwindow *window; //Represents our window

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
	mat4 m_view = glm::lookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	mat4 m_projection = glm::perspective(glm::pi<float>()*0.25f, 16.f /9.f, 0.1f, 100.f);
	mat4 m_projectionViewMatrix = m_projection * m_view;
	vec4 aoVertices[3]; //Array to stroe the 3 verts of a triangle
	OpenGLInfo glInfo; //Used to refrence the struct that stores the VBO VAO IBO
	
	//Statictly set all verts in the aoVerticies array
	//Left
	aoVertices[0].x = -2.0;
	aoVertices[0].y = 0.0;
	aoVertices[0].z = 0.0; 
	aoVertices[0].w = 1.0;

	//Right
	aoVertices[1].x = 2.0;
	aoVertices[1].y = 0.0;
	aoVertices[1].z = 0.0; 
	aoVertices[1].w = 1.0;
	
	//top
	aoVertices[2].x = 0.0;
	aoVertices[2].y = 2.0;
	aoVertices[2].z = 0.0; 
	aoVertices[2].w = 1.0;

	//Staticly sets the index number for the verts
	unsigned int auiIndices[3];
	auiIndices[0] = 0;
	auiIndices[1] = 1;
	auiIndices[2] = 2;

	///Creating our Vertex Array Objects 
	//Gen our GL Buffers
	glGenBuffers(1, &glInfo.m_VBO); //Vertex Buffer Object
	glBindBuffer(GL_ARRAY_BUFFER, glInfo.m_VBO); //Bind names to the buffer obj
	glBufferData(GL_ARRAY_BUFFER, (3) * sizeof(vec4), &aoVertices, GL_STATIC_DRAW); //Copies the names of the buffers to there new place in memory on the GPU

	glGenBuffers(1, &glInfo.m_IBO); //Index Buffer Object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glInfo.m_IBO);//Bind names to the buffer obj
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (3) * sizeof(unsigned int), &auiIndices, GL_STATIC_DRAW);	//Copies the names of the buffers to the new place in memory on the GPU

	glGenVertexArrays(1, &glInfo.m_VAO); //Generates the Vertex Array
	glBindVertexArray(glInfo.m_VAO); //Binds the Vertex Array
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glInfo.m_IBO); //Binds the Index buffer to the corresponding index
	glEnableVertexAttribArray(0); //Grabs the shader at the index of 0 (Vertex Shader)
	glEnableVertexAttribArray(1); //Grabs the shader at the index of 1 (Fragmant Shader)
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), BUFFER_OFFSET(0)); //Points to the object we are trying to draw to the corersponding shaders
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), BUFFER_OFFSET(12));

	//Binds and fills the Index Buffer Object
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(programID); //Tells the system what shader or program we are using to render the objects
	unsigned int projectionViewUniform = glGetUniformLocation(programID, "ProjectionView"); //Sets up the view for the window
	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionViewMatrix)); //Creates the view for the scene

	glBindVertexArray(glInfo.m_VAO); //Binds the Vertex array object
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, BUFFER_OFFSET(0)); //Draws the verts to the screen in the form of a triangle
}

//Creates and links all the shaders we will be using to draw to the screen when called
//Called in StartUp()
void CreateShaders()
{
	std::string vsSource;
	std::ifstream vshaderStream("VertexShader.vert");
	if (vshaderStream.is_open())
	{
		while (vshaderStream.good())
		{
			std::getline(vshaderStream, vsSource);
		}
		vshaderStream.close();
	}

	std::string fsSource;
	std::ifstream fshaderStream("FragmentShader.frag");
	if (fshaderStream.is_open())
	{
		while (fshaderStream.good())
		{
			std::getline(fshaderStream, fsSource);
		}
		fshaderStream.close();
	}


	//Compiles the shaders
	int success = GL_FALSE; //Check to see if the shader has been linked correctly
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); //Creates the Vertex Shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Creates the Fragment Shader

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0); //Replaces the soource code int the shader obj
	glCompileShader(vertexShader); //Compilies the shader obj
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	programID = glCreateProgram(); //Creates the shader/program that is allowing us to draw to the screen
	glAttachShader(programID, vertexShader); //Attachs the shader to the program
	glAttachShader(programID, fragmentShader);
	glBindAttribLocation(programID, 0,"Position"); //Binds the program to an attribute in a shader
	glBindAttribLocation(programID, 1, "Color");
	glLinkProgram(programID); //Links the shaders to the program

	glGetProgramiv(programID, GL_LINK_STATUS, &success); //Sets the value of the success variable declared earlier

	//If success == false the console will display an error and message explaining what the error is
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

	glDeleteShader(fragmentShader); //Deletes the shaders
	glDeleteShader(vertexShader);
}

//Creates the context window we are drawing to.
int StartUp()
{
	if (glfwInit() == false) //Checks if glfw can start
		return -1;

	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,"OpenGL", nullptr, nullptr); //Sets the property of the window we are trying correct

	if (window == nullptr) //If no window is created the program is closed
		glfwTerminate();

	glfwMakeContextCurrent(window); //Makes our window the current window being viewed

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) //Checks to see if all functions have been loaded for the program to run if not the program is closed
	{
		glfwDestroyWindow(window); //Closes the context window
		glfwTerminate(); //Closed the application
		return -3;
	}

	auto major = ogl_GetMajorVersion(); //Gets the version of OpenGL we are running
	auto minor = ogl_GetMinorVersion();
	printf_s("GL: %i.%i\n", major, minor);

	CreateShaders();

	return 1;
}

//Keeps the application in a constant game loop
int Update()
{
	//While the contect window is open the program will continue to loop
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

	//Starts the application
	if (StartUp() == true)
	{
		//Starts the game loop
		while (Update() == true)
		{
			GenGrid(2, 2); //Draws objects to the screen
			glfwSwapBuffers(window); //Keeps the window open
			glfwPollEvents(); //Polls all events that happen while the window is active
			
		}

		glfwDestroyWindow(window); //When window is closed destroys the window
		glfwTerminate(); //Closed the applicatoin
	}

	return 0;
}