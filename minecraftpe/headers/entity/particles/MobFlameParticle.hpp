#pragma once
#include <entity/particles/Particle.hpp>

struct MobFlameParticle: Particle
{
	static std::string FLAME_ATLAS;

	float field_164;
	Entity* entity;
	float xOff, yOff, zOff;
	float field_178;
	MobFlameParticle(Level* level, ParticleType pt, const std::string& a2)
		: Particle(level, pt, a2){
		this->noclip = 1;
		this->bColMul = this->gColMul = this->rColMul = 1;
		this->xOff = this->yOff = this->zOff = 0;
		this->coordMultiplier = -0.25;
		this->texture = TextureUVCoordinateSet(0, 0, 1, 0.03125, 16, 512);

	}

	virtual ~MobFlameParticle();
	virtual void tick();
	virtual float getBrightness(float);
	virtual void init(float, float, float, float, float, float, int32_t);
	virtual void setEntity(Entity*);
	virtual void render(Tesselator&, float, float, float, float, float, float);
};
