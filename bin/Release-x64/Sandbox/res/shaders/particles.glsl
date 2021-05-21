#shader compute
#version 430 core

struct Particle
{
	vec4 ColorBegin;
	vec4 ColorEnd;
	vec4 Color;

	vec2 Position;
	vec2 Velocity;

	float Rotation;
	float RotationSpeed;

	float SizeBegin, SizeEnd, Size;

	float LifeTime;
	float LifeRemaining;

	bool Active;
};

layout(std430, binding = 0) buffer ParticleProps
{
	Particle Particles[];
};

layout(local_size_x = 1024) in;

uniform float u_Timestep;

void main()
{
	uint gid = gl_GlobalInvocationID.x;

	Particles[gid].LifeRemaining -= u_Timestep;
	Particles[gid].Position += Particles[gid].Velocity* u_Timestep;
	Particles[gid].Rotation += Particles[gid].RotationSpeed * u_Timestep;

	// Fade away particles
	float lifeRatio = Particles[gid].LifeRemaining / Particles[gid].LifeTime;

	Particles[gid].Color = mix(Particles[gid].ColorEnd, Particles[gid].ColorBegin, lifeRatio);
	Particles[gid].Color.a = lifeRatio;

	Particles[gid].Size = mix(Particles[gid].SizeEnd, Particles[gid].SizeBegin, lifeRatio);
}