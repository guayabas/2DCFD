#pragma once
#include "Definitions.h"
#include "Fluid.h"
#include <vector>

#define TIME_INTEGRATION_INCREMENT_PARTICLE real(0.01)
#define NUM_TRAILING_PARTICLES 50

namespace FluidSimulation
{
	class Particle
	{
	public:
		Particle()
		{
			reset();
		}

		~Particle()
		{

		}
		
		vec2 fetchVelocityFluid(Fluid & fluid, uint cellParticleIDi, uint cellParticleIDj)
		{
			real velocityFluidU = fluid.getVelocityU(cellParticleIDi, cellParticleIDj);
			real velocityFluidV = fluid.getVelocityV(cellParticleIDi, cellParticleIDj);
			return vec2(velocityFluidU, velocityFluidV);
		}
		
		void boundaryConditions(vec2 & position, real spacing, const boundaryType boundary)
		{
			if (position.x < spacing)
			{
				position.x = (boundary == PERIODIC) ? real(1.0) - spacing : real(0.0);
			}

			if (position.y < spacing)
			{
				position.y = (boundary == PERIODIC) ? real(1.0) - spacing : real(0.0);
			}

			if (position.x > real(1.0) - spacing)
			{
				position.x = (boundary == PERIODIC) ? spacing : real(1.0);
			}

			if (position.y > real(1.0) - spacing)
			{
				position.y = (boundary == PERIODIC) ? spacing : real(1.0);
			}
		}
		
		void animate(real dt, Fluid & fluid, const boundaryType boundary)
		{
			uint numCells = fluid.getNumCells();
			real spacing = real(1.0) / numCells;
			
			/// Fetch velocity
			uint cellParticleIDi = (uint)(m_position.x * numCells);
			uint cellParticleIDj = (uint)(m_position.y * numCells);
			vec2 velocityFluid = fetchVelocityFluid(fluid, cellParticleIDi, cellParticleIDj);

			/// Euler integration
			m_position += (m_weight * dt * (velocityFluid));

			/// Apply boundary conditions
			boundaryConditions(m_position, spacing, boundary);
			
			/// Do trailing
			m_trailing[0] = m_position;
			for (uint n = 0; n < (NUM_TRAILING_PARTICLES - 1); n++)
			{
				uint cellParticleIDi = (uint)(m_trailing[n].x * numCells);
				uint cellParticleIDj = (uint)(m_trailing[n].y * numCells);
				vec2 velocityFluid = fetchVelocityFluid(fluid, cellParticleIDi, cellParticleIDj);

				m_trailing[n + 1] = m_trailing[n] - dt * (velocityFluid);

				boundaryConditions(m_trailing[n + 1], spacing, boundary);
			}
		}

		void reset()
		{
			m_active = false;
			m_trailing.resize(NUM_TRAILING_PARTICLES);
			m_position = vec2(0.0);

			real sampleRandomNumber = (std::rand() / real(RAND_MAX));
			m_weight = sampleRandomNumber;
		}

		vec2 getPosition() const
		{
			return m_position;
		}

		void setPosition(vec2 position)
		{
			m_position = position;
		}

		real getWeight() const
		{
			return m_weight;
		}

		std::vector<vec2> getTrailing() const
		{
			return m_trailing;
		}

	private:
		std::vector<vec2> m_trailing;
		vec2 m_position;
		bool m_active;
		real m_weight;
	};

	class ParticleSystem
		: public SceneObject, public TimeIntegrator, public BoundaryConditions
	{
	public:
		ParticleSystem()
		{
			setTimeIncrement(TIME_INTEGRATION_INCREMENT_PARTICLE);
			setTimeStep(TIME_INTEGRATION_INCREMENT_PARTICLE);
		}

		~ParticleSystem()
		{
			clear();
		}

		void update(Fluid * fluid)
		{
			for (auto & particle : m_particles)
			{
				particle.animate(m_timeStep, *fluid, m_boundary);
			}
		}

		void clear()
		{
			m_particles.clear();
		}

		void addParticle(const Particle & particle)
		{
			m_particles.push_back(particle);
		}

		const std::vector<Particle> getParticles() const
		{
			return m_particles;
		}

		uint getNumberParticles() const
		{
			return (uint)m_particles.size();
		}

		uint getNumberTrailingParticles() const
		{
			return NUM_TRAILING_PARTICLES;
		}

	private:
		std::vector<Particle> m_particles;
	};
}