#pragma once
#include <entity/particles/Particle.hpp>

struct BubbleParticle: Particle
{
	BubbleParticle(Level* level, ParticleType pt, const std::string& a2)
		: Particle(level, pt, a2) {
		this->rColMul = 1;
		this->gColMul = 1;
		this->bColMul = 1;
		this->setSize(0.02, 0.02);
		this->texture = TextureUVCoordinateSet(0, 0.125, 0.0625, 0.1875, 256, 256);
	}
	virtual ~BubbleParticle();
	virtual void tick();
	virtual void init(float, float, float, float, float, float, int32_t);
};
