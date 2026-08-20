#include <rendering/states/BlendFunctionState.hpp>

BlendFunctionState::~BlendFunctionState(){
	glBlendFunc(0x302, 0x303);
}
