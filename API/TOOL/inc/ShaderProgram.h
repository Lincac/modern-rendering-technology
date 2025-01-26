#pragma once

#include <iostream>

#include "glad.h"
#include "glm.hpp"

class ShaderProgram
{
public:

	ShaderProgram() {};

    void bind();

    void compile(const char* vert, const char* frag);

    template<typename T>
	void SetValue(const char* name, const T& value);

private:

    void check(GLuint _handle, const char* type);

    GLuint handle;

};

template<typename T>
inline void ShaderProgram::SetValue(const char*, const T&)
{
}

template<>
inline void ShaderProgram::SetValue<int>(const char* name, const int& value)
{
	glUniform1i(glGetUniformLocation(handle, name), value);
}

template<>
inline void ShaderProgram::SetValue<float>(const char* name, const float& value)
{
	glUniform1f(glGetUniformLocation(handle, name), value);
}

template<>
inline void ShaderProgram::SetValue<glm::vec2>(const char* name, const glm::vec2& value)
{
	glUniform2fv(glGetUniformLocation(handle, name), 1, &value[0]);
}

template<>
inline void ShaderProgram::SetValue<glm::vec3>(const char* name, const glm::vec3& value)
{
	glUniform3fv(glGetUniformLocation(handle, name), 1, &value[0]);
}

template<>
inline void ShaderProgram::SetValue<glm::mat4>(const char* name, const glm::mat4& value)
{
	glUniformMatrix4fv(glGetUniformLocation(handle, name), 1, GL_FALSE, &value[0][0]);
}