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

		for (size_t i = 0; i < faces.size(); i += 3)
		{
			for (size_t j = 0; j < 3; j++)
			{
				int v1 = faces[i + j];
				int v2 = faces[i + (j + 1) % 3];

				Edge edge(v1, v2);
				if (edgeMap.find(edge) == edgeMap.end())
				{
					int v3 = faces[i + (j + 2) % 3];

					glm::vec3 mid = 0.375f * (vertices[v1] + vertices[v2]) +
						0.125f * (vertices[v3] + vertices[faces[(i + (j + 3 - 1) % 3)]]);

					edgeMap[edge] = newVertices.size();
					newVertices.push_back(mid);
				}
			}
		}

		for (size_t i = 0; i < faces.size(); i += 3)
		{
			int v1 = faces[i], v2 = faces[i + 1], v3 = faces[i + 2];
			int a = edgeMap[Edge(v1, v2)];
			int b = edgeMap[Edge(v2, v3)];
			int c = edgeMap[Edge(v3, v1)];

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