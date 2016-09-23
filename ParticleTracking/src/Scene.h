#pragma once
#include "SceneObject.h"
#include "Renderer.h"

namespace FluidSimulation
{
	enum buttonPressed
	{
		LEFT_BUTTON = 0,
		RIGHT_BUTTON
	};

	class Scene
	{
	public:
		~Scene()
		{
			delete m_gui;
			delete m_fluid;
			delete m_renderer;
			delete m_particles;
		}

		void init()
		{
			m_fluid->init();
			m_gui->init();
		}

		void update(real dt)
		{
			m_dt = dt;

			bool enabledObstacle = m_gui->getButtonState("Obstacle");
			m_fluid->setObstacle(enabledObstacle);

			if (m_fluid->isAnimated())
			{
				m_fluid->update();
			}

			if (m_particles->isAnimated())
			{
				m_particles->update(m_fluid);
			}

			m_renderer->setDensityColorMap(m_fluid->getMaxDensity(), m_fluid->getMinDensity());
			m_renderer->setSpeedColorMap(m_fluid->getMaxSpeed(), m_fluid->getMinSpeed());
		}

		void displaySimulation()
		{
			m_renderer->render(*m_particles, *m_fluid);

			if (m_gui->getButtonState("Wireframe"))
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}

			m_renderer->render(*m_fluid, *m_gui);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		void displayGUI()
		{
			m_renderer->render(*m_gui);

			char dynamicText[256];

			if (m_gui->isEnabled())
			{
				m_gui->displayText("F9HideGUI", m_width - (25 * 9), m_height - 25);

				std::string textParticlesAnimation = !m_particles->isAnimated() ? "2:AnimateParticles" : "2:PauseParticles";
				m_gui->displayText(textParticlesAnimation.c_str(), m_width - (25 * 17), m_height - 50);

				m_gui->displayText("c:ClearParticles", m_width - (25 * 16), m_height - 75);

				std::string textFluidAnimation = !m_fluid->isAnimated() ? "1:AnimateFluid" : "1:PauseFluid";
				m_gui->displayText(textFluidAnimation.c_str(), m_width - (25 * 14), m_height - 100);
				
				m_gui->displayText("p:IncreaseGrid", m_width - (25 * 14), m_height - 125);
				m_gui->displayText("m:DecreaseGrid", m_width - (25 * 14), m_height - 150);
				m_gui->displayText("r:ResetFluid", m_width - (25 * 13), m_height - 175);
			}
			else
			{
				m_gui->displayText("F9ShowGUI", m_width - (25 * 9), m_height - 25);
			}

			if (m_gui->isParametersVisible())
			{
				m_gui->displayText("F10HideParameters", 0, m_height - 25);

				sprintf(dynamicText, "NumParticles%d", m_particles->getNumberParticles());
				m_gui->displayText(dynamicText, 0, m_height - 50);

				sprintf(dynamicText, "NumCells%d", m_fluid->getNumCells());
				m_gui->displayText(dynamicText, 0, m_height - 75);

				if (m_fluid->getBoundaryType() == PERIODIC)
				{
					m_gui->displayText("Periodic", 0, m_height - 100);
				}
				else
				{
					m_gui->displayText("Dirichlet", 0, m_height - 100);
				}

				sprintf(dynamicText, "Particle dt:%.3f", m_particles->getTimeIntegrationStep());
				m_gui->displayText(dynamicText, 0, m_height - 125);

				sprintf(dynamicText, "Fluid dt:%.3f", m_fluid->getTimeIntegrationStep());
				m_gui->displayText(dynamicText, 0, m_height - 150);

				sprintf(dynamicText, "Diffusion:%.3f", m_fluid->getDiffusion());
				m_gui->displayText(dynamicText, 0, m_height - 175);

				sprintf(dynamicText, "Viscosity:%.3f", m_fluid->getViscosity());
				m_gui->displayText(dynamicText, 0, m_height - 200);
			}
			else
			{
				m_gui->displayText("F10ShowParameters", 0, m_height - 25);
			}

			sprintf(dynamicText, "%.5fsec", m_dt);
			m_gui->displayText(dynamicText, m_width - (25 * 10), 0);

			sprintf(dynamicText, "Maxspeed%.5fsec", m_fluid->getMaxSpeed());
			m_gui->displayText(dynamicText, m_width - (25 * 15), 25);

			sprintf(dynamicText, "Minspeed%.5fsec", m_fluid->getMinSpeed());
			m_gui->displayText(dynamicText, m_width - (25 * 15), 50);

			sprintf(dynamicText, "Maxdensity%.5fsec", m_fluid->getMaxDensity());
			m_gui->displayText(dynamicText, m_width - (25 * 17), 75);

			sprintf(dynamicText, "Mindensity%.5fsec", m_fluid->getMinDensity());
			m_gui->displayText(dynamicText, m_width - (25 * 17), 100);
		}

		void toggleGUI()
		{
			m_gui->toggle();
		}

		void toggleParametersVisbiility()
		{
			m_gui->switchParametersVisibility();
		}

		void toggleFluidBoundaryCondition()
		{
			m_particles->switchBoundaryCondition();
			m_fluid->switchBoundaryCondition();
		}

		real getFluidGridSpacing()
		{
			return (real(1.0) / real(m_fluid->getNumCells()));
		}

		void increaseFluidGrid()
		{
			m_fluid->increaseGridSize();
		}

		void decreaseFluidGrid()
		{
			m_fluid->decreaseGridSize();
		}

		void increaseFluidViscosity()
		{
			m_fluid->increaseViscosity();
		}

		void decreaseFluidViscosity()
		{
			m_fluid->decreaseViscosity();
		}

		void increaseFluidDiffusion()
		{
			m_fluid->increaseDiffusion();
		}

		void decreaseFluidDifussion()
		{
			m_fluid->decreaseDiffusion();
		}

		void increaseFluidTimeStep()
		{
			m_fluid->increaseTimeStep();
		}

		void decreaseFluidTimeStep()
		{
			m_fluid->decreaseTimeStep();
		}

		void increaseParticleSystemTimeStep()
		{
			m_particles->increaseTimeStep();
		}

		void decreaseParticleSystemTimeStep()
		{
			m_particles->decreaseTimeStep();
		}

		void resetFluid()
		{
			m_fluid->init();
		}

		void clearParticleSystem()
		{
			m_particles->clear();
		}

		void toggleParticlesAnimation()
		{
			m_particles->toggleAnimation();
		}

		void toggleFluidAnimation()
		{
			m_fluid->toggleAnimation();
		}

		void resize(int width, int height)
		{
			m_renderer->resize(width, height);
			m_gui->resize(width, height);

			m_height = height;
			m_width = width;
		}

		void setCursorPosition(int x, int y)
		{
			m_cursorposxnew = (x);
			m_cursorposynew = (m_height - y);

			if (m_buttonRightEnabled)
			{
				if (x < 0 || x > m_width || y < 0 || y > m_height) return;

				int N = m_fluid->getNumCells();
				int i = (int)((m_cursorposxold / (float)m_width) * N + 1);
				int j = (int)((m_cursorposyold / (float)m_height) * N + 1);

				real fx = real(m_cursorposxnew - m_cursorposxold);
				real fy = real(m_cursorposynew - m_cursorposyold);

				m_fluid->addDrag(i, j, vec2(fx, fy));
			}

			m_cursorposxold = m_cursorposxnew;
			m_cursorposyold = m_cursorposynew;
		}

		void mousePressButton(buttonPressed button)
		{
			m_cursorposxold = m_cursorposxnew;
			m_cursorposyold = m_cursorposynew;

			switch (button)
			{
			default:
			case LEFT_BUTTON:
				m_buttonLeftEnabled = true;
				if (!m_gui->checkClick(m_cursorposxold, m_cursorposyold))
				{
					vec2 pos((m_cursorposxold / real(m_width)), (m_cursorposyold / real(m_height)));

					Particle particle;
					particle.setPosition(pos);
					m_particles->addParticle(particle);
				}
				break;

			case RIGHT_BUTTON:
				m_buttonRightEnabled = true;
				break;
			}
		}

		void mouseReleaseButton(buttonPressed button)
		{
			switch (button)
			{
			default:
			case LEFT_BUTTON:
				m_buttonLeftEnabled = false;
				break;

			case RIGHT_BUTTON:
				m_buttonRightEnabled = false;
				break;
			}
		}

		int getWidth() const
		{
			return m_width;
		}

		int getHeight() const
		{
			return m_height;
		}

		void toggleVideo()
		{
			m_video ^= 1;
		}

		bool isTakingVideo()
		{
			return m_gui->getButtonState("TakeVideo");
		}

	private:
		ParticleSystem * m_particles = new ParticleSystem;
		Renderer * m_renderer = new Renderer;
		Fluid * m_fluid = new Fluid;
		GUI * m_gui = new GUI;

		bool m_video = false;
        real m_dt;
        int m_height;
		int m_width;
        
		bool m_buttonRightEnabled = false;
		bool m_buttonLeftEnabled = false;
		int m_cursorposxnew;
		int m_cursorposynew;
		int m_cursorposxold;
		int m_cursorposyold;
	};
}

