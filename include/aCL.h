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
