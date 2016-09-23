#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace FluidSimulation
{
	class Shaders
	{
	public:
		static GLuint Load(const char * vertexShaderFile, const char * fragmentShaderFile)
		{
			GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
			GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);

			std::string vertexShaderCode;

			std::ifstream vertexShaderStream(vertexShaderFile, std::ios::in);
			if (vertexShaderStream.is_open()) 
			{
				std::string line = "";
				while (getline(vertexShaderStream, line))
				{
					vertexShaderCode += "\n" + line;
				}
				vertexShaderStream.close();
			}

			std::string fragmentShaderCode;

			std::ifstream fragmentShaderStream(fragmentShaderFile, std::ios::in);
			if (fragmentShaderStream.is_open())
			{
				std::string line = "";
				while (getline(fragmentShaderStream, line))
				{
					fragmentShaderCode += "\n" + line;
				}
				fragmentShaderStream.close();
			}

			GLint result = GL_FALSE;
			int infoLogLength;

			printf("Compiling shader : %s\n", vertexShaderFile);
			const char * vertexSourcePointer = vertexShaderCode.c_str();
			glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
			glCompileShader(vertexShaderID);

			glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
			glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0) 
			{
				std::vector<char> vertexShaderErrorMessage(infoLogLength + 1);
				glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
				printf("%s\n", &vertexShaderErrorMessage[0]);
			}

			printf("Compiling shader : %s\n", fragmentShaderFile);
			const char * fragmentSourcePointer = fragmentShaderCode.c_str();
			glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
			glCompileShader(fragmentShaderID);

			glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
			glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0)
			{
				std::vector<char> fragmentShaderErrorMessage(infoLogLength + 1);
				glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
				printf("%s\n", &fragmentShaderErrorMessage[0]);
			}

			printf("Linking program\n");
			GLuint programID = glCreateProgram();
			glAttachShader(programID, vertexShaderID);
			glAttachShader(programID, fragmentShaderID);
			glLinkProgram(programID);

			glGetProgramiv(programID, GL_LINK_STATUS, &result);
			glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
			if (infoLogLength > 0) 
			{
				std::vector<char> programErrorMessage(infoLogLength + 1);
				glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
				printf("%s\n", &programErrorMessage[0]);
			}

			glDetachShader(programID, fragmentShaderID);
			glDetachShader(programID, vertexShaderID);
			glDeleteShader(fragmentShaderID);
			glDeleteShader(vertexShaderID);
			return programID;
		}
	};
}
