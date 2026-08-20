#pragma once
#include <_types.h>
struct Random{
	int32_t seed;
	uint32_t permutations[624];
	int32_t index;
	bool_t haveNextNextGaussian;
	float nextNextGaussian;

	Random(long seed){
		this->setSeed(seed);
	}
	Random(void);
	void init_genrand(unsigned long);
	float nextFloat(void);
	float nextGaussian(void);
	uint32_t genrand_int32(void);
	void setSeed(long);
};
