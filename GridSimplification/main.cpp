#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

#include <glm.hpp>
#include <Tool.h>

class QEM
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<int> faces;

	std::vector<glm::ivec2> edges_adj;
	std::vector<glm::ivec2> edges;
	std::priority_queue< std::pair<double, glm::ivec2> > heap;

	void init()
	{
		edges_adj.resize(vertices.size());

		for (size_t i = 0; i < faces.size(); i += 3)
		{
			int f[3] = { faces[i] ,faces[i + 1] ,faces[i + 2] };
			std::sort(f, f + 3);

			edges_adj[f[0]] = glm::ivec2(f[1], f[2]);
			edges_adj[f[1]] = glm::ivec2(f[2], f[0]);
			edges_adj[f[2]] = glm::ivec2(f[0], f[1]);

			edges.push_back({ f[0], f[1] });
			edges.push_back({ f[1], f[2] });
			edges.push_back({ f[0], f[2] });
		}
	}

	void simplify(float ratio)
	{
	}

	void buildHeap()
	{
		while (!heap.empty()) heap.pop();
		for (const auto& e : edges)
		{
			float len = glm::length(vertices[e.x] - vertices[e.y]);

			if (len > 1e8) return;
			//auto pos = calculateQ(e);
			//heap.push(std::make_pair(-pos.second, e));
		}
	}

	void calculateQ(const glm::ivec2& e)
	{
		glm::mat4 q = glm::mat4(0);
		for (size_t i = 0; i < faces.size(); i += 3)
		{
		}
	}
};

int main()
{
	std::vector<glm::vec3> vertices;
	std::vector<int> faces;
	objread(R"(D:\user\modern-rendering-technology\models\pyramid_2.obj)", vertices, faces);

	QEM qem;
	qem.vertices = vertices;
	qem.faces = faces;

	qem.init();
}