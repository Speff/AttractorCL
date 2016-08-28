#define		PSETTINGS		TRUE
#define		WIDTH			400
#define		HEIGHT			300
#define		PATH			"C:/Users/Sahil/Documents/GitHub/AttractorCL/"
#define		VERBOSE			true
#define		NUM_PARTICLES	64

#include <gl_core_3_3.h>
#define GLFW_INCLUDE_GLU
#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <CL/cl.h>
#include <CL/cl_gl.h> 
#include <CL/cl_gl_ext.h>
#include <CL/cl_ext.h>

#include <stdlib.h>
#include <stdio.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

typedef struct{
	cl_uint alive;
	cl_float velocityDamping;
	cl_float lifetime;
	cl_float decayPerIteration;
	cl_float4 position;
	cl_float4 velocity;
	cl_float4 color;
} Particle;

char* readFile(const char*);
