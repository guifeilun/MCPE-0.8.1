#pragma once
#include <entity/particles/Particle.hpp>

struct CritParticle2: Particle
{
	int8_t field_164, field_165, field_166, field_167;
	float field_168;
	CritParticle2(Level* level, ParticleType pt, const std::string& a2)
		: Particle(level, pt, a2) {
		this->field_164 = 1;
		this->texture = {0.0625, 0.25, 0.125, 0.3125, 256, 256};
		this->noclip = 0;
	}
	virtual ~CritParticle2();
	virtual void tick();
	virtual void init(float, float, float, float, float, float, int32_t);
	virtual void render(Tesselator&, float, float, float, float, float, float);
};
