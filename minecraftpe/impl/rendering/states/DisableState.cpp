#include <rendering/states/DisableState.hpp>

DisableState::~DisableState(){
	if(this->disabled) glEnable(this->disabled);
}
