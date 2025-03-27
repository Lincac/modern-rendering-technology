/** @Lincian
 * 该OIT方式是使用深度剥离算法实现
 */

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
#include <Tool.h>

Camera camera;

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

GLuint quadVAO = 0;
GLuint quadVBO;
void RenderQuad()
{
	if (quadVAO == 0)
	{
		GLfloat quadVertices[] = {
			-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "OIT", nullptr, nullptr);
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

	ShaderProgram program_1;
	program_1.compile(R"(
		#version 460 core
		layout(location = 0) in vec3 aPos;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		out VS_OUT
		{
			vec3 FragPosViewSapce;
		} vs_out;

		void main()
		{
			vs_out.FragPosViewSapce = vec3(view * model * vec4(aPos, 1.0));
			gl_Position = projection * vec4(vs_out.FragPosViewSapce, 1.0);
		}
	)", R"(
		#version 460 core
		out vec4 FragColor;

		in VS_OUT
		{
			vec3 FragPosViewSapce;
		} fs_in;

		uniform vec3 color;

		void main()
		{
			vec3 vertexVS = fs_in.FragPosViewSapce;

			vec3 fdx = dFdx(vertexVS);
			vec3 fdy = dFdy(vertexVS);

			vec3 normalVS = normalize(cross(fdx, fdy));
			if(normalVS.z < 0) { normalVS.z = -1.0 * normalVS.z; }

			float df = max(0.00001, normalVS.z);

			FragColor = vec4(color * df, 0.7);
		}
	)");

	ShaderProgram program;
	program.compile(R"(
		#version 460 core
		layout(location = 0) in vec3 aPos;

		uniform mat4 model;
		uniform mat4 view;
		uniform mat4 projection;

		out VS_OUT
		{
			vec3 FragPosViewSapce;
		} vs_out;

		void main()
		{
			vs_out.FragPosViewSapce = vec3(view * model * vec4(aPos, 1.0));
			gl_Position = projection * vec4(vs_out.FragPosViewSapce, 1.0);
		}
	)", R"(
		#version 460 core
		out vec4 FragColor;

		in VS_OUT
		{
			vec3 FragPosViewSapce;
		} fs_in;

		uniform vec3 color;

		uniform sampler2D depthMap;

		void main()
		{
			vec2 texCoords = gl_FragCoord.xy / vec2(textureSize(depthMap, 0));
			float depth = texture(depthMap, texCoords).r;
			if(gl_FragCoord.z < depth + 0.001) discard;

			vec3 vertexVS = fs_in.FragPosViewSapce;

			vec3 fdx = dFdx(vertexVS);
			vec3 fdy = dFdy(vertexVS);

			vec3 normalVS = normalize(cross(fdx, fdy));
			if(normalVS.z < 0) { normalVS.z = -1.0 * normalVS.z; }

			float df = max(0.00001, normalVS.z);

			FragColor = vec4(color * df, 0.7);
		}
	)");

	ShaderProgram blend;
	blend.compile(R"(
		#version 460 core
		layout (location = 0) in vec4 aPosition;
		layout (location = 1) in vec2 aTexCoord;

		out vec2 TexCoord;

		void main()
		{
			TexCoord = aTexCoord;
			gl_Position = vec4(aPosition.xy, 0.0, 1.0);
		}

	)", R"(
		#version 460 core
		out vec4 FragColor;
		in vec2 TexCoord;

		uniform sampler2DArray colorArray;

		void main()
		{
			vec3 uv = vec3(TexCoord, 5);
			vec3 color = texture(colorArray, uv).rgb;
			for(int i=4;i>=0;i--)
			{
				uv = vec3(TexCoord, i);
				vec4 tex = texture(colorArray, uv);

				color = tex.rgb * tex.a + color * (1.0 - tex.a);
			}

			FragColor = vec4(color, 1);
		}

	)");

	std::vector<glm::vec3> vertex;
	std::vector<int> face;
	objread(R"(D:\user\modern-rendering-technology\models\dragon.obj)", vertex, face);

	unsigned obj_vao, obj_ebo, obj_vbo;
	glGenVertexArrays(1, &obj_vao);
	glBindVertexArray(obj_vao);

	glGenBuffers(1, &obj_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, obj_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex.size(), vertex.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &obj_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * face.size(), face.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

#pragma region PingPong
	unsigned int FBO[3];
	glGenFramebuffers(3, FBO);

	unsigned int color[3];
	glGenTextures(3, color);

	unsigned int depth[3];
	glGenTextures(3, depth);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);

	glBindTexture(GL_TEXTURE_2D, color[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color[0], 0);

	glBindTexture(GL_TEXTURE_2D, depth[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, FBO[1]);

	glBindTexture(GL_TEXTURE_2D, color[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color[1], 0);

	glBindTexture(GL_TEXTURE_2D, depth[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth[1], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

#pragma endregion

	unsigned texture2DArray;
	glGenTextures(1, &texture2DArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture2DArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, 800, 600, 6, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window, camera);

		auto view = camera.getviewmatrix();
		auto projection = camera.getprojmatrix(800.0f / 600.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, FBO[0]);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		program_1.bind();
		program_1.SetValue("projection", projection);
		program_1.SetValue("view", view);

		glm::mat4 model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(1.8));
		program_1.SetValue("model", model);
		program_1.SetValue("color", glm::vec3(252, 227, 205) / glm::vec3(255));

		glBindVertexArray(obj_vao);
		glDrawElements(GL_TRIANGLES, face.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glBindTexture(GL_TEXTURE_2D_ARRAY, texture2DArray);
		glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 0, 0, 800, 600);
		glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

		for (size_t i = 1; i < 6; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, FBO[i % 2]);
			glClearColor(0, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			program.bind();
			program.SetValue("projection", projection);
			program.SetValue("view", view);

			glm::mat4 model = glm::mat4(1);
			model = glm::scale(model, glm::vec3(1.8));
			program.SetValue("model", model);
			program.SetValue("color", glm::vec3(252, 227, 205) / glm::vec3(255));

			program.SetValue("near_plane", camera.near_clip);
			program.SetValue("far_plane", camera.far_clip);

			program.SetValue("depthMap", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, depth[(i - 1) % 2]);

			glBindVertexArray(obj_vao);
			glDrawElements(GL_TRIANGLES, face.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glBindTexture(GL_TEXTURE_2D_ARRAY, texture2DArray);
			glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 0, 0, 800, 600);
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		blend.bind();
		blend.SetValue("colorArray", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture2DArray);
		RenderQuad();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 1;
}