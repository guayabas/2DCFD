#pragma once
#include "AnalyticalSolutions.h"
#include "BoundaryConditions.h"
#include "TimeIntegrator.h"

#define TIME_INTEGRATION_INCREMENT_FLUID real(0.1)
#define VISCOSITY_STEP real(0.001)
#define DIFFUSION_STEP real(0.001)
#define NUM_CELLS_MAX 512
#define NUM_CELLS_MIN 4

namespace FluidSimulation
{
	class Fluid
		: public SceneObject
		, public TimeIntegrator
		, public BoundaryConditions
	{
	public:
		Fluid()
		{
			this->setTimeIncrement(TIME_INTEGRATION_INCREMENT_FLUID);
			this->setTimeStep(TIME_INTEGRATION_INCREMENT_FLUID);
		}

		~Fluid()
		{
			deallocateMemory();
		}

		void update()
		{			
			/// Velocity step
			std::swap(m_u0, m_u1);
			std::swap(m_v0, m_v1);
			diffuse(m_u1, m_u0, m_diffusion);
			diffuse(m_v1, m_v0, m_diffusion);

			std::swap(m_u0, m_u1);
			std::swap(m_v0, m_v1);
			advect(m_u1, m_u0, m_u0, m_v0, m_timeStep);
			advect(m_v1, m_v0, m_u0, m_v0, m_timeStep);
			project();

			/// Density step
			std::swap(m_d0, m_d1);
			diffuse(m_d1, m_d0, m_diffusion);

			std::swap(m_d0, m_d1);
			advect(m_d1, m_d0, m_u1, m_v1, m_timeStep);

			/// Find min/max values
			findExtremeValues();			

			/// Add an square obstacle
			addSimpleObstacle();
		}

		real getSpacingCells() const
		{
			return m_spacingCells;
		}

		real getVelocityU(uint i, uint j)
		{
			return m_u1[rowLinearIndexMap(i, j)];
		}

		real getVelocityV(uint i, uint j)
		{
			return m_v1[rowLinearIndexMap(i, j)];
		}

		real getDensity(uint i, uint j)
		{
			return m_d1[rowLinearIndexMap(i, j)];
		}

	public:
		void addDrag(uint i, uint j, vec2 force)
		{
			m_u1[rowLinearIndexMap(i, j)] += force.x;
			m_v1[rowLinearIndexMap(i, j)] += force.y;
		}

		void addSource(uint i, uint j, real intensity)
		{
			m_d1[rowLinearIndexMap(i, j)] += intensity;
		}

		void addSink(uint i, uint j, real intensity)
		{
			m_d1[rowLinearIndexMap(i, j)] -= intensity;
		}

		void setObstacle(bool enabled)
		{
			m_enabledObstacle = enabled;
		}

		bool isObstacleEnabled()
		{
			return m_enabledObstacle;
		}

	public:
		void init()
		{
			m_spacingCells = (real(1.0) / m_numberCells);

			deallocateMemory();
			allocateMemory();
			clearValues();

			initialCondition();
		}

		void increaseGridSize()
		{
			m_numberCells *= 2;
			if (m_numberCells > NUM_CELLS_MAX) { m_numberCells = NUM_CELLS_MAX; return; }
			init();
		}

		void decreaseGridSize()
		{
			m_numberCells /= 2;
			if (m_numberCells < NUM_CELLS_MIN) { m_numberCells = NUM_CELLS_MIN; return; }
			init();
		}

		void increaseDiffusion()
		{
			m_diffusion += DIFFUSION_STEP;
			if (m_diffusion > real(1.0)) { m_diffusion = real(1.0); return; }
		}

		void decreaseDiffusion()
		{
			m_diffusion -= DIFFUSION_STEP;
			if (m_diffusion < real(0.0)) { m_diffusion = real(0.0); return; }
		}

		void increaseViscosity()
		{
			m_viscosity += VISCOSITY_STEP;
			if (m_viscosity > real(1.0)) { m_viscosity = real(1.0); return; }
		}

		void decreaseViscosity()
		{
			m_viscosity -= VISCOSITY_STEP;
			if (m_viscosity < real(0.0)) { m_viscosity = real(0.0); return; }
		}

		real getMaxDensity() const
		{
			return m_maxDensity;
		}

		real getMinDensity() const
		{
			return m_minDensity;
		}

		real getMaxSpeed() const
		{
			return m_maxSpeed;
		}

		real getMinSpeed() const
		{
			return m_minSpeed;
		}

		real getViscosity() const
		{
			return m_viscosity;
		}

		real getDiffusion() const
		{
			return m_diffusion;
		}

		uint getNumCells() const
		{
			return m_numberCells;
		}

	protected:
		void deallocateMemory()
		{
			delete[] m_v1;
			delete[] m_v0;
			delete[] m_u1;
			delete[] m_u0;
			delete[] m_d1;
			delete[] m_d0;
			delete[] m_pressure;
			delete[] m_divergence;
		}

		void allocateMemory()
		{
			/// Count for the ghost cells with the factor of 2
			uint size = (m_numberCells + 2) * (m_numberCells + 2);
			m_divergence = new real[size];
			m_pressure = new real[size];
			m_d0 = new real[size];
			m_d1 = new real[size];
			m_u0 = new real[size];
			m_u1 = new real[size];
			m_v0 = new real[size];
			m_v1 = new real[size];
		}

		void clearValues()
		{
			uint size = (m_numberCells + 2) * (m_numberCells + 2);
			for (uint n = 0; n < size; n++)
			{
				m_divergence[n] = real(0.0);
				m_pressure[n] = real(0.0);
				m_d0[n] = real(0.0);
				m_d1[n] = real(0.0);
				m_u0[n] = real(0.0);
				m_u1[n] = real(0.0);
				m_v0[n] = real(0.0);
				m_v1[n] = real(0.0);
			}
		}

		void initialCondition()
		{
			for (uint i = 1; i <= m_numberCells; i++)
			{
				real x = (i - 0.5f) * m_spacingCells;
				for (uint j = 1; j <= m_numberCells; j++)
				{
					real y = (j - 0.5f) * m_spacingCells;

					real uVelocity = TaylorGreenVortexVelocityU(real(0.0), x, y);
					real vVelocity = TaylorGreenVortexVelocityV(real(0.0), x, y);
					real density = TaylorGreenVortexDensity(real(0.0), x, y);

					m_u1[rowLinearIndexMap(i, j)] = uVelocity;
					m_v1[rowLinearIndexMap(i, j)] = vVelocity;
					m_d1[rowLinearIndexMap(i, j)] = density;

					coreExtremeValues(i, j);
				}
			}
		}

		void findExtremeValues()
		{
			m_maxDensity = -infinity;
			m_minDensity = +infinity;
			m_maxSpeed = -infinity;
			m_minSpeed = +infinity;

			for (uint i = 1; i <= m_numberCells; i++)
			{
				for (uint j = 1; j <= m_numberCells; j++)
				{
					coreExtremeValues(i, j);
				}
			}
		}

		void coreExtremeValues(uint i, uint j)
		{
			real speedCell = glm::length(vec2(m_u1[rowLinearIndexMap(i, j)], m_v1[rowLinearIndexMap(i, j)]));
			findMinMaxValue(speedCell, m_maxSpeed, m_minSpeed);

			real densityCell = m_d1[rowLinearIndexMap(i, j)];
			findMinMaxValue(densityCell, m_maxDensity, m_minDensity);
		}

		void addSimpleObstacle()
		{
			if (m_enabledObstacle && m_numberCells > 8)
			{
				uint centerIndex = m_numberCells / 2;
				int extenstionObstacle = m_numberCells / 8;
				for (int oi = -extenstionObstacle; oi < (extenstionObstacle + 1); oi++)
				{
					for (int oj = -extenstionObstacle; oj < (extenstionObstacle + 1); oj++)
					{
						uint obstaceIndexi = centerIndex + oi;
						uint obstaceIndexj = centerIndex + oj;

						m_u1[rowLinearIndexMap(obstaceIndexi, obstaceIndexj)] = real(0.0);
						m_v1[rowLinearIndexMap(obstaceIndexi, obstaceIndexj)] = real(0.0);
						m_d1[rowLinearIndexMap(obstaceIndexi, obstaceIndexj)] = real(0.0);
					}
				}
			}
		}

		uint rowLinearIndexMap(uint i, uint j)
		{
			return (i + (j * (m_numberCells + 2)));
		}

		uint colLinearIndexMap(uint i, uint j)
		{
			return (j + (i * (m_numberCells + 2)));
		}

	protected:
		void applyBoundaryCell(real & position)
		{
			if (position < real(0.5))
			{
				position = (m_boundary == PERIODIC) ? m_numberCells + real(0.5) : real(0.5);
			}
			if (position > m_numberCells + real(0.5))
			{
				position = (m_boundary == PERIODIC) ? real(0.5) : m_numberCells + real(0.5);
			}
		}

		void periodicBoundaryConditions(real * x)
		{
			/// Walls
			for (uint i = 1; i <= m_numberCells; i++)
			{
				uint leftWallGhost = rowLinearIndexMap(0, i);
				uint rightWallFluid = rowLinearIndexMap(m_numberCells, i);
				x[leftWallGhost] = x[rightWallFluid];

				uint rightWallGhost = rowLinearIndexMap(m_numberCells + 1, i);
				uint leftWallFluid = rowLinearIndexMap(1, i);
				x[rightWallGhost] = x[leftWallFluid];
				
				uint bottomWallGhost = rowLinearIndexMap(i, 0);
				uint topWallFluid = rowLinearIndexMap(i, m_numberCells);
				x[bottomWallGhost] = x[topWallFluid];

				uint topWallGhost = rowLinearIndexMap(i, m_numberCells + 1);
				uint bottomWallFluid = rowLinearIndexMap(i, 1);
				x[topWallGhost] = x[bottomWallFluid];
			}

			/// Corners
			uint leftBottomGhost = rowLinearIndexMap(0, 0);
			uint rightTopFluid = rowLinearIndexMap(m_numberCells, m_numberCells);
			x[leftBottomGhost] = x[rightTopFluid];

			uint rightBottomGhost = rowLinearIndexMap(0, m_numberCells + 1);
			uint leftTopFluid = rowLinearIndexMap(m_numberCells, 1);
			x[rightBottomGhost] = x[leftTopFluid];

			uint leftTopGhost = rowLinearIndexMap(m_numberCells + 1, 0);
			uint rightBottomFluid = rowLinearIndexMap(1, m_numberCells);
			x[leftTopGhost] = x[rightBottomFluid];

			uint rightTopGhost = rowLinearIndexMap(m_numberCells + 1, m_numberCells + 1);
			uint leftBottomFLuid = rowLinearIndexMap(1, 1);
			x[rightTopGhost] = x[leftBottomFLuid];
		}

		void dirichletBoundaryConditions(real * x)
		{
			/// Walls
			for (uint i = 1; i <= m_numberCells; i++)
			{
				uint leftWallGhost = rowLinearIndexMap(0, i);
				uint leftWallFluid = rowLinearIndexMap(1, i);
				x[leftWallGhost] = -x[leftWallFluid];

				uint rightWallGhost = rowLinearIndexMap(m_numberCells + 1, i);
				uint rightWallFluid = rowLinearIndexMap(m_numberCells, i);
				x[rightWallGhost] = -x[rightWallFluid];
				
				uint bottomWallGhost = rowLinearIndexMap(i, 0);
				uint bottomWallFluid = rowLinearIndexMap(i, 1);
				x[bottomWallGhost] = -x[bottomWallFluid];
				
				uint topWallGhost = rowLinearIndexMap(i, m_numberCells + 1);
				uint topWallFluid = rowLinearIndexMap(i, m_numberCells);
				x[topWallGhost] = -x[topWallFluid];
			}

			/// Corners
			uint leftBottomGhost = rowLinearIndexMap(0, 0);
			uint rightBottomGhost = rowLinearIndexMap(0, m_numberCells + 1);
			uint leftTopGhost = rowLinearIndexMap(m_numberCells + 1, 0);
			uint rightTopGhost = rowLinearIndexMap(m_numberCells + 1, m_numberCells + 1);
			x[leftBottomGhost] = real(0.0);
			x[rightBottomGhost] = real(0.0);
			x[leftTopGhost] = real(0.0);
			x[rightTopGhost] = real(0.0);
		}

		void linearSolver(real * xNew, real * xOld, real a = real(0.0), real c = real(1.0))
		{
			/// Gauss-Seidel relaxation
			int relaxationSteps = 20;
			for (int steps = 0; steps < relaxationSteps; steps++)
			{
				for (uint j = 1; j <= m_numberCells; j++)
				{
					for (uint i = 1; i <= m_numberCells; i++)
					{
						uint cter = rowLinearIndexMap(i + 0, j + 0);
						uint east = rowLinearIndexMap(i + 1, j + 0);
						uint west = rowLinearIndexMap(i - 1, j + 0);
						uint nrth = rowLinearIndexMap(i + 0, j + 1);
						uint soth = rowLinearIndexMap(i + 0, j - 1);

						xNew[cter] = ((xOld[cter]) + a * (xNew[west] + xNew[east] + xNew[soth] + xNew[nrth])) / c;
					}
				}

				(m_boundary == PERIODIC) ? periodicBoundaryConditions(xNew) : dirichletBoundaryConditions(xNew);
			}
		}

		void diffuse(real * xNew, real * xOld, real diffuseTerm)
		{
			linearSolver(xNew, xOld, diffuseTerm);
		}

		void advect(real * xNew, real * xOld, const real * u, const real * v, real dt)
		{
			real dt0 = dt * m_numberCells;
			for (uint j = 1; j <= m_numberCells; j++)
			{
				for (uint i = 1; i <= m_numberCells; i++)
				{
					real x = i - dt0 * u[rowLinearIndexMap(i, j)]; 
					real y = j - dt0 * v[rowLinearIndexMap(i, j)];
					applyBoundaryCell(x);
					applyBoundaryCell(y);
					
					uint i0 = (uint)x; 
					uint i1 = i0 + 1;
					
					uint j0 = (uint)y; 
					uint j1 = j0 + 1;

					real s1 = x - i0; 
					real s0 = 1 - s1; 
					real t1 = y - j0; 
					real t0 = 1 - t1;

					xNew[rowLinearIndexMap(i, j)] = 
						s0*(t0*xOld[rowLinearIndexMap(i0, j0)] + t1*xOld[rowLinearIndexMap(i0, j1)]) +
						s1*(t0*xOld[rowLinearIndexMap(i1, j0)] + t1*xOld[rowLinearIndexMap(i1, j1)]);
				}
			}
			(m_boundary == PERIODIC) ? periodicBoundaryConditions(xNew) : dirichletBoundaryConditions(xNew);
		}
	
		void project()
		{
			for (uint j = 1; j <= m_numberCells; j++)
			{
				for (uint i = 1; i <= m_numberCells; i++)
				{
					uint cter = rowLinearIndexMap(i + 0, j + 0);
					uint east = rowLinearIndexMap(i + 1, j + 0);
					uint west = rowLinearIndexMap(i - 1, j + 0);
					uint nrth = rowLinearIndexMap(i + 0, j + 1);
					uint soth = rowLinearIndexMap(i + 0, j - 1);

					m_divergence[cter] = -real(0.5 * m_spacingCells) * (m_u1[east] - m_u1[west] + m_v1[nrth] - m_v1[soth]);
					m_pressure[cter] = real(0.0);
				}
			}
			(m_boundary == PERIODIC) ? periodicBoundaryConditions(m_divergence) : dirichletBoundaryConditions(m_divergence);
			(m_boundary == PERIODIC) ? periodicBoundaryConditions(m_pressure) : dirichletBoundaryConditions(m_pressure);

			linearSolver(m_pressure, m_divergence, real(1.0), real(4.0));

			for (uint j = 1; j <= m_numberCells; j++)
			{
				for (uint i = 1; i <= m_numberCells; i++)
				{
					uint cter = rowLinearIndexMap(i + 0, j + 0);
					uint east = rowLinearIndexMap(i + 1, j + 0);
					uint west = rowLinearIndexMap(i - 1, j + 0);
					uint nrth = rowLinearIndexMap(i + 0, j + 1);
					uint soth = rowLinearIndexMap(i + 0, j - 1);

					m_u1[cter] -= real(0.5 * m_numberCells) * (m_pressure[east] - m_pressure[west]);
					m_v1[cter] -= real(0.5 * m_numberCells) * (m_pressure[nrth] - m_pressure[soth]);
				}
			}
			(m_boundary == PERIODIC) ? periodicBoundaryConditions(m_u1) : dirichletBoundaryConditions(m_u1);
			(m_boundary == PERIODIC) ? periodicBoundaryConditions(m_v1) : dirichletBoundaryConditions(m_v1);
		}

	private:
		real m_spacingCells = 0.0;
		uint m_numberCells = 64;

		/// To enforce divergence free
		real * m_divergence = nullptr;
		real * m_pressure = nullptr;

		/// Old step
		real * m_d0 = nullptr;
		real * m_u0 = nullptr;
		real * m_v0 = nullptr;

		/// New step
		real * m_d1 = nullptr;
		real * m_u1 = nullptr;
		real * m_v1 = nullptr;

		/// Parameters
		real m_diffusion = real(0.0);
		real m_viscosity = real(0.0);

		/// Information from the fluid
		real m_maxDensity = -infinity;
		real m_minDensity = +infinity;
		real m_maxSpeed = -infinity;
		real m_minSpeed = +infinity;

		/// Obstacle handlers
		bool m_enabledObstacle = false;
	};
}