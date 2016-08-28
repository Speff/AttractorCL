#ifndef PSETTINGS
	#include "ProgramSettings.h"
#else
#endif

//#include <GL/glcorearb.h>
//#include <GL/glext.h>


void initGL();
void killGL();
bool compileShader(GLuint*, GLchar*, GLenum, GLchar*);
void makeProgram();
void createVBOs();