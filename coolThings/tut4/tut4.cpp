//Include standard libraries 
#include <stdio.h>
#include <stdlib.h>
#include <ctime>

//Include GLEW
#include <GL/glew.h>

//INclude GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

//Include GLM
#include <glm/glm.hpp>
using namespace glm;
#include <glm/gtx/transform.hpp> // after <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Include important shader
#include <common/shader.hpp>



int main()
{
	//Initialize GLFW
	glewExperimental = true; //Needed for core profile
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //3.3 version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Will not open older versions

	//Open a window and create its open gl context
	GLFWwindow* window;
	window = glfwCreateWindow(800, 600, "Tutorial 4", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); //Initialize GLEW
	glewExperimental = true; //Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//Allows keyboard
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Vertex Array Object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("vertexShader.txt", "fragmentShader.txt");


	// Our vertices. Three consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat cube_vertex_buffer[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};

	// One color for each vertex. They were generated randomly.
	static GLfloat cube_color_buffer[12*3*3];

	//Population of the color buffer array
	int i = 0;
	srand(time(0));
	for (i; i < 12 * 3; i++) {
		cube_color_buffer[3 * i + 0] = (static_cast <float>(rand()) / static_cast <float>(32767));
		cube_color_buffer[3 * i + 1] = (static_cast <float>(rand()) / static_cast <float>(32767));
		cube_color_buffer[3 * i + 2] = (static_cast <float>(rand()) / static_cast <float>(32767));

	}
	
	// vertex buffer for cube
	GLuint cube_vertexbuffer;
	//Genereates 1 buffer and makes it point to it with vertexbuffer
	glGenBuffers(1, &cube_vertexbuffer);
	//Commands to talk about the buffer
	glBindBuffer(GL_ARRAY_BUFFER, cube_vertexbuffer);
	//Send vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertex_buffer), cube_vertex_buffer, GL_STATIC_DRAW);

	//Projection Matrix
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

	//Camera (view) matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(7, 6, 6), // Position of the camera
		glm::vec3(0, 0, 2), // Target to look at
		glm::vec3(0, 1, 0)  //What direction is up?
	);

	//Model matrix : an identiy matrix (model is at the origin)
	glm::mat4 CubeModel = glm::mat4(1.0f);

	//Rotating
	glm::vec3 rotationalAxis(0, 1, 0);
	glm::mat4 rotateMat = glm::rotate(glm::radians(90.0f), rotationalAxis);

	//Remeber its transformed right to left
	//Model = transMat * rotateMat * scaleMat * Model;
	CubeModel = rotateMat * CubeModel;

	//MVP or model view persepective matrix
	glm::mat4 cube_mvp = Projection * View * CubeModel;

	//Color buffer bound and filled for cube
	GLuint cubecolorbuffer;
	glGenBuffers(1, &cubecolorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, cubecolorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_color_buffer), cube_color_buffer, GL_STATIC_DRAW);

	//This is for the triangle
	static const GLfloat triangle_vertex_buffer[] = {
		-3.0f, -3.0f, 0.0f,
		3.0f, -3.0f, 0.0f,
		0.0f,  3.0f, 0.0f,
	};

	// One color for each vertex for the triangle
	static GLfloat triangle_color_buffer[3*3];

	//Population of the traingle color buffer array
	int j = 0;
	srand(time(0));
	for (j; j < 3 * 3; j++) {
		triangle_color_buffer[3 * j + 0] = (static_cast <float>(rand()) / static_cast <float>(32767));
		triangle_color_buffer[3 * j + 1] = (static_cast <float>(rand()) / static_cast <float>(32767));
		triangle_color_buffer[3 * j + 2] = (static_cast <float>(rand()) / static_cast <float>(32767));

	}

	// vertex buffer
	GLuint triangle_vertexbuffer;
	//Genereates 1 buffer and makes it point to it with vertexbuffer
	glGenBuffers(1, &triangle_vertexbuffer);
	//Commands to talk about the buffer
	glBindBuffer(GL_ARRAY_BUFFER, triangle_vertexbuffer);
	//Send vertices to OpenGL
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertex_buffer), triangle_vertex_buffer, GL_STATIC_DRAW);

	//Model matrix : an shifted matrix for the triangle
	glm::mat4 TriangleModel = glm::mat4(1.0f);

	//Translate the triangle
	glm::mat4 transMat = glm::translate(glm::vec3(0, 0, 3));

	TriangleModel = transMat * rotateMat * TriangleModel;

	//MVP or model view persepective matrix
	glm::mat4 triangle_mvp = Projection * View * TriangleModel;

	//Triangle colorbuffer
	GLuint trianglecolorbuffer;
	glGenBuffers(1, &trianglecolorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, trianglecolorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color_buffer), triangle_color_buffer, GL_STATIC_DRAW);

	//NOW to have glsl do its thing
	//Get a handle for the mvp
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");


	//Z-buffer depth test
	glEnable(GL_DEPTH_TEST);
	//Accept fragments when closer than previous one
	glDepthFunc(GL_LESS);


	//Animation loop
	do {
		//Clears the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Shader first
		glUseProgram(programID);

		//Transformation to the current bound shader for the cube
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &cube_mvp[0][0]);

		//Drawing cube
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, cube_vertexbuffer);
		glVertexAttribPointer(
			0,				//no real reason
			3,				//size
			GL_FLOAT,		//type
			GL_FALSE,		//normalized?
			0,				//stride
			(void*)0		//array buffer offset
		);
		
		//Coloring cube
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, cubecolorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		// Draw the cube!
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // Starting from vertex 0; 3 vertices total -> 1 triangle


		//Transformation to the current bound shader
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &triangle_mvp[0][0]);

		//Drawing triangle
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, triangle_vertexbuffer);
		glVertexAttribPointer(
			0,				//no real reason
			3,				//size
			GL_FLOAT,		//type
			GL_FALSE,		//normalized?
			0,				//stride
			(void*)0		//array buffer offset
		);
		//Coloring triangle
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, trianglecolorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		// Draw the triangle!
		glDrawArrays(GL_TRIANGLES, 0, 3); // single triangle
		glDisableVertexAttribArray(0);

		//Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &cube_vertexbuffer);
	glDeleteBuffers(1, &triangle_vertexbuffer);
	glDeleteBuffers(1, &cubecolorbuffer);
	glDeleteBuffers(1, &trianglecolorbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

