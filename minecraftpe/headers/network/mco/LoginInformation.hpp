#pragma once
#include <_types.h>
#include <string>

struct LoginInformation{
	std::string accessToken;
	std::string clientId;
	std::string profileId;
	std::string profileName;
	LoginInformation() {
		this->accessToken = "";
		this->clientId = "";
		this->profileId = "";
		this->profileName = "";
	}

	~LoginInformation(void);
};
