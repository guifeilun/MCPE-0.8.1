#pragma once
#include <_types.h>
#include <sstream>

struct IPosTranslator;

struct ICreator
{
	struct TileEvent
	{
		int field_0, field_4, field_8, field_C;
		int field_10;

		void write(std::stringstream&, IPosTranslator&);
	};

	//0.7.2:
	//v11 = (*(int (__fastcall **)(int))(*(_DWORD *)Creator + 8))(Creator);
	//std::basic_stringstream<char,std::char_traits<char>,std::allocator<char>>::basic_stringstream(v13, 24);
	//ICreator::EventList<ICreator::TileEvent>::write(v11, v13, a2 + 28, *(_DWORD *)(a3 + 28));
	//seems like ICreator::EventList was inlined in later versions, the first entry here is ICreator::getEventList ?

	template<typename T>
	struct EventList
	{
		int field_0;
		int count;
		int field_8;
		struct
		{ //TODO check what type does this thing have
			int time;
			T event;
		}* events;
		void write(std::stringstream&, IPosTranslator&, int);
	};

	virtual ~ICreator();
	virtual ICreator::EventList<ICreator::TileEvent>* getEventList();
};

template<>
inline void ICreator::EventList<ICreator::TileEvent>::write(std::stringstream& stream, IPosTranslator& tr, int clientTime) {
	//TODO check does this actually work
	int v13 = 0;
	int v14 = this->field_0 + 1;
	int count = this->count;

	while(v13 < count) {
		if(v14 == count) v14 = 0;
		if(this->events[v14].time >= clientTime) {
			if(v14 >= 0) {
				while(1) {
					this->events[v14].event.write(stream, tr);
					if(v14 == this->field_0) break;
					stream << '|';
					if(++v14 == this->count) v14 = 0;
				}
			}
			break;
		}
		++v14;
		++v13;
	}
}
