#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <glm.hpp>

/** @Lincian
 * 目前该接口只能接受 顶点属性 v 和 面属性 f 的模型文件
 */
template<typename T>
void objread(const std::string& path, std::vector<T>& vertex, std::vector<int>& face)
{
}

template<>
void objread(const std::string& path, std::vector<float>& vertex, std::vector<int>& face)
{
	std::ifstream file(path);
	std::string line;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			std::istringstream iss(line);

			char type;
			iss >> type;

			if (type == 'v')
			{
				float v1, v2, v3;

				iss >> v1 >> v2 >> v3;

				vertex.push_back(v1);
				vertex.push_back(v2);
				vertex.push_back(v3);
			}
			else if (type == 'f')
			{
				int f1, f2, f3;

				iss >> f1 >> f2 >> f3;

				face.push_back(f1 - 1);
				face.push_back(f2 - 1);
				face.push_back(f3 - 1);
			}
		}
	}
	else
	{
		std::cout << "Error : can not read path : " << path << std::endl;
	}
}

template<>
void objread(const std::string& path, std::vector<glm::vec3>& vertex, std::vector<int>& face)
{
	std::ifstream file(path);
	std::string line;

	if (file.is_open())
	{
		while (std::getline(file, line))
		{
			std::istringstream iss(line);

			char type;
			iss >> type;

			if (type == 'v')
			{
				float v1, v2, v3;

				iss >> v1 >> v2 >> v3;

				vertex.push_back({ v1, v2, v3 });
			}
			else if (type == 'f')
			{
				int f1, f2, f3;

				iss >> f1 >> f2 >> f3;

				face.push_back(f1 - 1);
				face.push_back(f2 - 1);
				face.push_back(f3 - 1);
			}
		}
	}
	else
	{
		std::cout << "Error : can not read path : " << path << std::endl;
	}
}
