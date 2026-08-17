#include <ICreator.hpp>
#include <util/OffsetPosTranslator.hpp>

void ICreator::TileEvent::write(std::stringstream& a2, IPosTranslator& a3) {
	int32_t v13 = this->field_4;
	int32_t v14 = this->field_8;
	int32_t v15 = this->field_C;

	a3.to(v13, v14, v15);
	a2 << v13 << "," << v14 << "," << v15 << "," << this->field_10 << "," << this->field_0;
}

void ICreator::EventList<ICreator::TileEvent>::write(std::stringstream& stream, IPosTranslator& tr, int clientTime) {
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
