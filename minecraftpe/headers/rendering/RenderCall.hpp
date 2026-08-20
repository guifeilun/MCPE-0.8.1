#pragma once
#include <_types.h>
#include <rendering/MeshBuffer.hpp>
#include <string>

struct RenderCall
{
	int32_t field_0 = 0;
	MeshBuffer field_4;
	std::string field_2C;
	bool_t field_30 = 0, field_31 = 0, field_32 = 0;
	int8_t field_33 = 0;
	RenderCall(){
		//TODO memset to zero before constructor called? - is there even a constructor?
		this->field_31 = this->field_32 = 0;
	}
	~RenderCall();
};
