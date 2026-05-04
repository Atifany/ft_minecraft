#include "../include/Shader.h"

unsigned int CompileShader(const std::string shaderSource, int shaderType);
unsigned int CompileShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);

Shader::Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
{
	// Read shader files
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vertexFile;
	std::ifstream fragmentFile;

	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vertexFile.open(vertexShaderPath);
		std::stringstream vertexShaderStream;
		vertexShaderStream << vertexFile.rdbuf();
		vertexCode = vertexShaderStream.str();
		vertexFile.close();
		
		fragmentFile.open(fragmentShaderPath);
		std::stringstream fragmentShaderStream;
		fragmentShaderStream << fragmentFile.rdbuf();
		fragmentCode = fragmentShaderStream.str();
		fragmentFile.close();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "Error: Failed to read shader files.\n" << e.what() << '\n';
		this->ID = 0;
		return ;
	}

	// Compile shader program
	unsigned int vertexShader = CompileShader(vertexCode, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CompileShader(fragmentCode, GL_FRAGMENT_SHADER);
	this->ID = CompileShaderProgram(vertexShader, fragmentShader);
	if (this->ID == 0)
		glDeleteProgram(this->ID);
}


void Shader::Use()
{
	glUseProgram(this->ID);
}

void Shader::SetBool(std::string &name, bool value) const
{

}

void Shader::SetInt(std::string &name, int value) const
{

}

void Shader::SetFloat(std::string &name, float value) const
{

}

unsigned int CompileShader(const std::string shaderSource, int shaderType)
{
	unsigned int shader;
	shader = glCreateShader(shaderType);
	const char *sSource = shaderSource.c_str();
	glShaderSource(shader, 1, &sSource, NULL);
	glCompileShader(shader);

	int success = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		int infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char *message = (char*)alloca(infoLogLength * sizeof(char));
		glGetShaderInfoLog(shader, infoLogLength, &infoLogLength, message);
		if (shaderType == GL_VERTEX_SHADER)
			std::cout << "Failed to compile vertex shader:\n";
		else
				std::cout << "Failed to compile fragment shader:\n";
		std::cout << message;
		return (0);
	}
	return (shader);
}

unsigned int CompileShaderProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	if (vertexShader == 0 || fragmentShader == 0)
		return (0);
		
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int success = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		int infoLogLength;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		char *message = (char*)alloca(infoLogLength * sizeof(char));
		glGetProgramInfoLog(shaderProgram, infoLogLength, &infoLogLength, message);
		printf("Error: Failed to link shader program:\n%s", message);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return(0);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return (shaderProgram);
}

