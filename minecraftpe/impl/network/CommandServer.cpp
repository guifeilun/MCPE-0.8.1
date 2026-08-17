#include <network/CommandServer.hpp>
#include <entity/CameraEntity.hpp>
#include <entity/LocalPlayer.hpp>
#include <Minecraft.hpp>
#include <network/RakNetInstance.hpp>
#include <level/Level.hpp>
#include <network/ConnectedClient.hpp>
#include <nbt/Tag.hpp>
#include <ICreator.hpp>
#include <network/ConnectedClient.hpp>
#include <network/packet/AdventureSettingsPacket.hpp>

//not used in 0.8.1, pain to implement
std::string CommandServer::Ok = "\n", CommandServer::Fail = "Fail\n";
CommandServer::CommandServer(Minecraft* a2) {
	this->initialized = 0;
	this->_socket = 0;
	this->minecraft = a2;
	this->checkpoint = 0;
	this->field_30 = 0;
	this->field_34 = 0;
	this->field_38 = 0;
	this->camera = new CameraEntity(a2->level);
	TilePos res = a2->level->getSharedSpawnPos();
	this->posTranslator.x = -res.x;
	this->posTranslator.y = -res.y;
	this->posTranslator.z = -res.z;
}
void CommandServer::_close() {
	if(this->initialized) {
		if(this->_socket > 0) {
#ifdef __WIN32__
			closesocket(this->_socket);
#else
			close(this->_socket);
#endif
		}
		this->initialized = 0;
		this->_socket = 0;
	}
}
void CommandServer::_updateAccept(){
	printf("CommandServer::_updateAccept - not implemented\n");
	//TODO implement
}
bool_t CommandServer::_updateClient(ConnectedClient&){

	printf("CommandServer::_updateClient - not implemented\n");
	//TODO implement
	return 0;
}
void CommandServer::_updateClients(){

	printf("CommandServer::_updateClients - not implemented\n");
	//TODO implement
}
void CommandServer::dispatchPacket(Packet& a2) {
	if(this->minecraft->serverSideNetworkHandler) {
		if(this->minecraft->player) {
			this->minecraft->rakNetInstance->send(a2);
		}
	}
}
bool_t CommandServer::handleCheckpoint(bool_t a2){

	printf("CommandServer::handleCheckpoint - not implemented\n");
	//TODO implement
	return 0;
}
std::string CommandServer::handleEventPollMessage(ConnectedClient& client, const std::string& event) {
	ICreator* creator = this->minecraft->getCreator();
	if(!creator) {
		return CommandServer::Fail;
	}
	if(std::operator==(event, "events.clear")) {
		int time = this->minecraft->level->getTime();
		client.time = time;
		//returns some empty string?
		return Tag::NullString; //not exactly Tag::NullString, but the offset is very close~ - check later?
	}
	if(!std::operator==(event, "events.block.hits")) {
		return CommandServer::Fail;
	}
	std::stringstream st;
	creator->getEventList()->write(st, this->posTranslator, client.time);
	client.time = this->minecraft->level->getTime();
	st << "\n";
	return st.str();

}

void updateAdventureSettingFlag(Minecraft*, AdventureSettingsPacket::Flags, bool){
	printf("CommandServer updateAdventureSettingFlag - not implemented\n");
}

std::string CommandServer::handleSetSetting(const std::string& a2, int32_t a3) {
	bool v5 = a3 ? 1 : 0;
	if(a2 == "autojump") this->minecraft->player->allowAutojump = a3;
	if(a2 == "nametags_visible"){
		updateAdventureSettingFlag(this->minecraft, AdventureSettingsPacket::Flags::AS_FIELD_5, v5);
	}
	if(a2 == "world_immutable"){
		updateAdventureSettingFlag(this->minecraft, AdventureSettingsPacket::Flags::AS_ALLOW_INTERACT, v5);
	}
	//the original method might be slightly different?
	return Tag::NullString; //not exactly Tag::NullString, but the offset is very close~ - check later?
}
bool_t CommandServer::init(int16_t a2) {
	this->_close();
	this->_socket = socket(2, 1, 0);
	if(this->_socket < 0) {
		puts("Failed creating socket - 1");
		return 0;
	}
	printf("CommandServer::init - not implemented\n");
	//TODO implement
	this->initialized = 1;
	return 0; //1;
}
std::string CommandServer::parse(ConnectedClient&, const std::string&) {
	printf("CommandServer::parse - not implemented\n");
	//TODO implement
	return CommandServer::Fail;
}
void CommandServer::tick() {
	if(this->initialized) {
		this->_updateAccept();
		this->_updateClients();
		if(this->minecraft->viewEntityMaybe == this->camera) {
			this->minecraft->viewEntityMaybe->tick();
		}
	}
}
CommandServer::~CommandServer() {
	if(this->camera) delete this->camera;
	if(this->checkpoint) delete[] this->checkpoint;
}
