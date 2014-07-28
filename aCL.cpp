#include "aCL.h"

// OpenCL Vars
cl_uint numDevices;
cl_device_id *devices;
cl_program programCL;
cl_context context;
cl_command_queue cmdQueue;
cl_kernel kernel;
cl_mem bufParticle_CL;

extern GLuint bufParticle_GL;

void initCL(){
	boilerplateCode();
	compileKernel();
	setMemMappings();
}

void killCL(){
	// Free OpenCL resources
	clReleaseKernel(kernel);
	clReleaseProgram(programCL);
	clReleaseCommandQueue(cmdQueue);
	clReleaseContext(context);
	free(devices);
}

void runSim(){
	// Define an index space (global work size) of work items for execution
	// A workgroup size (local work size) is not required, but can be used.
	size_t globalWorkSize[1];
	cl_int status;

	globalWorkSize[0] = NUM_PARTICLES;

	status = clEnqueueAcquireGLObjects(cmdQueue, 1, &bufParticle_CL, NULL, NULL, NULL);
	clFinish(cmdQueue);

	// Execute the kernel
	status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, globalWorkSize, NULL, 0, NULL, NULL);
	//checkErrorCode("Running Sim...\t\t", status);

	status = clEnqueueReleaseGLObjects(cmdQueue, 1, &bufParticle_CL, NULL, NULL, NULL);
}

void setMemMappings(){
	cl_int status;	// Used to check for errors
	cl_float AR = WIDTH / (float) HEIGHT;

	// Create buffer objects --------------------------------------
	// Current Position Buffer (posC) is created from a GL VBO since this is the buffer to draw
	bufParticle_CL = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, bufParticle_GL, &status);
	checkErrorCode("Creating bufParticle...\t", status);

	// Associate the input and output buffers & variables with the kernel
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufParticle_CL); checkErrorCode("Setting KernelArg(0)...\t", status);
	status = clSetKernelArg(kernel, 1, sizeof(cl_float), &AR); checkErrorCode("Setting KernelArg(1)...\t", status);

	// Wait until command queue events are completed
	clFinish(cmdQueue);
}

void compileKernel(){
	char* kernelSource;
	char kernelFileLocation[] = "kernel.cl";
	cl_int status;

	// Read kernel source code into _kernelSource_ ------------------------
	kernelSource = readFile(kernelFileLocation);

	// Create a program using the source code
	programCL = clCreateProgramWithSource(context, 1, (const char**)&kernelSource, NULL, &status);
	checkErrorCode("Creating program...\t", status);

	// Compile the program
	status = clBuildProgram(programCL, numDevices, devices, NULL, NULL, NULL);
	checkErrorCode("Compiling program...\t", status);

	char* buildLog;
	size_t buildLogSize;
	clGetProgramBuildInfo(programCL,*devices,CL_PROGRAM_BUILD_LOG, NULL, NULL, &buildLogSize);
	buildLog = (char*)malloc(buildLogSize);
	clGetProgramBuildInfo(programCL,*devices,CL_PROGRAM_BUILD_LOG, buildLogSize, buildLog, NULL);
	if(buildLogSize > 2) printf("%s\n",buildLog);
	free(buildLog);

	// Create the vector addition kernel
	kernel = clCreateKernel(programCL, "updateParticle", &status);
	checkErrorCode("Creating kernel...\t", status);
}

void boilerplateCode(){
	// Use this to check the output of each API call
	cl_int status;

	// Retrieve the number of platforms
	cl_uint numPlatforms = 0;
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	checkErrorCode("Getting platforms...\t", status);

	// Allocate enough space for each platform
	cl_platform_id *platforms = NULL;
	platforms = (cl_platform_id*)malloc(numPlatforms * sizeof(cl_platform_id));

	// Fill in the platforms
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);
	checkErrorCode("Filling platforms...\t", status);

	// Retrieve the number of devices
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);

	// Allocate space for each device
	devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));

	// Fill in the devices
	status = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
	checkErrorCode("Filling devices...\t", status);

	cl_context_properties properties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), // WGL Context  
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), // WGL HDC
		CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[0], // OpenCL platform
		0
	};	

	// Create a contect and associate it with the devices
	context = clCreateContext(properties, numDevices, devices, NULL, NULL, &status);
	checkErrorCode("Creating context...\t", status);

	// Create a command queue and associate it with the device
	cmdQueue = clCreateCommandQueue(context, devices[0], 0, &status);
	checkErrorCode("Creating cmd queue...\t", status);

	char* devName;
	size_t nameSize;
	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, NULL, NULL, &nameSize);
	devName = (char*)malloc(nameSize);
	clGetDeviceInfo(devices[0], CL_DEVICE_NAME, nameSize, devName, NULL);
	if(status == CL_SUCCESS && VERBOSE) printf("Using device:\t\t%s\n", devName); 

	free(platforms);
	free(devName);
}

void checkErrorCode(char* action, int errorCode){
	if(!VERBOSE) return;
	else{
		printf("%s\t",action);
		if(errorCode == CL_SUCCESS) printf("CL_SUCCESS\n");
		if(errorCode == CL_DEVICE_NOT_FOUND) printf("CL_DEVICE_NOT_FOUND\n");
		if(errorCode == CL_DEVICE_NOT_AVAILABLE) printf("CL_DEVICE_NOT_AVAILABLE\n");
		if(errorCode == CL_COMPILER_NOT_AVAILABLE) printf("CL_COMPILER_NOT_AVAILABLE\n");
		if(errorCode == CL_MEM_OBJECT_ALLOCATION_FAILURE) printf("CL_MEM_OBJECT_ALLOCATION_FAILURE\n");
		if(errorCode == CL_OUT_OF_RESOURCES) printf("CL_OUT_OF_RESOURCES\n");
		if(errorCode == CL_OUT_OF_HOST_MEMORY) printf("CL_OUT_OF_HOST_MEMORY\n");
		if(errorCode == CL_PROFILING_INFO_NOT_AVAILABLE) printf("CL_PROFILING_INFO_NOT_AVAILABLE\n");
		if(errorCode == CL_MEM_COPY_OVERLAP) printf("CL_MEM_COPY_OVERLAP\n");
		if(errorCode == CL_IMAGE_FORMAT_MISMATCH) printf("CL_IMAGE_FORMAT_MISMATCH\n");
		if(errorCode == CL_IMAGE_FORMAT_NOT_SUPPORTED) printf("CL_IMAGE_FORMAT_NOT_SUPPORTED\n");
		if(errorCode == CL_BUILD_PROGRAM_FAILURE) printf("CL_BUILD_PROGRAM_FAILURE\n");
		if(errorCode == CL_MAP_FAILURE) printf("CL_MAP_FAILURE\n");
		if(errorCode == CL_INVALID_VALUE) printf("CL_INVALID_VALUE\n");
		if(errorCode == CL_INVALID_DEVICE_TYPE) printf("CL_INVALID_DEVICE_TYPE\n");
		if(errorCode == CL_INVALID_PLATFORM) printf("CL_INVALID_PLATFORM\n");
		if(errorCode == CL_INVALID_PROPERTY) printf("CL_INVALID_PROPERTY\n");
		if(errorCode == CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR ) printf("CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR\n");			
		if(errorCode == CL_INVALID_DEVICE) printf("CL_INVALID_DEVICE\n");
		if(errorCode == CL_INVALID_CONTEXT) printf("CL_INVALID_CONTEXT\n");
		if(errorCode == CL_INVALID_QUEUE_PROPERTIES) printf("CL_INVALID_QUEUE_PROPERTIES\n");
		if(errorCode == CL_INVALID_COMMAND_QUEUE) printf("CL_INVALID_COMMAND_QUEUE\n");
		if(errorCode == CL_INVALID_HOST_PTR) printf("CL_INVALID_HOST_PTR\n");
		if(errorCode == CL_INVALID_MEM_OBJECT) printf("CL_INVALID_MEM_OBJECT\n");
		if(errorCode == CL_INVALID_IMAGE_FORMAT_DESCRIPTOR) printf("CL_INVALID_IMAGE_FORMAT_DESCRIPTOR\n");
		if(errorCode == CL_INVALID_IMAGE_SIZE) printf("CL_INVALID_IMAGE_SIZE\n");
		if(errorCode == CL_INVALID_SAMPLER) printf("CL_INVALID_SAMPLER\n");
		if(errorCode == CL_INVALID_BINARY) printf("CL_INVALID_BINARY\n");
		if(errorCode == CL_INVALID_BUILD_OPTIONS) printf("CL_INVALID_BUILD_OPTIONS\n");
		if(errorCode == CL_INVALID_PROGRAM) printf("CL_INVALID_PROGRAM\n");
		if(errorCode == CL_INVALID_PROGRAM_EXECUTABLE) printf("CL_INVALID_PROGRAM_EXECUTABLE\n");
		if(errorCode == CL_INVALID_KERNEL_NAME) printf("CL_INVALID_KERNEL_NAME\n");
		if(errorCode == CL_INVALID_KERNEL_DEFINITION) printf("CL_INVALID_KERNEL_DEFINITION\n");
		if(errorCode == CL_INVALID_KERNEL) printf("CL_INVALID_KERNEL\n");
		if(errorCode == CL_INVALID_ARG_INDEX) printf("CL_INVALID_ARG_INDEX\n");
		if(errorCode == CL_INVALID_ARG_VALUE) printf("CL_INVALID_ARG_VALUE\n");
		if(errorCode == CL_INVALID_ARG_SIZE) printf("CL_INVALID_ARG_SIZE\n");
		if(errorCode == CL_INVALID_KERNEL_ARGS) printf("CL_INVALID_KERNEL_ARGS\n");
		if(errorCode == CL_INVALID_WORK_DIMENSION) printf("CL_INVALID_WORK_DIMENSION\n");
		if(errorCode == CL_INVALID_WORK_GROUP_SIZE) printf("CL_INVALID_WORK_GROUP_SIZE\n");
		if(errorCode == CL_INVALID_WORK_ITEM_SIZE) printf("CL_INVALID_WORK_ITEM_SIZE\n");
		if(errorCode == CL_INVALID_GLOBAL_OFFSET) printf("CL_INVALID_GLOBAL_OFFSET\n");
		if(errorCode == CL_INVALID_EVENT_WAIT_LIST) printf("CL_INVALID_EVENT_WAIT_LIST\n");
		if(errorCode == CL_INVALID_EVENT) printf("CL_INVALID_EVENT\n");
		if(errorCode == CL_INVALID_OPERATION) printf("CL_INVALID_OPERATION\n");
		if(errorCode == CL_INVALID_GL_OBJECT) printf("CL_INVALID_GL_OBJECT\n");
		if(errorCode == CL_INVALID_BUFFER_SIZE) printf("CL_INVALID_BUFFER_SIZE\n");
		if(errorCode == CL_INVALID_MIP_LEVEL) printf("CL_INVALID_MIP_LEVEL\n");
		if(errorCode == CL_INVALID_GLOBAL_WORK_SIZE) printf("CL_INVALID_GLOBAL_WORK_SIZE\n");
	}
}