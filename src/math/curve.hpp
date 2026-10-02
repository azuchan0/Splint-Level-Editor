#pragma once 
#include <vector>
#include <glm/glm.hpp>

class Curve {
public:
	virtual ~Curve() = default;
	virtual void addControlPoint(const glm::vec2& point) {
		m_ControlPoints.push_back(point);
	}
	virtual void setControlPoint(int index, const glm::vec2& point) {
		if (index >= 0 && index < m_ControlPoints.size()) {
			m_ControlPoints[index] = point;
		}
	}
	virtual void clear() {
		m_ControlPoints.clear();
		m_Vertices.clear();
	}
	virtual void generateVertices(int resolution) = 0;
	const std::vector<glm::vec2>& getControlPoints() const { return m_ControlPoints; }
	const std::vector<glm::vec2>& getVertices() const { return m_Vertices; }
protected:
	std::vector<glm::vec2> m_ControlPoints;
	std::vector<glm::vec2> m_Vertices;
};