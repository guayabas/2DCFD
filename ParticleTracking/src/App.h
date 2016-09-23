#pragma once
#define ONE_PI real(3.14159265359)
#define TWO_PI real(6.28318530718)

#include <FreeImage/FreeImage.h>
#include <GLFW/glfw3.h>
#include <ctime>

#include "UtilitiesGraphics.h"
#include "SimpleCamera.h"
#include "Utilities.h"
#include "Scene.h"

namespace FluidSimulation
{
	class CFDApp
	{
	public:
		CFDApp()
		{
			init();
		}

		~CFDApp()
		{
			delete m_scene;
			delete m_camera;
		}

		void quit(int code = EXIT_SUCCESS)
		{
			glfwTerminate();
			std::exit(code);
		}

		void start()
		{
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

			uint frame = 0;
			while (!glfwWindowShouldClose(m_window))
			{
				m_previousTime = m_currentTime;
				m_currentTime = (real)glfwGetTime();
				real factorTime = real(0.5);
				real dt = factorTime * real(m_currentTime - m_previousTime);
				
				if (m_scene->isTakingVideo())
				{
					screenshoot(m_width, m_height, frame);
				}
				else
				{
					frame = 0;
				}

				m_scene->update(dt);

				glfwGetFramebufferSize(m_window, &m_width, &m_height);
				m_scene->resize(m_width, m_height);

				glViewport(0, 0, m_width, m_height);
				glClear(GL_COLOR_BUFFER_BIT);
				
				real spacing = m_scene->getFluidGridSpacing();

				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(-0.5, 0.5, -0.5, 0.5, 1.0, -1.0);

				glMatrixMode(GL_MODELVIEW);

				glLoadIdentity();
				real eyeCameraOffset = m_camera->getEye().z;
				glScalef(eyeCameraOffset, eyeCameraOffset, eyeCameraOffset);
				glScalef(1.0f - spacing, 1.0f - spacing, 1.0f - spacing);
				
				glTranslatef(-0.5f, -0.5f, 0.0f);
				m_scene->displaySimulation();

				glLoadIdentity();
				glTranslatef(-0.5f, -0.5f, 0.0f);
				m_scene->displayGUI();

				glfwSwapBuffers(m_window);
				glfwPollEvents();

				frame++;
			}

			glfwDestroyWindow(m_window);
		}

	protected:
		void init()
		{
			std::srand((uint)std::time(NULL));

			if (!glfwInit()) 
				std::exit(EXIT_FAILURE);

			std::string programName = "Particle Tracking - Alejandro Guayaquil";

			m_window = glfwCreateWindow(m_width, m_height, programName.c_str(), glfwGetPrimaryMonitor(), NULL);
			if (!m_window) 
				this->quit(EXIT_FAILURE);
			glfwMakeContextCurrent(m_window);

			if (glewInit() != GLEW_OK) 
				this->quit(EXIT_FAILURE);

			glfwSetMouseButtonCallback(m_window, mouse);
			glfwSetCursorPosCallback(m_window, motion);
			glfwSetScrollCallback(m_window, scroll);
			glfwSetKeyCallback(m_window, keyboard);
			glfwSwapInterval(1);

			graphicsCardInfo();

			m_scene->resize(m_width, m_height);
			m_scene->init();
		}

		static void keyboard(GLFWwindow * window, int key, int scancode, int action, int mods)
		{
			if (action == GLFW_PRESS)
			{
				switch (key)
				{
				case GLFW_KEY_ESCAPE:
				case GLFW_KEY_Q:
					glfwSetWindowShouldClose(window, GL_TRUE);
					break;

				case GLFW_KEY_F9:
					m_scene->toggleGUI();
					break;

				case GLFW_KEY_F10:
					m_scene->toggleParametersVisbiility();
					break;

				case GLFW_KEY_M:
					m_scene->decreaseFluidGrid();
					break;

				case GLFW_KEY_P:
					m_scene->increaseFluidGrid();
					break;

				case GLFW_KEY_B:
					m_scene->toggleFluidBoundaryCondition();
					break;

				case GLFW_KEY_C:
					m_scene->clearParticleSystem();
					break;

				case GLFW_KEY_R:
					m_scene->resetFluid();
					break;

				case GLFW_KEY_1:
					m_scene->toggleFluidAnimation();
					break;

				case GLFW_KEY_2:
					m_scene->toggleParticlesAnimation();
					break;

				case GLFW_KEY_3:
					m_scene->increaseFluidDiffusion();
					break;

				case GLFW_KEY_4:
					m_scene->decreaseFluidDifussion();
					break;

				case GLFW_KEY_5:
					m_scene->increaseFluidViscosity();
					break;

				case GLFW_KEY_6:
					m_scene->decreaseFluidViscosity();
					break;

				case GLFW_KEY_F1:
					m_scene->increaseFluidTimeStep();
					break;

				case GLFW_KEY_F2:
					m_scene->decreaseFluidTimeStep();
					break;

				case GLFW_KEY_F3:
					m_scene->increaseParticleSystemTimeStep();
					break;

				case GLFW_KEY_F4:
					m_scene->decreaseParticleSystemTimeStep();
					break;

				case GLFW_KEY_ENTER:
					break;

				case GLFW_KEY_S:
					screenshoot(m_scene->getWidth(), m_scene->getHeight(), 0);
					break;

				default:
					std::cout << "key pressed" << key << std::endl;
					break;
				}
			}
		}

		static void scroll(GLFWwindow * window, double x, double y)
		{
			(y > 0) ? m_camera->zoomIn() : m_camera->zoomOut();
		}

		static void motion(GLFWwindow * window, double x, double y)
		{
			m_scene->setCursorPosition((int) x, (int) y);
		}

		static void mouse(GLFWwindow * window, int button, int action, int mods)
		{
			switch (action)
			{
			case GLFW_PRESS:
				switch (button)
				{
				default:
				case GLFW_MOUSE_BUTTON_LEFT:
					m_scene->mousePressButton(LEFT_BUTTON);
					break;

				case GLFW_MOUSE_BUTTON_RIGHT:
					m_scene->mousePressButton(RIGHT_BUTTON);
					break;
				}
				break;

			case GLFW_RELEASE:
				switch (button)
				{
				default:
				case GLFW_MOUSE_BUTTON_LEFT:
					m_scene->mouseReleaseButton(LEFT_BUTTON);
					break;

				case GLFW_MOUSE_BUTTON_RIGHT:
					m_scene->mouseReleaseButton(RIGHT_BUTTON);
					break;
				}

			default:
				break;
			}
		}

	private:
		real m_previousTime = 0.0;
		real m_currentTime = 0.0;
		int m_height = 1080;
		int m_width = 1920;

	private:
		GLFWwindow * m_window;

	private:
		static Camera * m_camera;
		static Scene * m_scene;
	};

	/// Coding for five years in C++ / OpenGL and those pointer to functions
	/// are something very, very obscure ... 
	Camera * CFDApp::m_camera = new Camera();
	Scene * CFDApp::m_scene = new Scene();
}