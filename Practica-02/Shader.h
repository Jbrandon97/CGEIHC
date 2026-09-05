#pragma once

#include <string>
#include <fstream>
#include <glew.h>

class Shader
{
public:
	Shader();

	void CreatefromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);

	std::string ReadFile(const char* fileLocation);

	GLuint getProjectLocation();
	GLuint getModelLocation();
	//Ir agregando nuevas variables Uniform

	void useShader();
	void ClearShader();

	~Shader();

private:
	GLuint shaderID, uniformModel, uniformProjection;
	//Ir agregando nuevas variables Uniform

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
};