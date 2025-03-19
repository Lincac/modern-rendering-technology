#include <iostream>
#include <vector>
#include <map>
#include <array>

#include <glm.hpp>
#include <Tool.h>

struct Edge
{
	int v1, v2;
	Edge(int a, int b) : v1(glm::min(a, b)), v2(glm::max(a, b)) {}

	bool operator < (const Edge& edge) const
	{
		return v1 < edge.v1 || (v1 == edge.v1 && v2 < edge.v2);
	}
};

class Loop
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<int> faces;

	void subdivide(std::vector<glm::vec3>& newVertices, std::vector<int>& newFaces)
	{
		std::map<Edge, int> edgeMap;

		newVertices.clear();
		newFaces.clear();

		newVertices = vertices;

		std::vector<glm::vec3> midpoints(vertices.size(), glm::vec3(0.0f));
		std::vector<int> adjacentCount(vertices.size(), 0);

		for (size_t i = 0; i < faces.size(); i += 3)
		{
			int v1 = faces[i];
			int v2 = faces[i + 1];
			int v3 = faces[i + 2];

			// 为每条边计算中点
			int a = calculateMidpoint(v1, v2, edgeMap, newVertices);
			int b = calculateMidpoint(v2, v3, edgeMap, newVertices);
			int c = calculateMidpoint(v3, v1, edgeMap, newVertices);

			midpoints[v1] += vertices[v2] + vertices[v3];
			midpoints[v2] += vertices[v1] + vertices[v3];
			midpoints[v3] += vertices[v1] + vertices[v2];

			adjacentCount[v1] += 2;
			adjacentCount[v2] += 2;
			adjacentCount[v3] += 2;

			// 生成新的面
			newFaces.push_back(v1);
			newFaces.push_back(a);
			newFaces.push_back(c);

			newFaces.push_back(v2);
			newFaces.push_back(b);
			newFaces.push_back(a);

			newFaces.push_back(v3);
			newFaces.push_back(c);
			newFaces.push_back(b);

			newFaces.push_back(a);
			newFaces.push_back(b);
			newFaces.push_back(c);
		}

		for (size_t i = 0; i < vertices.size(); ++i)
		{
			if (adjacentCount[i] > 0)
			{
				float beta = (adjacentCount[i] == 3) ? 3.0f / 16.0f : 3.0f / (8.0f * adjacentCount[i]);
				newVertices[i] = (1 - adjacentCount[i] * beta) * vertices[i] + beta * midpoints[i] / (float)adjacentCount[i];
			}
		}
	}

	int calculateMidpoint(int v1, int v2, std::map<Edge, int>& edgeMap, std::vector<glm::vec3>& newVertices)
	{
		Edge edge(v1, v2);

		if (edgeMap.find(edge) != edgeMap.end())
		{
			return edgeMap[edge];
		}

		glm::vec3 midpoint = 0.5f * (vertices[v1] + vertices[v2]);

		int newIndex = newVertices.size();
		newVertices.push_back(midpoint);

		edgeMap[edge] = newIndex;
		return newIndex;
	}
};

int main()
{
	std::vector<glm::vec3> vertices;
	std::vector<int> faces;
	objread(R"(D:\user\modern-rendering-technology\models\cube.obj)", vertices, faces);

	Loop loop;
	loop.vertices = vertices;
	loop.faces = faces;

	std::vector<glm::vec3> newVertices;
	std::vector<int> newFaces;

	loop.subdivide(newVertices, newFaces);

	objwrite(R"(D:\user\modern-rendering-technology\models\cube_1.obj)", newVertices, newFaces);
}