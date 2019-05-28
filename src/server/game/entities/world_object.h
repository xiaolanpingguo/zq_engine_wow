

#include "entity.h"
#include "position.h"
#include <list>
#include <set>
#include <unordered_map>


namespace zq{


class Map;
class WorldObject : public Entity, public WorldLocation
{
protected:
	explicit WorldObject(bool isWorldObject) {}; //note: here it means if it is in grid object list or world object list
public:
	virtual ~WorldObject() {};

	virtual void Update(uint32 /*time_diff*/) { }

};


}
