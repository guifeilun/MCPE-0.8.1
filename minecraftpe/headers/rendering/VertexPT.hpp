#pragma once
#include <_types.h>
#include <math/Vec3.hpp>
struct VertexPT{
	Vec3 vec;
	float u, v;

	VertexPT(){
		this->u = 0;
		this->v = 0;
	}
	VertexPT(const VertexPT&, float, float);
};
