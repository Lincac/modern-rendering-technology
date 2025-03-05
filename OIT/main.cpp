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

int main()
{
	std::vector<glm::vec3> vertex;
	std::vector<int> face;

	objread("../../models/cube.obj", vertex, face);
}