#include <iostream>

#include <Camera.h>
#include <ShaderProgram.h>
#include <glfw3.h>

std::string v_code = R"(
	#version 330 core

	layout (location = 0) in vec3 aPos;
	layout (location = 1) in vec3 aColor;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	out vec3 color;

	void main()
	{
		color = aColor;
		gl_Position = projection * view * model * vec4(aPos, 1.0); 
	}
)";

std::string f_code = R"(
	#version 330 core

	out vec4 FragColor;

	in vec3 color;

	void main()
	{
		FragColor = vec4(color, 1.0);
	}
)";

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "HelloTriangle", nullptr, nullptr);
	if (!window)
	{
		std::cout << "error to create glfw window" << std::endl;
		glfwTerminate();
		return -1;
	}
	std::cout <<  "srccess to create glfw window" << std::endl;

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "error to init glad" << std::endl;
		return -1;
	}
	std::cout << "srccess to init glad" << std::endl;

	Camera camera;

	ShaderProgram program;
	program.compile(v_code.c_str(), f_code.c_str());

	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
		 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glViewport(0, 0, 800, 600);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.3, 0.3, 0.1, 1);

		program.bind();
		program.SetValue("model", glm::mat4(1));
		program.SetValue("view", camera.getviewmatrix());
		program.SetValue("projection", camera.getprojmatrix(800.0f / 600.0f));

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

    return 1;
}