#include <level/storage/BundledLevelStorage.hpp>
#include <BinaryBlob.hpp>
#include <BitStream.h>
#include <level/chunk/LevelChunk.hpp>
#include <level/gen/RandomLevelSource.hpp>
#include <sstream>
#include <util/Util.hpp>
#include <zlib.h>

BundledLevelStorage::BundledLevelStorage(struct Minecraft* a2, const std::string& a3)
	: MemoryLevelStorage(0)
	, levelNameMaybe(a3) {
	this->levelData = new LevelData();
	this->field_14 = 0;
	this->field_18 = 3;
	this->field_1C = 0;
	this->minecraft = a2;
}

BundledLevelStorage::~BundledLevelStorage() {
	if(this->levelData) {
		delete this->levelData;
	}
	if(this->field_1C) {
		delete this->field_1C;
	}
}
LevelData* BundledLevelStorage::prepareLevel(Level* a2) {
	this->field_14 = a2;
	this->field_1C = new RandomLevelSource(a2, Util::hashCode("Nya1n"), 0, 1);
	bool_t v3 = this->levelNameMaybe.find("survival") == -1;
	this->levelData->setGameType(v3);
	this->levelData->setSpawnMobs(!v3);
	return this->levelData;
}
ChunkStorage* BundledLevelStorage::createChunkStorage(Dimension*) {
	return this;
}
bool_t BundledLevelStorage::load(Player*) {
	return 0;
}
LevelChunk* BundledLevelStorage::load(Level*, int32_t, int32_t) {
	printf("BundledLevelStorage::load - not implemented\n"); //TODO
	return 0;
}
void BundledLevelStorage::save2(Level* a2, LevelChunk* a3) {
	std::stringstream v13;
	v13 << "c:/temp/level/chunk_" << a3->chunkX << "_" << a3->chunkZ << ".bin";
	FILE* v8 = fopen(v13.str().c_str(), "wb");
	if(v8) {
		RakNet::BitStream v14;
		char v15[1024];
		v14.Write((char*)a3->tiles, 0x8000u);
		v14.Write((char*)a3->tileMeta.data, 0x4000u);
		v14.Write((char*)a3->skyLight.data, 0x4000u);
		v14.Write((char*)a3->blockLight.data, 0x4000u);
		v14.Write((char*)a3->updateMap, 0x100u);
		z_stream strm;
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		deflateInit2(&strm, -1, 8, 15, 8, 3);
		int ret;
		do {
			strm.avail_out = sizeof(v15);
			strm.next_out = (Bytef*)v15;
			ret = deflate(&strm, 4);
			fwrite(v15, sizeof(*v15), sizeof(v15) - strm.avail_out, v8);
		} while(ret != -1 && strm.avail_out == 0);
		deflateEnd(&strm);
		fclose(v8);
	}
}
AssetFile BundledLevelStorage::getChunkData(int32_t, int32_t) {
	printf("BundledLevelStorage::getChunkData - not implemented\n"); //TODO
	return AssetFile(0, 0);
}
void BundledLevelStorage::freeChunkData(int32_t a2, int32_t a3, BinaryBlob* a4) {
	delete a4->ptr1;
	a4->ptr1 = 0;
	a4->field_4 = -1;
}
