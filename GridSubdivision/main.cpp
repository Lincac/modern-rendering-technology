#include <iostream>
#include <vector>
#include <map>
#include <array>

#include <glm.hpp>
#include <gtc/constants.hpp>
#include <Tool.h>

class Loop
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<int> faces;

	std::vector<std::vector<std::pair<int, glm::ivec3>>> edge_vertexes;
	/* @Lincian
	 * edge_vertexes[i] ：表示与顶点 i 相关的所有边的信息。
	 * pair.first (int) ： 表示与顶点 i 相邻的另一个顶点的索引 ID（即边的另一个端点）。
	 * pair.second (glm::ivec3)： 用于存储与该边相关的细分数据，具体包括：x：沿这条边新生成顶点的索引 ID，
	 *															   y：与该边相关联的三角形中另一个顶点的索引 ID（帮助细分计算），
	 *															   z：一个标志位，用于标记是否是边界边（z == -1 表示边界情况）。
	 */

	void subdivide(std::vector<glm::vec3>& newVertices, std::vector<int>& newFaces)
	{
		edge_vertexes.clear();
		edge_vertexes.resize(vertices.size());

		for (size_t i = 0; i < faces.size(); i += 3)
		{
			int vertex3_id = add_edge_vertexes(faces[i], faces[i + 1], faces[i + 2]);
			int vertex4_id = add_edge_vertexes(faces[i + 1], faces[i + 2], faces[i]);
			int vertex5_id = add_edge_vertexes(faces[i], faces[i + 2], faces[i + 1]);

			newFaces.push_back(faces[i]); newFaces.push_back(vertex3_id); newFaces.push_back(vertex5_id);
			newFaces.push_back(vertex3_id); newFaces.push_back(faces[i + 1]); newFaces.push_back(vertex4_id);
			newFaces.push_back(vertex5_id); newFaces.push_back(vertex4_id); newFaces.push_back(faces[i + 2]);
			newFaces.push_back(vertex5_id); newFaces.push_back(vertex3_id); newFaces.push_back(vertex4_id);
		}

		for (int i = 0; i < vertices.size(); ++i) {
			for (int j = 0; j < edge_vertexes[i].size(); ++j) {
				int vertex_id = edge_vertexes[i][j].first;
				int new_vertex0_id = edge_vertexes[i][j].second.x;
				int new_vertex1_id = edge_vertexes[i][j].second.y;
				int new_vertex2_id = edge_vertexes[i][j].second.z;
				if (new_vertex2_id == -1)
				{
					vertices[new_vertex0_id] = (vertices[i] + vertices[vertex_id]) / 2.0f;
				}
				else
				{
					vertices[new_vertex0_id] =
						(vertices[i] + vertices[vertex_id]) * 3.0f / 8.0f +
						(vertices[new_vertex1_id] + vertices[new_vertex2_id]) / 8.0f;
				}
			}
		}

		for (int i = 0; i < vertices.size(); ++i) {
			for (int j = 0; j < edge_vertexes[i].size(); ++j) {
				if (edge_vertexes[i][j].first > i) {
					edge_vertexes[edge_vertexes[i][j].first].push_back(
						std::make_pair(i, edge_vertexes[i][j].second));
				}
			}
		}

		std::vector<glm::vec3> temp_positions;
		for (int i = 0; i < vertices.size(); ++i) {
			int k = edge_vertexes[i].size();
			std::vector<int> adj_boundary;
			glm::vec3 v3f;
			for (int j = 0; j < k; ++j) {
				int vertex_id = edge_vertexes[i][j].first;
				if (edge_vertexes[i][j].second.z == -1) {
					adj_boundary.push_back(vertex_id);
				}
				v3f = v3f + vertices[vertex_id];
			}
			if (adj_boundary.size() == 2) {
				temp_positions.push_back(vertices[i] * 3.0f / 4.0f +
					(vertices[adj_boundary[0]] + vertices[adj_boundary[1]]) / 8.0f);
			}
			else {
				float beta = (5.0 / 8 - glm::sqrt(3.0 / 8 + cos(2 * glm::pi<float>() / k) / 4)) / k;
				temp_positions.push_back(vertices[i] * (1 - k * beta) + v3f * beta);
			}
		}
		for (int i = 0; i < vertices.size(); ++i) {
			vertices[i] = temp_positions[i];
		}

		newVertices = vertices;
	}

	int add_edge_vertexes(int vertex0_id, int vertex1_id, int vertex2_id) {
		int ret = -1;
		if (vertex0_id > vertex1_id)
		{
			std::swap(vertex0_id, vertex1_id);
		}

		int index = -1;
		for (int i = 0; i < edge_vertexes[vertex0_id].size(); ++i) {
			if (edge_vertexes[vertex0_id][i].first == vertex1_id) {
				index = i;
				break;
			}
		}
		if (index == -1) {
			ret = vertices.size();
			edge_vertexes[vertex0_id].push_back(std::make_pair(vertex1_id, glm::ivec3(ret, vertex2_id, -1)));
			vertices.push_back(glm::vec3(0));
		}
		else {
			ret = edge_vertexes[vertex0_id][index].second.x;
			edge_vertexes[vertex0_id][index].second.z = vertex2_id;
		}
		return ret;
	}
};

int main()
{
	std::vector<glm::vec3> vertices;
	std::vector<int> faces;
	objread(R"(D:\user\modern-rendering-technology\models\pyramid.obj)", vertices, faces);

	Loop loop;
	loop.vertices = vertices;
	loop.faces = faces;

	std::vector<glm::vec3> newVertices;
	std::vector<int> newFaces;

	loop.subdivide(newVertices, newFaces);

	objwrite(R"(D:\user\modern-rendering-technology\models\pyramid_1.obj)", newVertices, newFaces);
}