#include "Window.h"

// GLFW Global Variables
int windowSize[2] = {WIDTH, HEIGHT};

// OpenGL Global Variables
extern GLuint programGL;
extern GLuint bufParticle_GL;

// Main: Program Entry Point
// Responsible for initializing GLFW, OpenGL, OpenCL and running the main loop
int main(int argc, char **argv){
	GLFWwindow* window;
	//double time = glfwGetTime();
	double loopStart;

	// GLFW Initialization ----------------------------------------------------------------------
	// Set GLFW Error Callback Function
	glfwSetErrorCallback(error_callback);

	// Initialize GLFW
	if (!glfwInit()) exit(EXIT_FAILURE);

	// Set OpenGL Version (Use 3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Create Window
	window = glfwCreateWindow(windowSize[0], windowSize[1], "AttractorCL", NULL, NULL); 
	// Check if window creation failed
	if (!window){
		glfwTerminate();
		exit(EXIT_FAILURE);
	}	
	// Set current OpenGL Context
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	printf("wglGetCurrentDC(): %u\nGLFW DC: %u\n", (unsigned int)wglGetCurrentDC(), (unsigned int)GetDC(GetForegroundWindow()));

	// Load OpenGL3.3 Extensions (MUST BE AFTER CONTEXT CREATION)
	if(ogl_LoadFunctions() == ogl_LOAD_FAILED){
		glfwDestroyWindow(window);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Set GLFW Functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, resize);
	// ------------------------------------------------------------------------------------------

	// Initialize OpenGL
	initGL();
	// Initialize OpenCL
	initCL();

	glUseProgram(programGL);

	glm::mat4 mtx = calcViewTransforms();
	GLint uniMtx = glGetUniformLocation(programGL, "mtx");
	glUniformMatrix4fv(uniMtx, 1, GL_FALSE, glm::value_ptr(mtx));

	glUseProgram(0);

    runSim();
    readBuffer();
    runSim();
    readBuffer();

	// Run main loop, checking the glfwWindowShouldClose flag for termination
	while (!glfwWindowShouldClose(window)){
		loopStart = glfwGetTime();
		glfwPollEvents();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		runSim();

		glUseProgram(programGL);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, bufParticle_GL);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(cl_uint)+3*sizeof(cl_float)));
		glBindBuffer(GL_ARRAY_BUFFER, bufParticle_GL);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(sizeof(cl_uint) + 2*sizeof(cl_float4) + 3*sizeof(cl_float)));

		glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glUseProgram(0);
		glFinish();
		glfwSwapBuffers(window);
		//printf("%f\n",1/(glfwGetTime()-loopStart));
	}

	// Free Resources and Exit Program -----
	killCL();
	killGL();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

void error_callback(int error, const char* description){
	printf("ERROR: %s\n",description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void resize(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height);

	glUseProgram(programGL);

	glm::mat4 mtx = calcViewTransforms();
	GLint uniMtx = glGetUniformLocation(programGL, "mtx");
	glUniformMatrix4fv(uniMtx, 1, GL_FALSE, glm::value_ptr(mtx));

	glUseProgram(0);
}

char* readFile(const char* fileName){
	FILE *fp;
	//char *fileLocation;
	size_t sourceSize;
	char* output;

	//fileLocation = (char*)malloc(strlen(fileName));
	//strcpy(fileLocation, PATH);
	//strcat(fileLocation, fileName);

	fp = fopen(fileName, "r");
	if(!fp){
		printf("Can't read %s\n", fileName);
	}
	fseek(fp, 0, SEEK_END);
	sourceSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	output = (char*)calloc(1, sourceSize + 1);
	fread(output, 1, sourceSize, fp);
	fclose(fp);
	//free(fileLocation);
	if(VERBOSE) printf("%s read\n", fileName);
	return output;
}

glm::mat4 calcViewTransforms(){
	glm::mat4 view = glm::lookAt(
		glm::vec3(2, 0, 2),			// Observer Location
		glm::vec3(0.0f, 0.0f, 0.0f),// View Target
		glm::vec3(0.0f, 0, 1));		// Up Axis

	glm::mat4 proj = glm::perspective(
		45.0f,									// Vert FOV
		windowSize[1] / (float) windowSize[2],	// Aspect Ratio
		1.0f,									// Near Z
		10.0f);									// Far Z

	return proj * view;
}
