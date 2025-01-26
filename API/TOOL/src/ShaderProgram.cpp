#include "ShaderProgram.h"

void ShaderProgram::bind()
{
    glUseProgram(handle);
}

void ShaderProgram::compile(const char* vert, const char* frag)
{
	GLuint v_handle = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(v_handle, 1, &vert, NULL);
	glCompileShader(v_handle);
    check(v_handle, "VERTEX");

    GLuint f_handle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_handle, 1, &frag, NULL);
	glCompileShader(f_handle);
    check(f_handle, "FRAGMENT");

    handle = glCreateProgram();
    glAttachShader(handle, v_handle);
    glAttachShader(handle, f_handle);
    glLinkProgram(handle);
    check(handle, "PROGRAM");

    glDeleteShader(v_handle);
    glDeleteShader(f_handle);
}

void ShaderProgram::check(GLuint _handle, const char* type)
{
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(_handle, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(_handle, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(_handle, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(_handle, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
}