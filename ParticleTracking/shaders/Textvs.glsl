#version 330 core

layout(location = 0) in vec2 vertexPosition_screenspace;
layout(location = 1) in vec2 vertexUV;

uniform vec2 windowDimensions;

out vec2 UV;

void main()
{
	float w = (windowDimensions.x * 0.5);
	float h = (windowDimensions.y * 0.5);

	vec2 vertexPosition_homoneneousspace = vertexPosition_screenspace - vec2(w, h);
	vertexPosition_homoneneousspace /= vec2(w, h);
	gl_Position =  vec4(vertexPosition_homoneneousspace, 0.0, 1.0);
	UV = vertexUV;
}

