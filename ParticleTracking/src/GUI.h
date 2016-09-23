#pragma once
#include <algorithm>
#include "Text2D.h"
#include "Button.h"
#include <vector>

namespace FluidSimulation
{
	class GUI
	{
	public:
		void init()
		{
			addButton("PressureField");
			addButton("VelocityField");
			addButton("Wireframe");
			addButton("TakeVideo");
			addButton("SwitchMap");
			addButton("Obstacle");
			addButton("Grid");
			
			m_text2D.init();
		}

		void addButton(std::string buttonName = "")
		{
			if (m_buttons.empty())
			{
				m_buttons.push_back(std::make_pair(buttonName, Button(true)));
			}
			else
			{
				auto & lastButton = m_buttons.back();
				auto x = lastButton.second.getScreenSpacePositionx();
				auto y = lastButton.second.getScreenSpacePositiony() + lastButton.second.getHeight();

				Button newButton(false, x, y);
				m_buttons.push_back(std::make_pair(buttonName, newButton));
			}
		}

		bool getButtonState(std::string name) const
		{
			auto positionContainer = std::find_if(m_buttons.begin(), m_buttons.end(), comparison(name));
			if (positionContainer != m_buttons.end())
			{
				return (*positionContainer).second.getState();
			}
			return false;
		}

		void resize(int width, int height)
		{
			m_text2D.resize(width, height);
			m_height = height;
			m_width = width;
		}

		bool checkClick(int x, int y)
		{
			bool buttonClicked = false;
			for (auto & button : m_buttons)
			{
				buttonClicked ^= button.second.checkClick(x, y);
			}
			return buttonClicked;
		}

		const std::vector<std::pair<std::string, Button>> & getButtons() const
		{
			return m_buttons;
		}

		void displayText(const char * text, int x, int y, int size = 25) const
		{
			m_text2D.renderText2D(text, x, y, size);
		}

		void toggle()
		{
			m_enabled = !m_enabled;
		}

		void switchParametersVisibility()
		{
			m_showParameters = !m_showParameters;
		}

		bool isParametersVisible() const
		{
			return m_showParameters;
		}

		bool isEnabled() const
		{
			return m_enabled;
		}

	private:
		std::vector<std::pair<std::string, Button>> m_buttons;
		Text2D m_text2D;

		bool m_showParameters = true;
		bool m_enabled = true;

		int m_height;
		int m_width;

	private:
		struct comparison
		{
			comparison(std::string const & s) : m_s(s) {}

			bool operator() (std::pair<std::string, Button> const & p)
			{
				return (p.first == m_s);
			}

			std::string m_s;
		};
	};
}