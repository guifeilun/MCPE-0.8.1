#pragma once
#include <_types.h>
#include <string>

struct ConnectedClient{
	int sock;
	std::string field_4;
	int time;

	ConnectedClient(int sock); //inlined
};
