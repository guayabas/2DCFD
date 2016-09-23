#pragma once
#include "ParticleSystem.h"
#include "ColorGradient.h"
#include "Shaders.h"
#include "Fluid.h"
#include "GUI.h"

#define POINT_WIDTH_PIXELS real(10.0)
#define LINE_WIDTH_PIXELS real(3.0)

namespace FluidSimulation
{
	enum GHOST_CELLS
	{
		BOTTOM_WALL = 0,
		RIGHT_WALL,
		LEFT_WALL,
		TOP_WALL,
		LAST
	};

	class Renderer
	{
	public:
		Renderer()
		{
			m_hetMapGradient.clearGradient();
			m_waterGradient.clearGradient();
			m_rbGradient.clearGradient();
		}

		void resize(int width, int height)
		{
			m_height = height;
			m_width = width;
		}

		void render(Fluid & fluid, const GUI & gui)
		{
			if (gui.getButtonState("VelocityField"))
			{
				renderVelocity(fluid);
			}

			if (gui.getButtonState("PressureField"))
			{
				if (gui.getButtonState("SwitchMap"))
				{
					renderDensity(fluid, m_hetMapGradient);
				}
				else
				{
					renderDensity(fluid, m_waterGradient);
				}
			}

			if (gui.getButtonState("Grid"))
			{
				renderCenterCells(fluid);
				renderGhostCells(fluid);
				renderDomain(fluid);
			}
		}

		void render(const ParticleSystem & particles, Fluid & fluid)
		{
			renderParticles(particles, fluid);
		}

		void render(const GUI & gui)
		{
			using namespace glm;

			if (gui.isEnabled())
			{
				for (auto & buttonPair : gui.getButtons())
				{
					auto & button = buttonPair.second;

					double xButton = button.getScreenSpacePositionx();
					xButton /= double(m_width);

					double yButton = button.getScreenSpacePositiony();
					yButton /= double(m_height);

					double wButton = button.getWidth();
					wButton /= double(m_width);

					double hButton = button.getHeight();
					hButton /= double(m_height);

					gui.displayText(buttonPair.first.c_str(), button.getWidth(), button.getScreenSpacePositiony());

					auto state = button.getState();

					auto v0 = vec2(xButton, yButton);
					auto v1 = vec2(v0.x + wButton, v0.y);
					auto v2 = vec2(v0.x + wButton, v0.y + hButton);
					auto v3 = vec2(v0.x, v0.y + hButton);

					glUseProgram(0);
					if (state)
					{
						glColor4fv(value_ptr(vec4(button.getColor(), 0.75)));
					}
					else
					{
						glColor4fv(value_ptr(vec4(button.getColor(), 0.25)));
					}

					glEnable(GL_BLEND);

					glBegin(GL_QUADS);
					glVertex2fv(value_ptr(v0));
					glVertex2fv(value_ptr(v1));
					glVertex2fv(value_ptr(v2));
					glVertex2fv(value_ptr(v3));
					glEnd();

					glColor3d(0.0, 0.0, 0.0);
					glBegin(GL_LINES);
					glVertex2fv(value_ptr(v0));
					glVertex2fv(value_ptr(v1));
					glVertex2fv(value_ptr(v1));
					glVertex2fv(value_ptr(v2));
					glVertex2fv(value_ptr(v2));
					glVertex2fv(value_ptr(v3));
					glVertex2fv(value_ptr(v3));
					glVertex2fv(value_ptr(v0));
					glEnd();
					
					glDisable(GL_BLEND);
				}
			}
		}

		void setSpeedColorMap(real maxspeed, real minspeed)
		{
			m_rbGradient.clearGradient();
			m_rbGradient.addColorPoint(real(1.0), real(0.0), real(0.0), maxspeed);
			m_rbGradient.addColorPoint(real(0.0), real(0.0), real(1.0), minspeed);
		}

		void setDensityColorMap(real maxdensity, real mindensity)
		{
			m_hetMapGradient.clearGradient();
			m_waterGradient.clearGradient();
		
			real deltaColor = (maxdensity - mindensity) / real(4);
			
			m_waterGradient.addColorPoint(real(135. / 255.), real(206. / 255.), real(235. / 255.), mindensity); // Sky blue
			m_waterGradient.addColorPoint(real(49. / 255.), real(140. / 255.), real(231. / 255.), mindensity + (1 * deltaColor)); // Bleu de France
			m_waterGradient.addColorPoint(real(70. / 255.), real(130. / 255.), real(180. / 255.), mindensity + (2 * deltaColor)); // Steel blue
			m_waterGradient.addColorPoint(real(0.0), real(112. / 255.), real(184. / 255.), mindensity + (3 * deltaColor)); // Spanish blue
			m_waterGradient.addColorPoint(real(0.0), real(0.0), real(1.0), maxdensity); // Navy blue

			m_hetMapGradient.addColorPoint(real(0.0), real(0.0), real(1.0), mindensity);
			m_hetMapGradient.addColorPoint(real(0.0), real(1.0), real(1.0), mindensity + (1 * deltaColor));
			m_hetMapGradient.addColorPoint(real(0.0), real(1.0), real(0.0), mindensity + (2 * deltaColor));
			m_hetMapGradient.addColorPoint(real(1.0), real(1.0), real(0.0), mindensity + (3 * deltaColor));
			m_hetMapGradient.addColorPoint(real(1.0), real(0.0), real(0.0), maxdensity);
		}

	protected:
		void renderVelocity(Fluid & fluid)
		{
			using namespace glm;
			glUseProgram(0);

			uint numCells = fluid.getNumCells();
			real spacingCells = real(1.0) / numCells;
			real scalingFactor = real(1.0) * spacingCells;

			glLineWidth(LINE_WIDTH_PIXELS);
			glBegin(GL_LINES);

			for (uint i = 1; i <= numCells; i++)
			{
				real x = (i - real(0.5)) * spacingCells;
				for (uint j = 1; j <= numCells; j++)
				{
					real y = (j - real(0.5)) * spacingCells;

					real u = fluid.getVelocityU(i, j);
					real v = fluid.getVelocityV(i, j);

					vec2 v0 = vec2(x, y);
					vec2 v1 = vec2(x + (scalingFactor * u), y + (scalingFactor * v));

					vec3 colorVelocity;
					real speed = glm::length(vec2(u, v));

					m_rbGradient.getColorAtValue(speed, colorVelocity.x, colorVelocity.y, colorVelocity.z);

					glColor3fv(value_ptr(vec3(1.0) - colorVelocity));
					glVertex2fv(value_ptr(v0));

					glColor3fv(value_ptr(colorVelocity));
					glVertex2fv(value_ptr(v1));
				}
			}

			glEnd();
			glLineWidth(1.0f);
		}

		void renderDensity(Fluid & fluid, ColorGradient & colormap)
		{
			using namespace glm;
			glUseProgram(0);

			uint numCells = fluid.getNumCells();
			real spacingCells = real(1.0) / numCells;

			glEnable(GL_BLEND);
			glBegin(GL_QUADS);

			for (uint i = 0; i <= numCells; i++) 
			{
				real x = (i - 0.5f)*spacingCells;
				for (uint j = 0; j <= numCells; j++) 
				{
					real y = (j - 0.5f)*spacingCells;

					real d00 = fluid.getDensity(i + 0, j + 0);
					real d01 = fluid.getDensity(i + 0, j + 1);
					real d10 = fluid.getDensity(i + 1, j + 0);
					real d11 = fluid.getDensity(i + 1, j + 1);

					vec2 v0 = vec2(x, y);
					vec2 v1 = vec2(x + spacingCells, y);
					vec2 v2 = vec2(x + spacingCells, y + spacingCells);
					vec2 v3 = vec2(x, y + spacingCells);

					vec4 c0;
					vec4 c1;
					vec4 c2;
					vec4 c3;
					colormap.getColorAtValue(d00, c0.r, c0.g, c0.b);
					c0.w = real(0.5);
					colormap.getColorAtValue(d10, c1.r, c1.g, c1.b);
					c1.w = real(0.5);
					colormap.getColorAtValue(d11, c2.r, c2.g, c2.b);
					c2.w = real(0.5);
					colormap.getColorAtValue(d01, c3.r, c3.g, c3.b);
					c3.w = real(0.5);

					if (fluid.isObstacleEnabled() && fluid.isAnimated())
					{
						real epsilon = std::numeric_limits<real>::epsilon();

						if (std::abs(d00) < epsilon ||
							std::abs(d01) < epsilon ||
							std::abs(d10) < epsilon ||
							std::abs(d11) < epsilon)
						{
							c0.xyz() = vec3(0.0);
							c1.xyz() = vec3(0.0);
							c2.xyz() = vec3(0.0);
							c3.xyz() = vec3(0.0);
						}
					}

					glColor4fv(value_ptr(c0)); 
					glVertex2fv(value_ptr(v0));
					
					glColor4fv(value_ptr(c1));
					glVertex2fv(value_ptr(v1));
					
					glColor4fv(value_ptr(c2));
					glVertex2fv(value_ptr(v2));
					
					glColor4fv(value_ptr(c3));
					glVertex2fv(value_ptr(v3));
				}
			}

			glEnd();
			glDisable(GL_BLEND);
		}

		void renderCenterCells(Fluid & fluid)
		{
			using namespace glm;
			glUseProgram(0);

			uint numCells = fluid.getNumCells();
			real spacingCells = real(1.0) / numCells;

			glPointSize(POINT_WIDTH_PIXELS);
			glColor3f(0.0f, 0.0f, 0.0f);
			glBegin(GL_POINTS);
			for (uint i = 1; i <= numCells; i++)
			{
				real x = (i - real(0.5)) * spacingCells;
				for (uint j = 1; j <= numCells; j++)
				{
					real y = (j - real(0.5)) * spacingCells;
					glVertex2f(x, y);
				}
			}
			glEnd();
			glPointSize(1.0f);
		}

		void renderDomain(Fluid & fluid)
		{
			using namespace glm;
			glUseProgram(0);

			uint numCells = fluid.getNumCells();
			real spacingCells = real(1.0) / numCells;

			glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
			glLineWidth(LINE_WIDTH_PIXELS);
			glEnable(GL_BLEND);
			
			for (uint i = 1; i <= numCells; i++)
			{
				real x = (i - 1) * spacingCells;
				for (uint j = 1; j <= numCells; j++)
				{
					real y = (j - 1) * spacingCells;

					vec2 v0 = vec2(x, y);
					vec2 v1 = vec2(x + spacingCells, y);
					vec2 v2 = vec2(x + spacingCells, y + spacingCells);
					vec2 v3 = vec2(x, y + spacingCells);

					renderFancyCell(v0, v1, v2, v3);
				}
			}

			glDisable(GL_BLEND);
			glLineWidth(1.0f);
		}

		void renderGhostCells(Fluid & fluid)
		{
			using namespace glm;
			glUseProgram(0);

			uint numCells = fluid.getNumCells();
			real spacingCells = real(1.0) / numCells;

			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glLineWidth(LINE_WIDTH_PIXELS);
			glEnable(GL_BLEND);
			
			for (int n = 0; n < int(numCells + 2); n++)
			{
				for (uint ghostCellID = 0; ghostCellID < LAST; ghostCellID++)
				{
					real x;
					real y;
					switch ((GHOST_CELLS)ghostCellID)
					{
					case BOTTOM_WALL:
						x = (n - 1) * spacingCells;
						y = -spacingCells;
						break;

					case RIGHT_WALL:
						x = numCells * spacingCells;
						y = (n - 1) * spacingCells;
						break;

					case LEFT_WALL:
						x = -spacingCells;
						y = (n - 1) * spacingCells;;
						break;

					case TOP_WALL:
						x = (n - 1) * spacingCells;
						y = numCells * spacingCells;
						break;
                            
                    case LAST:
                        /// Do nothing, is for automatic adding extra features
                        break;
					}

					vec2 v0 = vec2(x, y);
					vec2 v1 = vec2(x + spacingCells, y);
					vec2 v2 = vec2(x + spacingCells, y + spacingCells);
					vec2 v3 = vec2(x, y + spacingCells);

					renderFancyCell(v0, v1, v2, v3);
				}				
			}

			glDisable(GL_BLEND);
			glLineWidth(1.0f);
		}

		void renderFancyCell(vec2 & v0, vec2 & v1, vec2 & v2, vec2 & v3)
		{
			glBegin(GL_QUADS);
			glVertex2fv(value_ptr(v0));
			glVertex2fv(value_ptr(v1));
			glVertex2fv(value_ptr(v2));
			glVertex2fv(value_ptr(v3));
			glEnd();

			glBegin(GL_LINES);
			glVertex2fv(value_ptr(v0));
			glVertex2fv(value_ptr(v1));
			glVertex2fv(value_ptr(v1));
			glVertex2fv(value_ptr(v2));
			glVertex2fv(value_ptr(v2));
			glVertex2fv(value_ptr(v3));
			glVertex2fv(value_ptr(v3));
			glVertex2fv(value_ptr(v0));
			glEnd();
		}

		void renderParticles(const ParticleSystem & particles, Fluid & fluid)
		{
			using namespace glm;
			glUseProgram(0);

			glEnable(GL_BLEND);
			glBegin(GL_QUADS);

			uint numParticlesTrailing = particles.getNumberTrailingParticles();
			for (auto & particle : particles.getParticles())
			{
				auto particleTrailing = particle.getTrailing();
				real pixelSizeParticle = real(0.01);

				for (uint n = 0; n < numParticlesTrailing; n++)
				{
					pixelSizeParticle -= real(0.0002);

					real unitColor = real(1.0) - (n / real(numParticlesTrailing - 1));
					real alphaBlending = real(0.9) - (real(0.85) * (n / real(numParticlesTrailing - 1)));
					real weight = particle.getWeight();

					vec4 particleColor;
					if (weight < real(0.25))
					{
						particleColor = vec4(unitColor, 0.0, 0.0, alphaBlending);
					}
					else if (weight > real(0.25) && weight < real(0.50))
					{
						particleColor = vec4(0.0, unitColor, 0.0, alphaBlending);
					}
					else if (weight > real(0.50) && weight < real(0.75))
					{
						particleColor = vec4(0.0, 0.0, unitColor, alphaBlending);
					}
					else
					{
						particleColor = vec4(vec3(unitColor), alphaBlending);
					}

					glColor4fv(value_ptr(particleColor));

					vec2 particlePosition = particleTrailing[n];
					vec2 v0 = particlePosition;
					vec2 v1 = vec2(v0.x + pixelSizeParticle, v0.y);
					vec2 v2 = vec2(v0.x + pixelSizeParticle, v0.y + pixelSizeParticle);
					vec2 v3 = vec2(v0.x, v0.y + pixelSizeParticle);

					glVertex2fv(value_ptr(v0));
					glVertex2fv(value_ptr(v1));
					glVertex2fv(value_ptr(v2));
					glVertex2fv(value_ptr(v3));
				}
			}

			glEnd();
			glDisable(GL_BLEND);
		}

	private:
		ColorGradient m_hetMapGradient;
		ColorGradient m_waterGradient;
		ColorGradient m_rbGradient;
		int m_height = 0;
		int m_width = 0;
	};
}