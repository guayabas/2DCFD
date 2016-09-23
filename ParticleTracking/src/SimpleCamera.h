#pragma once
#include "Definitions.h"

#define ZOOM_STEP real(0.1)

namespace FluidSimulation
{
	class Camera
	{
	public:
		void zoomIn()
		{
			m_eye.z -= ZOOM_STEP;
			if (m_eye.z < real(1.0)) { m_eye.z = real(1.0); }
		}

		void zoomOut()
		{
			m_eye.z += ZOOM_STEP;
		}

		vec3 getEye() const
		{
			return m_eye;
		}

	private:
		vec3 m_eye = vec3(0.0, 0.0, 1.0);
	};
}