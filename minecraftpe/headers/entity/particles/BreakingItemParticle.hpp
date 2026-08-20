#pragma once
#include <entity/particles/Particle.hpp>

struct BreakingItemParticle: Particle
{
	struct Item* item;
	BreakingItemParticle(Level* level, ParticleType pt, const std::string& a2, Item* it)
		: Particle(level, pt, a2) {
		this->item = it;
	}

	virtual ~BreakingItemParticle();
	virtual void init(float, float, float, float, float, float, int32_t);
	virtual void render(Tesselator&, float, float, float, float, float, float);
};
