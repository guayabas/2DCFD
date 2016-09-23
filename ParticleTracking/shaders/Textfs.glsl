#version 330 core

in vec2 UV;
out vec4 color;
uniform sampler2D myTextureSampler;

void main()
{
	vec4 textColor = texture(myTextureSampler, UV);
	color = vec4(vec3(1.0) - textColor.xyz, textColor.w);
}