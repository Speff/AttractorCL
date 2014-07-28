#ifndef PSETTINGS
	#include "ProgramSettings.h"
#else
#endif

#include "aGL.h"
#include "aCL.h"

void error_callback(int, const char*);
static void key_callback(GLFWwindow*, int, int, int, int);
void resize(GLFWwindow*, int, int);
glm::mat4 calcViewTransforms();

