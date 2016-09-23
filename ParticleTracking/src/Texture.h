#pragma once
#include <cstring>
#include <cstdlib>
#include <cstdio>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

namespace FluidSimulation
{
	class ImageLoaders
	{
	public:
		static GLuint loadDDS(const char * filePath)
		{
			unsigned char header[124];
			FILE * filePointer = fopen(filePath, "rb");
			if (filePointer == NULL)
				return 0;

			/// Very interesting how the DDS - https://en.wikipedia.org/wiki/DirectDraw_Surface
			char fileCode[4];
			fread(fileCode, 1, 4, filePointer);
			if (strncmp(fileCode, "DDS ", 4) != 0)
			{
				fclose(filePointer);
				return 0;
			}

			fread(&header, 124, 1, filePointer);

			unsigned int mipmapCount = *(unsigned int *)&(header[24]);
			unsigned int linearSize = *(unsigned int *)&(header[16]);
			unsigned int fourCC = *(unsigned int *)&(header[80]);
			unsigned int height = *(unsigned int *)&(header[8]);
			unsigned int width = *(unsigned int *)&(header[12]);

			unsigned int bufferSize;
			unsigned char * buffer;

			bufferSize = mipmapCount > 1 ? linearSize * 2 : linearSize;
			buffer = (unsigned char *)malloc(bufferSize * sizeof(unsigned char));
			fread(buffer, 1, bufferSize, filePointer);
			fclose(filePointer);

			unsigned int format;
			switch (fourCC)
			{
			case FOURCC_DXT1:
				format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
				break;

			case FOURCC_DXT3:
				format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
				break;

			case FOURCC_DXT5:
				format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
				break;

			default:
				free(buffer);
				return 0;
				break;
			}

			GLuint textureID;
			glGenTextures(1, &textureID);

			glBindTexture(GL_TEXTURE_2D, textureID);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
			unsigned int offset = 0;

			for (unsigned int level = 0; level < mipmapCount && (width || height); ++level)
			{
				unsigned int size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
				glCompressedTexImage2D(
					GL_TEXTURE_2D, 
					level, 
					format, 
					width, 
					height,
					0, 
					size, 
					buffer + offset);

				offset += size;
				width /= 2;
				height /= 2;

				if (width < 1) width = 1;
				if (height < 1) height = 1;
			}

			free(buffer);
			return textureID;
		}
	};
}