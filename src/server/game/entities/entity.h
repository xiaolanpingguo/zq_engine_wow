#pragma once


#include "object_guid.h"
#include "object_defines.h"
#include "baselib/base_code/base_object.hpp"
#include "../grids/grid_reference.h"
#include <list>
#include <set>
#include <unordered_map>

namespace PBObject
{
	class Object;
}

namespace zq{


class Entity
{
public:

	Entity(const std::string& guid = "");
	virtual ~Entity();

public:

	bool covertBaseObjToPbObj(PBObject::Object& pb_obj);
	bool covertPbObjToBaseObj(const PBObject::Object& pb_obj);

	const std::string& getGuid() { return guid_; }
	BaseObject& getData() { return data_; }

	virtual bool parseFromString(const std::string& data) { return true; };
	virtual bool serialize(std::string& data) { return true; };
protected:


protected:

	uint16 m_objectType;
	std::string guid_;
	BaseObject data_;

private:

	Entity(Entity const& right) = delete;
	Entity& operator=(Entity const& right) = delete;
};


template<class T>
class GridObject
{
public:
	virtual ~GridObject() { }

	bool IsInGrid() const { return _gridRef.isValid(); }
	void AddToGrid(GridRefManager<T>& m) { ASSERT(!IsInGrid()); _gridRef.link(&m, (T*)this); }
	void RemoveFromGrid() { ASSERT(IsInGrid()); _gridRef.unlink(); }
private:
	GridReference<T> _gridRef;
};



}
