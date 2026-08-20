#pragma once
#include <_types.h>

struct TextureUVCoordinateSet{
	float minX, minY, maxX, maxY;
	float width, height;

	TextureUVCoordinateSet(float minX, float minY, float maxX, float maxY, float width, float height){
		this->width = width;
		this->height = height;
		this->minX = minX;
		this->minY = minY;
		this->maxX = maxX;
		this->maxY = maxY;
	}


	TextureUVCoordinateSet(){
		this->minX = 0;
		this->minY = 0;
		this->maxX = 0;
		this->maxY = 0;
	}
	static TextureUVCoordinateSet fromOldSystem(int32_t);
};
