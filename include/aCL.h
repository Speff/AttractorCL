#ifndef PSETTINGS
	#include "ProgramSettings.h"
#else
#endif


void initCL();
void killCL();
void runSim();
void boilerplateCode();
void compileKernel();
void setMemMappings();
void checkErrorCode(char const*, int);
void readBuffer();
extern "C" void CL_CALLBACK pfn_notify(const char *, const void *, size_t, void *);

#ifndef cl_khr_gl_sharing
#define cl_khr_gl_sharing     1
#endif
