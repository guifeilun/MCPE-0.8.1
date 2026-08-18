#include <entity/CameraEntity.hpp>
#include <level/Level.hpp>
CameraEntity::CameraEntity(Level* a2)
	: Mob(a2) {
	this->followEid = -1;
	this->moveTo(128.0, 72.0, 128.0, 0.0, 90.0);
}
CameraEntity::~CameraEntity() {
}
void CameraEntity::tick() {
	if(this->followEid >= 0) {
		this->prevPosX = this->prevX = this->posX;
		this->prevPosY = this->prevY = this->posY;
		this->prevPosZ = this->prevZ = this->posZ;
		this->prevPitch = this->pitch;
		this->prevYaw = this->yaw;
		Entity* e = this->level->getEntity(this->followEid);
		if(e) {
			this->setPos(e->posX, e->posY + 6.0, e->posZ);
		}
	}
}
int32_t CameraEntity::getEntityTypeId() const {
	return 0;
}
float CameraEntity::getBaseSpeed() {
	return 0.2;
}
