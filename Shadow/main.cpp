#include <iostream>

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>
#include <ShaderProgram.h>
#include <Camera.h>

Camera camera;

void RenderCube();
void RenderQuad();

bool mouseLeftPressed = false;
bool mouseRightPressed = false;
double lastX = 0.0, lastY = 0.0;
float deltaX = 0.0, deltaY = 0.0;
double scrollOffsetX = 0.0, scrollOffsetY = 0.0;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		mouseLeftPressed = true;
		glfwGetCursorPos(window, &lastX, nullptr);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		mouseLeftPressed = false;
		deltaX = 0.0;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		mouseRightPressed = true;
		glfwGetCursorPos(window, nullptr, &lastY);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
	{
		mouseRightPressed = false;
		deltaY = 0.0;
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (mouseLeftPressed)
	{
		deltaX = lastX - xpos;

		lastX = xpos;
	}

	if (mouseRightPressed)
	{
		deltaY = ypos - lastY;

		lastY = ypos;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollOffsetX = xoffset;
	scrollOffsetY = yoffset;
}

void processInput(GLFWwindow* window, Camera& camera)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		float distance = glm::distance(camera.position, camera.focal);

		glm::vec3 front = glm::normalize(camera.focal - camera.position);
		glm::vec3 right = glm::normalize(glm::cross(front, camera.up));
		glm::vec3 currentUp = glm::normalize(glm::cross(right, front));

		glm::quat currentQuat = glm::quatLookAt(front, camera.up);

		//glm::quat pitchQuat = glm::angleAxis(glm::radians(deltaY), right);
		glm::quat yawQuat = glm::angleAxis(glm::radians(-deltaX), currentUp);

		//glm::quat deltaQuat = yawQuat * pitchQuat;
		glm::quat deltaQuat = yawQuat;
		glm::quat newQuat = deltaQuat * currentQuat;

		glm::vec3 newFront = glm::rotate(newQuat, glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 newUp = glm::rotate(newQuat, glm::vec3(0.0f, 1.0f, 0.0f));

		camera.position = camera.focal - newFront * distance;
		camera.up = newUp;
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		float distance = glm::distance(camera.position, camera.focal);

		glm::vec3 front = glm::normalize(camera.focal - camera.position);
		glm::vec3 right = glm::normalize(glm::cross(front, camera.up));
		glm::vec3 currentUp = glm::normalize(glm::cross(right, front));

		glm::quat currentQuat = glm::quatLookAt(front, camera.up);

		glm::quat pitchQuat = glm::angleAxis(glm::radians(deltaY), right);
		//glm::quat yawQuat = glm::angleAxis(glm::radians(-deltaX), currentUp);

		//glm::quat deltaQuat = yawQuat * pitchQuat;
		glm::quat deltaQuat = pitchQuat;
		glm::quat newQuat = deltaQuat * currentQuat;

		glm::vec3 newFront = glm::rotate(newQuat, glm::vec3(0.0f, 0.0f, -1.0f));
		glm::vec3 newUp = glm::rotate(newQuat, glm::vec3(0.0f, 1.0f, 0.0f));

		camera.position = camera.focal - newFront * distance;
		camera.up = newUp;
	}

	glm::vec3 front = glm::normalize(camera.position - camera.focal);

	if (scrollOffsetY > 0)
	{
		camera.position -= front * 0.05f;
		scrollOffsetY = 0.0;
	}
	else if (scrollOffsetY < 0)
	{
		camera.position += front * 0.05f;
		scrollOffsetY = 0.0;
	}
}

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "error to init glad!" << std::endl;
		return -1;
	}
	std::cout << "success to init glad!" << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

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
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

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

	ShaderProgram depthrogram;
	depthrogram.compile(depthVC.c_str(), R"(
		#version 460 core
		void main()
		{
		}
	)");

	glm::vec3 lightPos = glm::vec3(3.0f, 2.0f, 3.0f);

	glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 40.0f);
	glm::mat4 lightsapceMatrix = lightProjection * lightView;

	ShaderProgram shadow;
	shadow.compile(R"(
		#version 460 core
		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec3 aNorm;
		layout(location = 2) in vec2 aTexCoord;

		uniform mat4 lightSpaceMatrix;
		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		out VS_OUT
		{
			vec3 FragPos;
			vec3 Normal;
			vec4 FragPosLightSpace;
		} vs_out;

		void main()
		{
			vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
			vs_out.Normal = transpose(inverse(mat3(model))) * aNorm;
			vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
			gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
		}
	)", R"(
		#version 460 core
		out vec4 FragColor;

		in VS_OUT
		{
			vec3 FragPos;
			vec3 Normal;
			vec4 FragPosLightSpace;
		} fs_in;

		uniform sampler2D depthMap;
		uniform vec3 CC;
		uniform vec3 lightPos;

		float ShadowCalculation(vec4 fragPosLightSpace)
		{
			vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
			projCoords = projCoords * 0.5 + 0.5;

			float closestDepth = texture(depthMap, projCoords.xy).r;
			float currentDepth = projCoords.z;

			vec3 normal = normalize(fs_in.Normal);
			vec3 lightDir = normalize(lightPos - fs_in.FragPos);

			float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
			float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

			return 1.0 - shadow;
		}

		void main()
		{
			vec3 normal = normalize(fs_in.Normal);

			vec3 color = CC;

			vec3 lightDir = normalize(lightPos - fs_in.FragPos);
			float diff = max(dot(lightDir, normal), 0.0);

			color *= diff;

			float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
			color *= shadow;

			FragColor = vec4(color, 1.0);
		}
	)");

	while (!glfwWindowShouldClose(window))
	{
		processInput(window, camera);
		auto view = camera.getviewmatrix();
		auto projection = camera.getprojmatrix(800.0f / 600.0f);

		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClear(GL_DEPTH_BUFFER_BIT);

		//glEnable(GL_CULL_FACE);
		//glCullFace(GL_FRONT);

		glm::mat4 model = glm::mat4(1);
		depthrogram.bind();
		depthrogram.SetValue("lightSpaceMatrix", lightsapceMatrix);
		depthrogram.SetValue("model", model);
		RenderCube();

		//glDisable(GL_CULL_FACE);

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, -0.5f));
		model = glm::scale(model, glm::vec3(3, 3, 1));
		depthrogram.SetValue("lightSpaceMatrix", lightsapceMatrix);
		depthrogram.SetValue("model", model);
		RenderQuad();

		glViewport(0, 0, 800, 600);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shadow.bind();
		shadow.SetValue("projection", projection);
		shadow.SetValue("view", view);
		shadow.SetValue("lightPos", lightPos);
		shadow.SetValue("lightSpaceMatrix", lightsapceMatrix);
		shadow.SetValue("depthMap", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		model = glm::mat4(1);
		shadow.SetValue("model", model);
		shadow.SetValue("CC", glm::vec3(240, 190, 77) / 255.0f);
		RenderCube();

		model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(0, 0, -0.5f));
		model = glm::scale(model, glm::vec3(3, 3, 1));
		shadow.SetValue("model", model);
		shadow.SetValue("CC", glm::vec3(1, 0, 0));
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
		   -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // Bottom-left
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,  // top-right
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,  // bottom-left
			-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,// top-left
			// Front face
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
			-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // top-left
			-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom-left
			// Left face
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-left
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom-right
			-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
			// Right face
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // bottom-right
			0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,  // top-left
			0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
			// Bottom face
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,// bottom-left
			0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
			-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
			// Top face
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
			0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
			-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,// top-left
			-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
		};

		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);

		glGenBuffers(1, &cubeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

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
			 1.0f, 1.0f, 0.0f,	 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,	 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,	 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

			 1.0f, 1.0f, 0.0f,	 0.0f, 0.0f, 1.0f,	1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,	 0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		};

		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}