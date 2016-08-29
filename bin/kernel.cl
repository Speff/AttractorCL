//#ifdef cl_khr_fp64
//#pragma OPENCL EXTENSION cl_khr_fp64 : enable
//#elif defined(cl_amd_fp64)
//#pragma OPENCL EXTENSION cl_amd_fp64 : enable
//#else
//#endif

//#ifdef cl_khr_gl_sharing
//#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
//#elif defined(cl_amd_gl_sharing)
//#pragma OPENCL EXTENSION cl_amd_gl_sharing : enable
//#else
//#endif

typedef struct{
	uint alive;
	float velocityDamping;
	float lifetime;
	float decayPerIteration;
	float4 position;
	float4 velocity;
	float4 color;
} Particle;  

__kernel void updateParticle(__global Particle* particleArray, 
				const float AR,
				const float4 WINDOW_LIMIT_LOWER,
				const float4 WINDOW_LIMIT_UPPER,
				__global float4* Test){
	
	const float2 AR_MATRIX = (float2)(AR, 1.0f);	
	int gid = get_global_id(0);

	Test[gid] = particleArray[gid].position;

	particleArray[gid].position.x *= AR_MATRIX.x;
	particleArray[gid].position.y *= AR_MATRIX.y;


        //particleArray[gid].position.x = sin(particleArray[gid].position.y) + cos(particleArray[gid].position.x);
        //particleArray[gid].position.y = sin(particleArray[gid].position.x) + cos(particleArray[gid].position.y);
	//particleArray[gid].position.z -= .01;

	particleArray[gid].position.x /= AR_MATRIX.x;
	particleArray[gid].position.y /= AR_MATRIX.y;
}
