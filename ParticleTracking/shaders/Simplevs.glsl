#version 330 core

layout(location = 0) in vec4 aPosition;
layout(location = 2) in vec4 aColor;

out vec4 vColor;

void main()
{
	vColor = aColor;
	gl_Position = aPosition;
}
