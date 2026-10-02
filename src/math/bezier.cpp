#include "bezier.hpp"

void Bezier::generateVertices(int resolution) {
	m_Vertices.clear();
	if (m_ControlPoints.size() < 2) return;

	if (m_method == Method::DIRECT) {
		generateDirect(resolution);
	}
	else {
		generateCasteljau(resolution);
	}
}

//pascal triangle compute
unsigned long long Bezier::computeCombinatorial(int n, int k) {
	if (k == 0 || k == n) return 1;
	if (k > n / 2) k = n - k;
	unsigned long long res = 1;
	for (int i = 1; i <= k; ++i) {
		res = res * (n - i + 1) / i;
	}
	return res;
}

void Bezier::generateDirect(int resolution) {
	int n = m_ControlPoints.size() - 1;
	
	for (int step = 0; step <= resolution; ++step) {
		float t = (float)step / (float)resolution;
		glm::vec2 point(0.0f, 0.0f);

		for (int i = 0; i <= n; ++i) {
			float bernstein = computeCombinatorial(n, i) * std::pow(t, i) * std::pow(1.0f - t, n - i);
			point += m_ControlPoints[i] * bernstein;
		}
		m_Vertices.push_back(point);
	}
}

void Bezier::generateCasteljau(int resolution) {
	int n = m_ControlPoints.size() - 1;

	for (int step = 0; step <= resolution; ++step) {
		float t = (float)step / (float)resolution;

		std::vector<glm::vec2> temp = m_ControlPoints;

		for (int r = 1; r <= n; r++) {
			for (int i = 0; i <= n - r; i++) {
				temp[i] = (1.0f - t) * temp[i] + t * temp[i + 1];
			}
		}
		m_Vertices.push_back(temp[0]);
	}
}








