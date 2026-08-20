#include <rendering/states/EnableClientState.hpp>

EnableClientState::~EnableClientState(){
	if(this->enabled){
		glDisableClientState(this->enabled);
	}
}
