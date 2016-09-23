#pragma once

namespace FluidSimulation
{
	enum boundaryType
	{
		DIRICHLET = 0,
		PERIODIC
	};

	class BoundaryConditions
	{
	public:
		virtual ~BoundaryConditions(){};

		boundaryType getBoundaryType()
		{
			return m_boundary;
		}

		void switchBoundaryCondition()
		{
			m_boundary = (m_boundary == PERIODIC) ? DIRICHLET : PERIODIC;
		}

	protected:
		boundaryType m_boundary = PERIODIC;
	};
}