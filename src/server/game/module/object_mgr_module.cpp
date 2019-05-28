#include "object_mgr_module.h"	
#include "item.h"
#include "entities_key_def.hpp"
#include "baselib/base_code/containers.hpp"
#include "config_header/cfg_item_template.hpp"

namespace zq {


ObjectMgrModule::ObjectMgrModule(ILibManager* p)
{
	libManager_ = p;
}

ObjectMgrModule::~ObjectMgrModule()
{

}

bool ObjectMgrModule::init()
{
	classModule_ = libManager_->findModule<IClassModule>();
	dataAgentModule_ = libManager_->findModule<IDataAgentModule>();
	configModule_ = libManager_->findModule<IConfigModule>();
	return true;
}

bool ObjectMgrModule::initEnd()
{

	return true;
}

bool ObjectMgrModule::run()
{

	return true;
}

std::string ObjectMgrModule::createGuid(HighGuid high)
{
	return std::to_string(classModule_->genUuid((uint16)high));
}

Item* ObjectMgrModule::createItem(uint32 itemEntry, uint32 count)
{
	if (count < 1)
		return nullptr;                            

	if (const CSVItemTemplate* proto = configModule_->getCsvRow<CSVItemTemplate>(itemEntry); proto)
	{
		if (count > proto->GetMaxStackSize())
			count = proto->GetMaxStackSize();

		Item* item = new Item(createGuid(HighGuid::Item));
		BaseObject&	data = item->getData();
		data.appendValue(ItemKeys::item_id, itemEntry);
		data.appendValue(ItemKeys::item_count, count);
		return item;
		//if (item->Create(createGuid(HighGuid::Item, itemEntry))
		//{
		//	item->SetCount(count);
		//	return item;
		//}
		//else
		//	delete item;
	}

	return nullptr;
}

//ObjectGuidGeneratorBase& ObjectMgrModule::getGuidSequenceGenerator(HighGuid type)
//{
//	auto itr = _guidGenerators.find(type);
//	if (itr == _guidGenerators.end())
//		itr = _guidGenerators.insert(std::make_pair(type, std::unique_ptr<ObjectGuidGenerator>(new ObjectGuidGenerator()))).first;
//
//	return *itr->second;
//}

//const ItemTemplate* ObjectMgrModule::getItemTemplate(uint32 entry) const
//{
//	return Containers::MapGetValuePtr(itemTemplateStore_, entry);
//}
//
}

