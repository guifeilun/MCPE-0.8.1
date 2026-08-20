#include <rendering/states/EnableState.hpp>

EnableState::~EnableState(){
	if(this->enabled) glDisable(this->enabled);
}
