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
							 __global float4* Test){
	
	int gid = get_global_id(0); 

	//if( == 1){
		//particleArray[gid].position += particleArray[gid].velocity * particleArray[gid].alive;
		//particleArray[gid].velocity = particleArray[gid].velocity * particleArray[gid].velocityDamping * particleArray[gid].alive;
		//particleArray[gid].color = particleArray[gid].color * particleArray[gid].lifetime * particleArray[gid].alive;
		particleArray[gid].lifetime -= particleArray[gid].decayPerIteration * particleArray[gid].alive;
		//if(particleArray[gid].lifetime < 0) particleArray[gid].alive = 0;
	//}
}
