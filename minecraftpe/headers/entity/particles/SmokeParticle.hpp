#pragma once
#include <entity/particles/Particle.hpp>

struct SmokeParticle: Particle
{
	float field_164;
	SmokeParticle(Level* level, ParticleType pt, const std::string& a2)
		: Particle(level, pt, a2) {
		this->noclip = 0;
	}

	virtual ~SmokeParticle();
	virtual void tick();
	virtual void init(float, float, float, float, float, float, int32_t);
	virtual void render(Tesselator&, float, float, float, float, float, float);
};
