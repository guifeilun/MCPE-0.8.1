#pragma once
#include <_types.h>
#include <unigl.hpp>

struct EnableState{
	GLenum enabled;

	EnableState(GLenum s){
		this->enabled = s;
		if(s) glEnable(this->enabled);
	}
	~EnableState();
};
