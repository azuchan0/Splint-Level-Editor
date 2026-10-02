#pragma once 
#include "curve.hpp"

class Bezier : public Curve {
public:
	enum class Method {
		DIRECT, 
		CASTELJAU
	};

	void generateVertices(int resolution) override;
	void setMethod(Method method) { m_method = method; }
private:
	Method m_method = Method::CASTELJAU;
	void generateDirect(int resolution);
	void generateCasteljau(int resolution);

	unsigned long long computeCombinatorial(int n, int k);
};