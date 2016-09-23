#pragma once
#include "Definitions.h"

namespace FluidSimulation
{
	class Button
	{
	public:
		Button(bool active, int x = 0, int y = 0, vec3 color = vec3(1.0), int w = 25, int h = 25)
			: m_active(active)
			, m_color(color)
			, m_x(x)
			, m_y(y)
			, m_w(w)
			, m_h(h)
		{

		}

		bool getState() const
		{
			return m_active;
		}

		int getScreenSpacePositionx() const
		{
			return m_x;
		}

		int getScreenSpacePositiony() const
		{
			return m_y;
		}

		int getHeight() const
		{
			return m_h;
		}

		int getWidth() const
		{
			return m_w;
		}

		vec3 getColor() const
		{
			return m_color;
		}

		bool checkClick(int cursorposx, int cursorposy)
		{
			if ((cursorposx > m_x) &&
				(cursorposx < (m_x + m_w)) &&
				(cursorposy > m_y) &&
				(cursorposy < (m_y + m_h)))
			{
				m_active ^= 1;
				return true;
			}
			return false;
		}

	private:
		bool m_active;
		vec3 m_color;
		int m_x;
		int m_y;
		int m_w;
		int m_h;
	};
}