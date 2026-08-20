#pragma once
#include <entity/particles/Particle.hpp>

struct LavaParticle: Particle
{
	float scaleMult;
	LavaParticle(Level* level, ParticleType pt, const std::string& a2)
		: Particle(level, pt, a2) {
		this->bColMul = this->gColMul = this->rColMul = 1;
		this->noclip = 0;
		this->texture = TextureUVCoordinateSet(0, 0.1875, 0.125, 0.25, 256, 256);
	}

	virtual ~LavaParticle();
	virtual void tick();
	virtual float getBrightness(float);
	virtual void init(float, float, float, float, float, float, int32_t);
	virtual void render(Tesselator&, float, float, float, float, float, float);
};
