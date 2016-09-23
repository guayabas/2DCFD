#pragma once
#include "Definitions.h"

namespace FluidSimulation
{
	static real TaylorGreenVortexDensity(real t, real x, real y)
	{
		real factorA = -real(0.25) * std::exp(-real(4.0) * t);
		real factorB = std::cos(real(2.0) * x * ONE_PI) + std::cos(real(2.0) * y * ONE_PI);
		return (factorA * factorB);
	}

	static real TaylorGreenVortexVelocityU(real t, real x, real y)
	{
		real factorA = -std::exp(-real(2.0) * t);
		real factorB = std::cos(TWO_PI * x) * std::sin(TWO_PI * y);
		return (factorA * factorB);
	}

	static real TaylorGreenVortexVelocityV(real t, real x, real y)
	{
		real factorA = +std::exp(-real(2.0) * t);
		real factorB = std::sin(TWO_PI * x) * std::cos(TWO_PI * y);
		return (factorA * factorB);
	}
}