//#include "stdio.h"
//#include <gl_core_4_4.h> 
//#include <GLFW/glfw3.h> 
//#include <Gizmos.h>
//#include <glm\ext.hpp>
//#include "MyApplication.h"
//#include <fstream>
//#include "tiny_obj_loader.h"
//#include "Nodes.h"
//
//
//
////Link to OpenGL 4.5 refrence https://www.opengl.org/sdk/docs/man4/
//
//struct OpenGLInfo
//{
//	// vertex and index buffers
//	unsigned int m_VAO; //Vertex Alligned Object
//	unsigned int m_VBO; //Vertex Buffer Object
//	unsigned int m_IBO; //Index Buffer Object
//	unsigned int m_index_count; //Number of indexes
//};
//
//
////Defines what makes up a Vertex obj
//struct Vertex
//{
//	vec4 position;
//	vec4  color;
//};	
//
//#define BUFFER_OFFSET(i) ((char *)NULL + (i))
//
//std::vector<OpenGLInfo> m_gl_info;
//
////Screen Resoulution
//float WINDOW_WIDTH = 1600;
//float WINDOW_HEIGHT = 900;
//
//unsigned int programID; //Value that represent our application
//
//GLFWwindow *window; //Represents our window
//
//mat4 m_view = glm::lookAt(vec3(0, 0, 50), vec3(0), vec3(0, 1, 0));
//mat4 m_projection = glm::perspective(glm::pi<float>()*0.25f, 16.f / 9.f, 0.1f, 100.f);
//mat4 m_projectionViewMatrix = m_projection * m_view;
//
//void CreateObjBuffers(std::vector<tinyobj::shape_t> &shapes)
//{
//	m_gl_info.resize(shapes.size()); //sets the size of m_gl_info to the size of shapes
//	for (unsigned int mesh_index = 0; mesh_index < shapes.size(); mesh_index++)
//	{
//		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO); //Gens the vertex array at the current index
//		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO); //Gen the vertex buffer
//		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO); //Gen the index buffer
//		glBindVertexArray(m_gl_info[mesh_index].m_VAO); //binds the vertex array
//
//		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
//		float_count += shapes[mesh_index].mesh.normals.size();
//		float_count += shapes[mesh_index].mesh.texcoords.size();
//
//		std::vector<float> vertex_data;
//		vertex_data.reserve(float_count);
//
//		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.positions.begin(), shapes[mesh_index].mesh.positions.end());
//
//		vertex_data.insert(vertex_data.end(), shapes[mesh_index].mesh.normals.begin(), shapes[mesh_index].mesh.normals.end());
//
//		m_gl_info[mesh_index].m_index_count = shapes[mesh_index].mesh.indices.size();
//
//		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
//		glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int), shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);
//
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));
//
//		glBindVertexArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//		glUseProgram(programID); //Tells the system what shader or program we are using to render the objects
//		unsigned int projectionViewUniform = glGetUniformLocation(programID, "ProjectionView"); //Sets up the view for the window
//		glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionViewMatrix)); //Creates the view for the scene
//
//		for (unsigned int i = 0; i < m_gl_info.size(); i++)
//		{
//			glBindVertexArray(m_gl_info[i].m_VAO);
//			glDrawElements(GL_TRIANGLES, m_gl_info[i].m_index_count, GL_UNSIGNED_INT, 0);
//		}	
//	}
//}
//
//void CreateBuffers(Plane *p)
//{
//	OpenGLInfo gl_Info;
//
//	glGenVertexArrays(1, &gl_Info.m_VAO); //vertex array generation
//	glGenBuffers(1, &gl_Info.m_VBO); //Vertex Buffer gen
//	glGenBuffers(1, &gl_Info.m_IBO); //Index buffer gen
//	glBindVertexArray(gl_Info.m_VAO); //bing vertex array
//
//	glBindBuffer(GL_ARRAY_BUFFER, gl_Info.m_VBO);
//	glBufferData(GL_ARRAY_BUFFER, (p->row * p->cols) * sizeof(Vertex), &p->vertices, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ARRAY_BUFFER, gl_Info.m_IBO);
//	glBufferData(GL_ARRAY_BUFFER, (p->row-1) * (p->cols - 1) * 6 * sizeof(unsigned int), &p->indicies, GL_STATIC_DRAW);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_Info.m_IBO);
//
//	glEnableVertexAttribArray(0);
//	glEnableVertexAttribArray(1);
//
//	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
//	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(vec4)));
//
//	glBindVertexArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//
//	glUseProgram(programID);
//	unsigned int projectionViewUniform = glGetUniformLocation(programID, "ProjectionView");
//	glUniformMatrix4fv(projectionViewUniform, 1, false, glm::value_ptr(m_projectionViewMatrix));
//	glBindVertexArray(gl_Info.m_VAO);
//	glDrawElements(GL_TRIANGLES, (p->row - 1) * (p->cols - 1) * 6, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
//}
//
////Creates and links all the shaders we will be using to draw to the screen when called
////Called in StartUp()
//void CreateShaders()
//{
//	std::string vSource; //Source for the shader
//	std::ifstream vshaderStream("VertexShader.vert"); //Gets the file we are trying to read from
//	if (vshaderStream.is_open())
//	{
//		std::string vbuff; //Creates a buffer to represent the current line we are reading
//		while (std::getline(vshaderStream, vbuff))
//		{
//			//While we are reading a line in we add it to the source and add a terminating character to end the line
//			vSource = vSource + vbuff + "\n";
//		}
//		vshaderStream.close();
//	}
//
//	std::string fSource; //Source for the shader
//	std::ifstream fshaderStream("FragmentShader.frag"); //Gets the file we are reading from
//	if (fshaderStream.is_open())
//	{
//		std::string fbuff; //Creates a buffer to represent the current line we are reading
//		while (std::getline(fshaderStream, fbuff))
//		{
//			//While we are reading a line in we add it to the source and add a terminating character to end the line
//			fSource = fSource + fbuff + "\n"; 
//		}
//		fshaderStream.close();
//	}
//
//	const char* vsSource = vSource.c_str(); //Converts the source from a string to a char
//	const char* fsSource = fSource.c_str(); //Converts the source from a string to a char
//
//
//	//Compiles the shaders
//	int success = GL_FALSE; //Check to see if the shader has been linked correctly
//	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); //Creates the Vertex Shader
//	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //Creates the Fragment Shader
//
//	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0); //Replaces the soource code int the shader obj
//	glCompileShader(vertexShader); //Compilies the shader obj
//	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
//	glCompileShader(fragmentShader);
//
//	programID = glCreateProgram(); //Creates the shader/program that is allowing us to draw to the screen
//	glAttachShader(programID, vertexShader); //Attachs the shader to the program
//	glAttachShader(programID, fragmentShader);
//	glBindAttribLocation(programID, 0,"Position"); //Binds the program to an attribute in a shader
//	glBindAttribLocation(programID, 1, "Color");
//	glLinkProgram(programID); //Links the shaders to the program
//
//	glGetProgramiv(programID, GL_LINK_STATUS, &success); //Sets the value of the success variable declared earlier
//
//	//If success == false the console will display an error and message explaining what the error is
//	if (success == GL_FALSE)
//	{
//		int infoLogLength = 0;
//		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
//		char* infoLog = new char[infoLogLength];
//
//		glGetProgramInfoLog(programID, infoLogLength, 0, infoLog);
//
//		printf("Error: failed to link shader program! \n");
//		printf("%s\n", infoLog);
//		delete[] infoLog;
//	}
//
//	glDeleteShader(fragmentShader); //Deletes the shaders
//	glDeleteShader(vertexShader);
//}
//
////Creates the context window we are drawing to.
//int StartUp()
//{
//	if (glfwInit() == false) //Checks if glfw can start
//		return -1;
//
//	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,"OpenGL", nullptr, nullptr); //Sets the property of the window we are trying correct
//
//	if (window == nullptr) //If no window is created the program is closed
//		glfwTerminate();
//
//	glfwMakeContextCurrent(window); //Makes our window the current window being viewed
//
//	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) //Checks to see if all functions have been loaded for the program to run if not the program is closed
//	{
//		glfwDestroyWindow(window); //Closes the context window
//		glfwTerminate(); //Closed the application
//		return -3;
//	}
//
//	auto major = ogl_GetMajorVersion(); //Gets the version of OpenGL we are running
//	auto minor = ogl_GetMinorVersion();
//	printf_s("GL: %i.%i\n", major, minor);
//
//	CreateShaders();
//
//	return 1;
//}
//
////Keeps the application in a constant game loop
//int Update()
//{
//	//While the contect window is open the program will continue to loop
//	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
//	{
//		glClearColor(0.25f, 0.25f, 0.25f, 1);
//		glEnable(GL_DEPTH_TEST);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		return 1;
//	}
//
//	return 0;
//}
//
//int main()
//{
//	//std::vector<tinyobj::shape_t> bunny;
//	//std::vector<tinyobj::material_t> bMaterials;
//	//std::string berr;
//	//tinyobj::LoadObj(bunny, bMaterials, berr, "bunny.obj");
//	//std::vector<tinyobj::shape_t> tri;
//	//std::vector<tinyobj::material_t> tMaterials;
//	//std::string terr;
//	//tinyobj::LoadObj(tri, tMaterials, terr, "triangle.obj");
//	//std::vector<tinyobj::shape_t> sqr;
//	//std::vector<tinyobj::material_t> sMaterials;
//	//std::string serr;
//	//tinyobj::LoadObj(sqr, sMaterials, serr, "square.obj");
//
//	Plane *p = new Plane;
//	p->GenIndicies();
//	//Starts the application
//	if (StartUp() == true)
//	{	
//
//		CreateBuffers(p);
//
//		//Starts the game loop
//		while (Update() == true)
//		{
//			//CreateObjBuffers(bunny);
//			//CreateObjBuffers(tri);
//			//CreateObjBuffers(sqr);
//
//			glfwSwapBuffers(window); //Keeps the window open
//			glfwPollEvents(); //Polls all events that happen while the window is active		
//		}
//		glfwDestroyWindow(window); //When window is closed destroys the window
//		glfwTerminate(); //Closed the applicatoin
//	}
//
//	return 0;
//}