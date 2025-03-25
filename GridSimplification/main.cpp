#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>

#include <glm.hpp>
#include <Tool.h>

struct CompareIvec2
{
	bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
		return (a.x < b.x) || (a.x == b.x && a.y < b.y); // 按 (x, y) 排序
	}
};

struct CompareIvec2_P
{
	bool operator()(const std::pair<double, glm::ivec2>& a, const std::pair<double, glm::ivec2>& b) const {
		return a.first < b.first; // 按浮点数优先级升序
	}
};

class QEM
{
public:
	std::vector<glm::vec3> vertices;
	std::vector<int> indexes;

	std::vector<std::set<glm::ivec2, CompareIvec2>> faces;
	std::set<glm::ivec2, CompareIvec2> edges;
	std::vector<bool> removed;
	std::priority_queue<std::pair<double, glm::ivec2>, std::vector<std::pair<double, glm::ivec2>>, CompareIvec2_P> heap;

	size_t faceN;

	void init()
	{
		removed.resize(vertices.size(), false);
		faces.resize(vertices.size());
		faceN = indexes.size() / 3;

		for (size_t i = 0; i < indexes.size(); i += 3)
		{
			int f[3] = { indexes[i] ,indexes[i + 1] ,indexes[i + 2] };
			std::sort(f, f + 3);

			faces[f[0]].insert({ f[1], f[2] });
			faces[f[1]].insert({ f[2], f[0] });
			faces[f[2]].insert({ f[0], f[1] });

			edges.insert({ f[0], f[1] });
			edges.insert({ f[1], f[2] });
			edges.insert({ f[0], f[2] });
		}
	}

	void simplify(std::vector<glm::vec3>& newVertices, std::vector<int>& newFaces, float ratio)
	{
		buildHeap();

		size_t target = ratio * faceN;

		while (faceN > target)
		{
			auto e = selectEdge(1e8);
			if (e.first != glm::ivec2(-1))
			{
				removeEdge(e.first, e.second, 1e8);
			}
			else
			{
				return;
			}
		}

		std::vector<int> vertexID(vertices.size(), 0);
		int vertexReal = 0;

		for (int i = 0; i < vertices.size(); i++)
		{
			if (removed[i]) continue;
			vertexID[i] = ++vertexReal;
			newVertices.push_back(vertices[i]);
		}

		for (int i = 0; i < vertices.size(); i++) {
			if (removed[i]) continue;
			for (const auto& f : faces[i]) {
				if (i < f.x && i < f.y) {
					newFaces.push_back(vertexID[i]);
					newFaces.push_back(vertexID[f.x]);
					newFaces.push_back(vertexID[f.y]);
				}
			}
		}
	}

	void buildHeap()
	{
		while (!heap.empty()) heap.pop();
		for (const auto& e : edges)
		{
			float len = glm::length(vertices[e.x] - vertices[e.y]);

			if (len > 1e8) return;
			auto pos = calculateQ(e);
			heap.push(std::make_pair(-pos.second, e));
		}
	}

	std::pair<glm::vec3, double> calculateQ(const glm::ivec2& e)
	{
		glm::mat4 q = glm::mat4(0);

		for (const auto& f : faces[e.x])
		{
			auto n = glm::normalize(glm::cross(vertices[f.x] - vertices[e.x], vertices[f.y] - vertices[e.x]));
			auto plane = glm::vec4(n, -glm::dot(vertices[e.x], n));
			outerProductFast(plane, plane, q);
		}

		for (const auto& f : faces[e.y])
		{
			auto n = glm::normalize(glm::cross(vertices[f.x] - vertices[e.x], vertices[f.y] - vertices[e.x]));
			auto plane = glm::vec4(n, -glm::dot(vertices[e.x], n));
			outerProductFast(plane, plane, q);
		}

		glm::vec3 v(0);
		try {
			// 尝试从矩阵 q 中求解最优位置
			glm::mat4 qReduced = q;
			qReduced[3] = glm::vec4(0, 0, 0, 1); // 确保方程可求解
			if (glm::determinant(qReduced) > 1e-10) {
				glm::vec4 solution = glm::inverse(qReduced) * glm::vec4(0, 0, 0, 1);
				v = glm::vec3(solution); // 取前三个分量
			}
			else {
				throw std::runtime_error("Singular matrix");
			}
		}
		catch (...) {
			v = (vertices[e.x] + vertices[e.y]) / 2.0f;
		}
		if (glm::length(v - vertices[e.x]) + glm::length(v - vertices[e.y]) > 2.0 * glm::length(vertices[e.x] - vertices[e.y]))
		{
			v = (vertices[e.x] + vertices[e.y]) / 2.0f;
		}

		glm::vec4 extendedV(v, 1.0f);
		double cost = glm::dot(extendedV, q * extendedV);

		return std::make_pair(v, cost);
	}

	std::pair<glm::ivec2, glm::vec3> selectEdge(double threshold) {
		auto idx = glm::ivec2(-1);
		glm::vec3 pos(0);

		std::pair<double, glm::ivec2> tmp;
		while (!heap.empty()) {
			tmp = heap.top();
			heap.pop();
			if (std::find(edges.begin(), edges.end(), tmp.second) != edges.end()) continue;
			if (removed[tmp.second.x] || removed[tmp.second.y]) continue;

			float len = glm::length(vertices[tmp.second.x] - vertices[tmp.second.y]);
			if (len > threshold) continue;

			auto act = calculateQ(tmp.second);
			if (fabs(act.second + tmp.first) > 1e-8) continue;
			idx = tmp.second;
			pos = act.first;
			break;
		}

		return std::make_pair(idx, pos);
	}

	void removeEdge(const glm::ivec2& e, const glm::vec3& v, double threshold) {
		std::vector<glm::ivec2> toRev;

		for (const auto& f : faces[e.x])
		{
			if (f.x == e.y || f.y == e.y) continue;
			auto reverse = faceReverse(f, vertices[e.x], v);
			if (!reverse) continue;
			toRev.push_back(f);

			faces[f.y].erase({ e.x, f.x });
			faces[f.y].insert({ f.x, e.x });

			faces[f.x].erase({ f.y, e.x });
			faces[f.x].insert({ e.x, f.y });
		}

		for (const auto& f : toRev)
		{
			faces[e.x].erase(f);
			faces[e.x].insert({ f.y, f.x });
		}

		for (const auto& f : faces[e.y]) {
			faces[f.y].erase({ e.y, f.x });
			auto reverse = faceReverse(f, vertices[e.y], v);
			if (f.x != e.x && f.y != e.x) {
				if (reverse) {
					faces[f.y].insert({ f.x, e.x });
				}
				else {
					faces[f.y].insert({ e.x, f.x });
				}
			}

			faces[f.x].erase({ f.y, e.y });
			if (f.x != e.x && f.y != e.x) {
				if (reverse) {
					faces[f.x].insert({ e.x, f.y });
				}
				else {
					faces[f.x].insert({ f.y, e.x });
				}
			}

			if (f.x == e.x || f.y == e.x)
				faceN--;
			else {
				if (reverse) {
					faces[e.x].insert({ f.y, f.x });
				}
				else {
					faces[e.x].insert(f);
				}
			}

			auto tmp = glm::ivec2(glm::min(e.y, f.x), glm::max(e.y, f.x));
			if (std::find(edges.begin(), edges.end(), tmp) != edges.end())
				edges.erase(tmp);
			tmp = glm::ivec2(glm::min(e.y, f.y), glm::max(e.y, f.y));
			if (std::find(edges.begin(), edges.end(), tmp) != edges.end())
				edges.erase(tmp);
			if (f.x != e.x && f.y != e.x) {
				edges.insert({ glm::min(e.x, f.x), glm::max(e.x, f.x) });
				edges.insert({ glm::min(e.x, f.y), glm::max(e.x, f.y) });
			}
		}

		edges.erase(e);
		vertices[e.x] = v;
		vertices[e.y] = glm::vec3(0);
		removed[e.y] = true;
		faces[e.y].clear();

		std::set<int> neighbor;
		for (const auto& f : faces[e.x])
		{
			neighbor.insert(f.x);
			neighbor.insert(f.y);
		}
		for (auto nb : neighbor)
		{
			updateNeighborEdge(nb, threshold);
		}
	}

	bool faceReverse(const glm::ivec2& e, const glm::vec3& v1, const glm::vec3& v2)
	{
		const auto& x = vertices[e.x];
		const auto& y = vertices[e.y];
		return glm::dot(glm::cross(x - v1, y - v1), glm::cross(x - v2, y - v2)) < 0;
	}

	void updateNeighborEdge(int v, double threshold)
	{
		std::set<int> neighbor;
		for (const auto& f : faces[v])
		{
			neighbor.insert(f.x);
			neighbor.insert(f.y);
		}

		for (auto x : neighbor)
		{
			auto e = glm::ivec2(glm::min(x, v), glm::max(x, v));
			float len = glm::length(vertices[e.x] - vertices[e.y]);

			if (len > threshold) return;
			auto pos = calculateQ(e);
			heap.push(std::make_pair(-pos.second, e));
		}
	}

	void outerProductFast(const glm::vec4& a, const glm::vec4& b, glm::mat4& c)
	{
		c += glm::outerProduct(a, b);
	}
};

int main()
{
	std::vector<glm::vec3> vertices;
	std::vector<int> indexes;
	objread(R"(D:\user\modern-rendering-technology\models\bunny.obj)", vertices, indexes);

	QEM qem;
	qem.vertices = vertices;
	qem.indexes = indexes;

	std::vector<glm::vec3> newVertices;
	std::vector<int> newFaces;

	qem.init();
	qem.simplify(newVertices, newFaces, 0.1);

	objwrite(R"(D:\user\modern-rendering-technology\models\bunny_01.obj)", newVertices, newFaces);
}