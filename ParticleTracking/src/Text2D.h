#pragma once
#include "Texture.h"
#include "Shaders.h"

namespace FluidSimulation
{
	class Text2D
	{
	public:
		void init()
		{
			m_text2DTextureID = ImageLoaders::loadDDS("./data/latin.dds");

			glGenBuffers(1, &m_text2DTextureBufferID);
			glGenBuffers(1, &m_text2DVertexBufferID);
			
			m_text2DShaderID = Shaders::Load("./shaders/Textvs.glsl", "./shaders/Textfs.glsl");
			m_text2DUniformID = glGetUniformLocation(m_text2DShaderID, "myTextureSampler");
			m_text2DWindowDimensionsUniformID = glGetUniformLocation(m_text2DShaderID, "windowDimensions");
		}

		void renderText2D(const char * text, int x, int y, int size) const
		{
			unsigned int length = (unsigned int)strlen(text);

			std::vector<vec2> vertices;
			std::vector<vec2> textures;
			for (unsigned int i = 0; i < length; i++) 
			{
				vec2 vertex_up_right = vec2(x + i * size + size, y + size);
				vec2 vertex_down_right = vec2(x + i * size + size, y);
				vec2 vertex_up_left = vec2(x + i * size, y + size);
				vec2 vertex_down_left = vec2(x + i * size, y);

				vertices.push_back(vertex_up_left);
				vertices.push_back(vertex_down_left);
				vertices.push_back(vertex_up_right);

				vertices.push_back(vertex_down_right);
				vertices.push_back(vertex_up_right);
				vertices.push_back(vertex_down_left);

				char character = text[i];
				real uv_x = (character % 16) / real(16.0);
				real uv_y = (character / 16) / real(16.0);

				vec2 uv_up_left = vec2(uv_x, uv_y);
				vec2 uv_up_right = vec2(uv_x + real(1.0) / real(16.0), uv_y);
				vec2 uv_down_right = vec2(uv_x + real(1.0) / real(16.0), (uv_y + real(1.0) / real(16.0)));
				vec2 uv_down_left = vec2(uv_x, (uv_y + real(1.0) / real(16.0)));
				textures.push_back(uv_up_left);
				textures.push_back(uv_down_left);
				textures.push_back(uv_up_right);

				textures.push_back(uv_down_right);
				textures.push_back(uv_up_right);
				textures.push_back(uv_down_left);
			}

			glBindBuffer(GL_ARRAY_BUFFER, m_text2DVertexBufferID);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), &vertices[0], GL_STATIC_DRAW);
			
			glBindBuffer(GL_ARRAY_BUFFER, m_text2DTextureBufferID);
			glBufferData(GL_ARRAY_BUFFER, textures.size() * sizeof(vec2), &textures[0], GL_STATIC_DRAW);

			glUseProgram(m_text2DShaderID);

			glUniform2f(m_text2DWindowDimensionsUniformID, (real)m_windowWidth, (real)m_windowHeight);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_text2DTextureID);
			glUniform1i(m_text2DUniformID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, m_text2DVertexBufferID);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, m_text2DTextureBufferID);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnable(GL_BLEND);
			glDrawArrays(GL_TRIANGLES, 0, (GLsizei)vertices.size());
			glDisable(GL_BLEND);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}

		~Text2D()
		{
			glDeleteProgram(m_text2DShaderID);

			glDeleteBuffers(1, &m_text2DVertexBufferID);
			glDeleteBuffers(1, &m_text2DTextureBufferID);

			glDeleteTextures(1, &m_text2DTextureID);
		}

		void resize(int width, int height)
		{
			m_windowHeight = height;
			m_windowWidth = width;
		}

	private:
		uint m_text2DWindowDimensionsUniformID;
		uint m_text2DTextureBufferID;
		uint m_text2DVertexBufferID;
		uint m_text2DUniformID;
		uint m_text2DTextureID;
		uint m_text2DShaderID;

		int m_windowHeight;
		int m_windowWidth;
	};
}