#include <iostream>

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <ShaderProgram.h>
#include <Camera.h>

Camera camera;

void RenderCube();
void RenderQuad();

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Shadow", nullptr, nullptr);
	if (!window)
	{
		std::cout << "error to create glfw window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	std::cout << "success to create glfw window!" << std::endl;

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "error to init glad!" << std::endl;
		return -1;
	}
	std::cout << "success to init glad!" << std::endl;

	glEnable(GL_DEPTH_TEST);

	unsigned int fbo;
	glGenFramebuffers(1, &fbo);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0,
		GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::string depthVC = R"(
		#version 460 core
		layout(location = 0) in vec3 aPos;
		uniform mat4 lightSpaceMatrix;
		uniform mat4 model;
		void main()
		{
			gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
		}
	)";

	ShaderProgram shadowProgram;
	shadowProgram.compile(depthVC.c_str(), R"(
		#version 460 core
		void main()
		{
		}
	)");

	glm::vec3 lightPos = glm::vec3(3.0f, 3.0f, 0.0f);

	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 10.0f);
	glm::mat4 lightsapceMatrix = lightProjection * lightView;

	ShaderProgram test;
	test.compile(R"(
		#version 460 core
		layout(location = 0) in vec3 aPos;
		void main()
		{
			gl_Position = vec4(aPos, 1.0);
		}
	)", R"(
		#version 460 core
		out vec4 FragColor;
		uniform sampler2D depthMap;
		void main()
		{
			vec2 uv = gl_FragCoord.xy / vec2(800, 600);
			float depthValue = texture(depthMap, uv).r;
			FragColor = vec4(vec3(depthValue), 1.0);
		}
	)");

	while (!glfwWindowShouldClose(window))
	{
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1);
		shadowProgram.bind();
		shadowProgram.SetValue("lightSpaceMatrix", lightsapceMatrix);
		shadowProgram.SetValue("model", model);
		RenderCube();
		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, -1));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f));
		shadowProgram.SetValue("lightSpaceMatrix", lightsapceMatrix);
		shadowProgram.SetValue("model", model);
		RenderQuad();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		test.bind();
		test.SetValue("depthMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		RenderQuad();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 1;
}

unsigned int cubeVAO = -1, cubeVBO;
void RenderCube()
{
	if (cubeVAO == -1)
	{
		float vertices[] = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f
		};

		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);

		glGenBuffers(1, &cubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int quadVAO = -1, quadVBO;
void RenderQuad()
{
	if (quadVAO == -1)
	{
		float vertices[] = {
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,

			-1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
		};

		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}