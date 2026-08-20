#pragma once
#include <_types.h>

struct AssetFile{
	uint8_t* bytes;
	int32_t length;

	AssetFile(uint8_t* bytes, int32_t length){
		this->bytes = bytes;
		this->length = length;
	}
};
