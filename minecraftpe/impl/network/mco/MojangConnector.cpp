#include <network/mco/MojangConnector.hpp>
#include <util/ThreadCollection.hpp>
#include <network/mco/LoginInformation.hpp>
#include <Minecraft.hpp>
#include <network/mco/MCOParser.hpp>
#include <network/RestService.hpp>
#include <util/Common.hpp>
#include <network/mco/MCOPayloadPacker.hpp>
#include <oaes_lib.h>
#include <string.h>
#include <util/Base64.hpp>
#include <network/mco/RestRequestJob.hpp>
#include <util/Util.hpp>
#include <sstream>

#ifndef AUTH_SERVER
#define AUTH_SERVER "https://authserver.mojang.com"
#endif

#ifndef PEOAPI_SERVER
#define PEOAPI_SERVER "https://peoapi.minecraft.net"
#endif
MojangConnector::MojangConnector(Minecraft* minecraft) {
	this->serverCreationEnabled = 0;
	this->serviceEnabled = 0;
	this->random = std::shared_ptr<Random>(new Random());
	this->loginInformation = std::make_shared<LoginInformation>(minecraft->platform()->getLoginInformation());
	this->minecraft = minecraft;
	this->threadCollection = std::shared_ptr<ThreadCollection>(new ThreadCollection(4));
	this->mcoParser = std::shared_ptr<MCOParser>(new MCOParser());
	this->accountService = std::shared_ptr<RestService>(new RestService(AUTH_SERVER));
	this->mcoService = std::shared_ptr<RestService>(new RestService(PEOAPI_SERVER));
	this->gameVersionNet = Common::getGameVersionStringNet();
	this->mcoService->setCookieData("version", this->gameVersionNet);
	this->status = STATUS_0;
}

void MojangConnector::clearLoginInformation() {
	this->setLoginInformation(LoginInformation());
}
std::shared_ptr<RestService> MojangConnector::getAccountSercice() {
	return this->accountService;
}
MojangConnectionStatus MojangConnector::getConnectionStatus() {
	return this->status;
}
std::string MojangConnector::getEncryptedJoinDataString(long long a3, const std::string& a4, const std::string& a5) {
	MCOPayloadPacker v7(*this->random);
	std::string v8 = v7.writeBitStream(a3, a4);
	OAES_CTX* ctx = oaes_alloc();
	oaes_set_option(ctx, OAES_OPTION_ECB, 0);
	oaes_key_import_data(ctx, (const uint8_t*)a5.c_str(), a5.size());
	char v12[512];
	memset(v12, 0, sizeof(v12));
	size_t v10 = 512;
	oaes_encrypt(ctx, (const uint8_t*)v8.c_str(), v8.size(), (uint8_t*)v12, &v10);
	std::string v11(&v12[32], v10 - 32);
	return Base64::base64Encode(v11);
}
std::string* MojangConnector::getJoinMCOPayload() {
	return &this->joinMCOPayload;
}
std::shared_ptr<LoginInformation> MojangConnector::getLoginInformation() {
	return this->loginInformation;
}
std::shared_ptr<MCOParser> MojangConnector::getMCOParser() {
	return this->mcoParser;
}
std::shared_ptr<std::unordered_map<long long, MCOServerListItem>> MojangConnector::getMCOServerList() {
	return this->serverList;
}
std::shared_ptr<RestService> MojangConnector::getMCOService() {
	return this->mcoService;
}
std::string* MojangConnector::getServerKey() {
	return &this->serverKey;
}
std::shared_ptr<ThreadCollection> MojangConnector::getThreadCollection() {
	return this->threadCollection;
}
bool_t MojangConnector::isMCOCreateServersEnabled() {
	return this->status == STATUS_2 && this->serverCreationEnabled;
}
bool_t MojangConnector::isServiceEnabled() {
	return this->serviceEnabled;
}
void MojangConnector::setLoginInformation(const LoginInformation& a2) {
	this->loginInformation = std::shared_ptr<LoginInformation>(new LoginInformation(a2));
	std::stringstream v14;
	v14 << "token:" << this->loginInformation->accessToken << ":" << this->loginInformation->profileId;
	this->mcoService->setCookieData("sid", v14.str());
	this->mcoService->setCookieData("user", this->loginInformation->profileName);
	if(a2.accessToken != "") { //TODO check: compareStringsMaybe(&a2->accessToken, &_byte_nullstr_D67153C4)
		this->minecraft->options.set(&Options::Option::NAME, this->loginInformation->profileName);
		this->setStatus(STATUS_2);
		this->minecraft->platform()->setLoginInformation(a2);
	} else {
		this->setStatus(STATUS_0);
	}
}
void MojangConnector::setMCOCreateServersEnabled(bool_t a2) {
	this->serverCreationEnabled = a2;
}
void MojangConnector::setMCOServerList(std::shared_ptr<std::unordered_map<long long, MCOServerListItem>> a2) {
	this->serverList = a2;
}
void MojangConnector::setMCOServiceEnabled(bool_t a2) {
	this->serviceEnabled = a2;
}
void MojangConnector::setPayload(const std::string& a2) {
	this->joinMCOPayload = a2;
}
void MojangConnector::setServerKey(const std::string& a2) {
	this->getMCOService()->setCookieData("key", a2);
	this->serverKey = a2;
}
void MojangConnector::setStatus(MojangConnectionStatus status) {
	if(status != this->status) {
		if(status == STATUS_2) {
			std::shared_ptr<RestRequestJob> v8 = RestRequestJob::CreateJob(RRT_GET, this->getMCOService(), this->minecraft);
			v8->field_30 = Util::simpleFormat("/info/status", {});
			RestRequestJob::launchRequest(
				v8,
				this->getThreadCollection(),
				[this](int32_t a2, const std::string& a3, const RestCallTagData& a4, std::shared_ptr<RestRequestJob> a5) {
					bool v8 = 0;
					bool createServersEnabled = 0;
					bool serviceEnabled = 0;
					this->getMCOParser()->parseStatus(a3, v8, createServersEnabled, serviceEnabled);
					this->setMCOServiceEnabled(serviceEnabled);
					this->setMCOCreateServersEnabled(createServersEnabled);
				},
				[this](bool, bool, int32_t, const std::string&, const RestCallTagData&, std::shared_ptr<RestRequestJob>) {
					this->serviceEnabled = 0;
					this->serverCreationEnabled = 0;
				}
			);
		} else {
			this->serviceEnabled = 0;
		}
		this->status = status;
		this->minecraft->onMojangConnectorStatus(status);

	}
}
void MojangConnector::updateUIThread() {
	this->threadCollection->processUIThread();
}
std::string MojangConnector::urlEncode(std::string a2) {
	printf("MojangConnector::urlEncode(%s) - not implemented\n", a2.c_str()); //TODO
	return "";
}
MojangConnector::~MojangConnector() {
}
