#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include <glm.hpp>

/** @Lincian
 * Ŀǰ�ýӿ�ֻ�ܽ��� �������� v �� ������ f ��ģ���ļ�
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

		file.close();
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

		file.close();
	}
	else
	{
		std::cout << "Error : can not read path : " << path << std::endl;
	}
}

template<typename T>
void objwrite(const std::string& path, const std::vector<T>& vertex, const std::vector<int>& face)
{
}

template<>
void objwrite(const std::string& path, const std::vector<float>& vertex, const std::vector<int>& face)
{
	std::ofstream file(path);

	if (file.is_open())
	{
		for (size_t i = 0; i < vertex.size(); i += 3)
		{
			file << "v " << vertex[i] << ' ' << vertex[i + 1] << ' ' << vertex[i + 2] << std::endl;
		}

		for (size_t i = 0; i < face.size(); i += 3)
		{
			file << "f " << face[i] + 1 << ' ' << face[i + 1] + 1 << ' ' << face[i + 2] + 1 << std::endl;
		}

		file.close();
	}
	else
	{
		std::cout << "Error : can not read path : " << path << std::endl;
	}
}

template<>
void objwrite(const std::string& path, const std::vector<glm::vec3>& vertex, const std::vector<int>& face)
{
	std::ofstream file(path);

	if (file.is_open())
	{
		for (size_t i = 0; i < vertex.size(); i++)
		{
			file << "v " << vertex[i].x << ' ' << vertex[i].y << ' ' << vertex[i].z << std::endl;
		}

		for (size_t i = 0; i < face.size(); i += 3)
		{
			file << "f " << face[i] + 1 << ' ' << face[i + 1] + 1 << ' ' << face[i + 2] + 1 << std::endl;
		}

		file.close();
	}
	else
	{
		std::cout << "Error : can not read path : " << path << std::endl;
	}
}