#pragma once
#include <entity/Mob.hpp>

struct CameraEntity: Mob
{
	int32_t followEid;

	CameraEntity(Level* a2);
	virtual ~CameraEntity();
	virtual void tick();
	virtual int32_t getEntityTypeId() const;
	virtual float getBaseSpeed();
};
