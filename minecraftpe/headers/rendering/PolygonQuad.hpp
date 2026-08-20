#pragma once
#include <_types.h>
#include <rendering/VertexPT.hpp>

struct PolygonQuad{
	VertexPT verticies[4];
	bool_t flipnormal;
	char align1, align2, align3;

	PolygonQuad(){
		for(int i = 0; i < 4; ++i){
			this->verticies[i].vec.x = 0;
			this->verticies[i].vec.y = 0;
			this->verticies[i].vec.z = 0;
		}
	}
	PolygonQuad(VertexPT*, VertexPT*, VertexPT*, VertexPT*);
	PolygonQuad(VertexPT*, VertexPT*, VertexPT*, VertexPT*, float, float, float, float);
	PolygonQuad(VertexPT*, VertexPT*, VertexPT*, VertexPT*, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t);
	void flipNormal(void);
	void mirror(void);
	void render(struct Tesselator&, float, int32_t);
};

