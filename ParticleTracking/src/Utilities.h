#pragma once
#include "Definitions.h"
#include <iostream>

namespace FluidSimulation
{
	std::ostream& operator<<(std::ostream & os, const vec2 & v)
	{
		return (os << v.x << ' ' << v.y);
	}

	std::ostream& operator<<(std::ostream & os, const vec3 & v)
	{
		return (os << v.x << ' ' << v.y << ' ' << v.z);
	}

	template <typename T>
	void findMinMaxValue(const T & value, T & max, T & min)
	{
		if (value > max) max = value;
		if (value < min) min = value;
	}
}