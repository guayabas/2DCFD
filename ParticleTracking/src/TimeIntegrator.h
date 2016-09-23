#pragma once

namespace FluidSimulation
{
	class TimeIntegrator
	{
	public:
		virtual ~TimeIntegrator(){}

		real getTimeIntegrationStep() const
		{
			return m_timeStep;
		}

		void increaseTimeStep()
		{
			m_timeStep += m_timeIncrement;
			if (m_timeStep > real(1.0)) { m_timeStep = real(1.0); return; }
		}

		void decreaseTimeStep()
		{
			m_timeStep -= m_timeIncrement;
			if (m_timeStep < m_timeIncrement) { m_timeStep = m_timeIncrement; return; }
		}

		void setTimeIncrement(real increment)
		{
			m_timeIncrement = increment;
		}

		void setTimeStep(real step)
		{
			m_timeStep = step;
		}

	protected:
		real m_timeIncrement;
		real m_timeStep;
	};
}