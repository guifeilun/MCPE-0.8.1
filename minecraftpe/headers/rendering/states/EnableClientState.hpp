#pragma once
#include <_types.h>
#include <unigl.hpp>

struct EnableClientState{
	GLenum enabled;

	EnableClientState(){
		this->enabled = 0;
	}
	EnableClientState(GLenum s){
		this->enabled = s;
		if(this->enabled) glEnableClientState(this->enabled);
	}
	~EnableClientState();
};
