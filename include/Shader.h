#ifndef SHADER_H
#define SHADER_H

#include "ft_minecraft.h"

class Shader
{
	public:

		Shader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
		// Activate shaders
		void Use();
		// Set uniforms
		void SetBool(std::string &name, bool value) const;
		void SetInt(std::string &name, int value) const;
		void SetFloat(std::string &name, float value) const;

		// program ID
		unsigned int ID;
		
	private:

};

#endif