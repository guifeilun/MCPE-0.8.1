#pragma once
#include <_types.h>

struct SmoothFloat
{
	float field_0, field_4, field_8;
	SmoothFloat() {
		this->field_0 = this->field_4 = this->field_8 = 0;
	}
	float getNewDeltaValue(float, float);
};
