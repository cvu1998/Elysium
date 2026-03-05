#shader compute
#version 430 core

layout(std430, binding = 0) buffer PositionXBuffer
{
	float PositionX[];
};
layout(std430, binding = 1) buffer PositionYBuffer
{
	float PositionY[];
};
layout(std430, binding = 2) buffer VelocityXBuffer
{
	float VelocityX[];
};
layout(std430, binding = 3) buffer VelocityYBuffer
{
	float VelocityY[];
};

layout(std430, binding = 4) buffer RotationBuffer
{
	float Rotation[];
};

layout(std430, binding = 5) buffer RotationSpeedBuffer
{
	float RotationSpeed[];
};

layout(std430, binding = 6) buffer SizeBeginBuffer
{
	float SizeBegin[];
};
layout(std430, binding = 7) buffer SizeEndBuffer
{
	float SizeEnd[];
};
layout(std430, binding = 8) buffer SizeBuffer
{
	float Size[];
};

layout(std430, binding = 9) buffer ColorBeginBuffer
{
	vec4 ColorBegin[];
};
layout(std430, binding = 10) buffer ColorEndBuffer
{
	vec4 ColorEnd[];
};
layout(std430, binding = 11) buffer ColorBuffer
{
	vec4 Color[];
};

layout(std430, binding = 12) buffer LifeTimeBuffer
{
	float LifeTime[];
};
layout(std430, binding = 13) buffer LifeRemainingBuffer
{
	float LifeRemaining[];
};

layout(local_size_x = 1024) in;

uniform float u_Timestep;

void main()
{
	uint gid = gl_GlobalInvocationID.x;

	LifeRemaining[gid] -= u_Timestep;
	PositionX[gid] += VelocityX[gid] * u_Timestep;
	PositionY[gid] += VelocityY[gid] * u_Timestep;
	Rotation[gid] += RotationSpeed[gid] * u_Timestep;

	// Fade away particles
	float lifeRatio = LifeRemaining[gid] / LifeTime[gid];

	Color[gid] = mix(ColorEnd[gid], ColorBegin[gid], lifeRatio);
	Color[gid].a = lifeRatio;

	Size[gid] = mix(SizeEnd[gid], SizeBegin[gid], lifeRatio);
}