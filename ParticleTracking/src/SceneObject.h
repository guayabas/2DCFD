#pragma once

namespace FluidSimulation
{
	class SceneObject
	{
	public:
		virtual ~SceneObject(){};

		bool isAnimated()
		{
			return m_animated;
		}

		void toggleAnimation()
		{
			m_animated ^= 1;
		}

	private:
		bool m_animated = false;
	};
}