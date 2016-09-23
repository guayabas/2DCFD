#pragma once
#include "Definitions.h"
#include <iostream>
#include <string>

namespace FluidSimulation
{
	static void screenshoot(int width, int height, unsigned int frame)
	{
		BYTE * pixels = new BYTE[3 * width * height];

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glReadPixels(0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, pixels);

		FIBITMAP * image = FreeImage_ConvertFromRawBits(pixels, width, height, 3 * width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);

		std::string fileOutput = "frames/Screen" + std::to_string(frame) + ".png";
		FreeImage_Save(FIF_PNG, image, fileOutput.c_str(), 0);

		FreeImage_Unload(image);
		delete[] pixels;
	}

	static void graphicsCardInfo()
	{
		const uchar * glslversion;
		const uchar * glversion;
		const uchar * renderer;
		const uchar * vendor;

		glslversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
		glversion = glGetString(GL_VERSION);
		renderer = glGetString(GL_RENDERER);
		vendor = glGetString(GL_VENDOR);

		std::cout << "OpenGL Version : " << glversion << std::endl;
		std::cout << "GLSL Version : " << glslversion << std::endl;
		std::cout << "Renderer : " << renderer << std::endl;
		std::cout << "Vendor : " << vendor << std::endl;
	}
}