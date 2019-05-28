#pragma once


#include "entity.h"
#include "interface_header/logic/IObjectMgrModule.h"
#include "interface_header/base/IDataAgentModule.h"
#include "interface_header/base/IConfigModule.h"
#include "interface_header/base/IClassModule.h"

namespace zq{


class ObjectMgrModule : public IObjectMgrModule
{
	//using ItemTemplateContainer = std::unordered_map<uint32, ItemTemplate>;
public:
	ObjectMgrModule(ILibManager* p);
	virtual ~ObjectMgrModule();

	bool init() override;
	bool initEnd() override;
	bool run() override;

	std::string createGuid(HighGuid high) override;
	Item* createItem(uint32 itemEntry, uint32 count) override;


protected:

	//ObjectGuidGeneratorBase& getGuidSequenceGenerator(HighGuid type);

	//const ItemTemplate* getItemTemplate(uint32 entry) const;

private:

	//std::map<HighGuid, std::unique_ptr<ObjectGuidGeneratorBase>> _guidGenerators;
	//std::unordered_map<ObjectGuid, Object*> objects_;
private:

	IClassModule* classModule_;
	IConfigModule* configModule_;
	IDataAgentModule* dataAgentModule_;
};

}
