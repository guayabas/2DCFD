#pragma once
#define GLM_SWIZZLE

#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>

namespace FluidSimulation
{
	typedef unsigned char uchar;
	typedef unsigned int uint;
	typedef glm::vec4 vec4;
	typedef glm::vec3 vec3;
	typedef glm::vec2 vec2;
	typedef float real;

	real infinity = std::numeric_limits<real>::infinity();
}