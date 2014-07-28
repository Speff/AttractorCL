#ifdef cl_khr_fp64
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#elif defined(cl_amd_fp64)
#pragma OPENCL EXTENSION cl_amd_fp64 : enable
#else
#endif

#ifdef cl_khr_gl_sharing
#pragma OPENCL EXTENSION cl_khr_gl_sharing : enable
#elif defined(cl_amd_gl_sharing)
#pragma OPENCL EXTENSION cl_amd_gl_sharing : enable
#else
#endif

typedef struct ParticleContainer{
	uint alive;
	float3 position;
	float3 velocity;
	float velocityDamping;
	float3 color;
	float lifetime;
	float decayPerIteration;
} Particle;

__kernel void updateParticle(__global Particle* particleArray, 
							 const float AR){

	int gid = get_global_id(0); 
	particleArray[gid].position.x += 1.0f;
}