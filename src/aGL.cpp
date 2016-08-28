#include "aGL.h"

GLuint programGL;
GLuint bufParticle_GL;

void initGL(){
	GLuint vao;
	makeProgram();

	if(programGL == 0) printf("GL program failed to compile\n");

	createVBOs();

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnable(GL_DEPTH_TEST);
}

void createVBOs(){
	Particle* particleArray= (Particle*)malloc(NUM_PARTICLES*sizeof(Particle));

	cl_float4 initialFloat4p = {0.0f, 0.5f, 0.0f, 1.0f};
	cl_float4 initialFloat4v = {0.0f, -0.001f, 0.0f, 0.0f};
	cl_float4 initialFloat4c = {1.0f, 0.0f, 1.0f, 1.0f};
	cl_float particleSpacing = 1/(cl_float)NUM_PARTICLES;

	for(int i = 0; i < NUM_PARTICLES; i++){
		particleArray[i].alive = (cl_uint)1;
		particleArray[i].position = initialFloat4p;
		particleArray[i].position.s[0] = i*particleSpacing-.5;
		particleArray[i].velocity = initialFloat4v;
		particleArray[i].velocityDamping = (cl_float)1.0f;
		particleArray[i].color = initialFloat4c;
		particleArray[i].lifetime = (cl_float)1.0f;
		particleArray[i].decayPerIteration = (cl_float)0.00001f;
	}

	glGenBuffers(1, &bufParticle_GL);
	glBindBuffer(GL_ARRAY_BUFFER, bufParticle_GL);
	glBufferData(GL_ARRAY_BUFFER, NUM_PARTICLES*sizeof(Particle), particleArray, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void makeProgram(){
	GLint compilationResult;
	GLuint vertexShader;
	GLuint fragShader;
	GLchar* vertShaderSource;
	GLchar* fragShaderSource;

	vertShaderSource = readFile("attractor.vert");
	fragShaderSource = readFile("attractor.frag");

	compileShader(&vertexShader, vertShaderSource, GL_VERTEX_SHADER, "Vertex");
	compileShader(&fragShader, fragShaderSource, GL_FRAGMENT_SHADER, "Frag");

	programGL = glCreateProgram();
	glAttachShader(programGL, vertexShader);
	glAttachShader(programGL, fragShader);
	glLinkProgram(programGL);


	glGetProgramiv(programGL, GL_LINK_STATUS, &compilationResult);
	if(!compilationResult){		
		int compilationLogSize;

		glGetProgramiv(programGL, GL_INFO_LOG_LENGTH, &compilationLogSize);
		char* compilationLog;
		compilationLog = (char*)malloc(compilationLogSize);

		glGetProgramInfoLog(programGL, compilationLogSize, NULL, compilationLog);
		printf("%s\n", compilationLog);
		delete[] compilationLog;
	}
	else printf("GL Program compiled\n");

	glDetachShader(programGL, vertexShader);
	glDetachShader(programGL, fragShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
	delete[] vertShaderSource;
	delete[] fragShaderSource;
}

bool compileShader(GLuint* shaderHandle, GLchar* shaderSource, GLenum shaderType, GLchar const* shaderName){
	GLint compilationResult = 0;

	*shaderHandle = glCreateShader(shaderType);
	if(!(*shaderHandle)) printf("Can't make %s shader\n", shaderName);

	glShaderSource(*shaderHandle, 1, &shaderSource, NULL);
	glCompileShader(*shaderHandle);

	glGetShaderiv(*shaderHandle, GL_COMPILE_STATUS, &compilationResult);
	if(!compilationResult){
		printf("%s shader failed to compile\n", shaderName);

		int compilationLogSize;

		glGetShaderiv(*shaderHandle, GL_INFO_LOG_LENGTH, &compilationLogSize);
		char* compilationLog;
		compilationLog = (char*)malloc(compilationLogSize);

		glGetShaderInfoLog(*shaderHandle, compilationLogSize, NULL, compilationLog);
		printf("%s\n", compilationLog);

		delete[] compilationLog;
		return false;
	}

	return true;
}

void killGL(){

}
