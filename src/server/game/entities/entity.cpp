#include "entity.h"
#include "player.h"
#include "entities_key_def.hpp"

#include "baselib/message/game_db_object.pb.h"
#include "baselib/message/pbobj_convert.hpp"


namespace zq{

Entity::Entity(const std::string& guid)
	:guid_(guid)
{
	data_.setKey(guid);
}

Entity::~Entity()
{

}

bool Entity::covertBaseObjToPbObj(PBObject::Object& pb_obj)
{
	PbObjConvert::BaseObj2PbObj(data_, pb_obj);
	return true;
}

bool Entity::covertPbObjToBaseObj(const PBObject::Object& pb_obj)
{
	PbObjConvert::pbObj2BaseObj(pb_obj, data_);
	return true;
}


}
