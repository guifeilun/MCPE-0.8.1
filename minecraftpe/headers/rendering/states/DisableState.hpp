#pragma once
#include <_types.h>
#include <unigl.hpp>

struct DisableState{
	GLenum disabled;

	DisableState(GLenum s){
		this->disabled = s;
		if(s) glDisable(this->disabled);
	}
	~DisableState();
};
