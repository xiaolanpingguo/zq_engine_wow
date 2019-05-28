#include "player_mgr_module.h"
#include "item.h"
#include "entities_key_def.hpp"
#include "config_header/cfg_item_template.hpp"

namespace zq {


PlayerMgrModule::PlayerMgrModule(ILibManager* p)
{
	libManager_ = p;
}

PlayerMgrModule::~PlayerMgrModule()
{

}

bool PlayerMgrModule::init()
{
	objectMgrModule_ = libManager_->findModule<IObjectMgrModule>();
	configModule_ = libManager_->findModule<IConfigModule>();
	return true;
}

bool PlayerMgrModule::initEnd()
{

	return true;
}

bool PlayerMgrModule::run()
{

	return true;
}

bool PlayerMgrModule::storeNewItemInBestSlots(Player* player, uint32 item_id, uint32 item_count)
{
	const CSVItemTemplate* proto = configModule_->getCsvRow<CSVItemTemplate>(item_id); 
	if (proto == nullptr)
	{
		return false;
	}

	int slot = NULL_SLOT;
	InventoryResult result = canEquipItem(player, item_id, slot);
	if (result == EQUIP_ERR_OK)
	{	   
		auto& equip_slots = player->getEquipSlots();
		Item* new_item = objectMgrModule_->createItem(item_id, 1);
		equip_slots[slot] = new_item;
	}

	return true;
}

InventoryResult PlayerMgrModule::canEquipItem(Player* player, uint32 item_id, int& slot)
{
	auto& equip_slots = player->getEquipSlots();
	const CSVItemTemplate* proto = configModule_->getCsvRow<CSVItemTemplate>(item_id);
	if (proto == nullptr)
	{
		return EQUIP_ERR_ITEM_NOT_FOUND;
	}

	// 查找可用的装备插槽
	uint8 equip_slot = FindEquipSlot(player, proto->InventoryType, NULL_SLOT);
	if (equip_slot != NULL_SLOT)
	{
		slot = equip_slot;
		return EQUIP_ERR_OK;
	}

	return EQUIP_ERR_ITEM_NOT_FOUND;
}

uint8 PlayerMgrModule::FindEquipSlot(Player* player, int InventoryType, uint32 slot) 
{
	int32 playerClass = player->getData().getValueInt32(PlayerKeys::player_class);
	auto& equip_slots = player->getEquipSlots();

	uint8 slots[4];
	slots[0] = NULL_SLOT;
	slots[1] = NULL_SLOT;
	slots[2] = NULL_SLOT;
	slots[3] = NULL_SLOT;
	switch (InventoryType)
	{
	case INVTYPE_HEAD:
		slots[0] = EQUIPMENT_SLOT_HEAD;
		break;
	case INVTYPE_NECK:
		slots[0] = EQUIPMENT_SLOT_NECK;
		break;
	case INVTYPE_SHOULDERS:
		slots[0] = EQUIPMENT_SLOT_SHOULDERS;
		break;
	case INVTYPE_BODY:
		slots[0] = EQUIPMENT_SLOT_BODY;
		break;
	case INVTYPE_CHEST:
		slots[0] = EQUIPMENT_SLOT_CHEST;
		break;
	case INVTYPE_ROBE:
		slots[0] = EQUIPMENT_SLOT_CHEST;
		break;
	case INVTYPE_WAIST:
		slots[0] = EQUIPMENT_SLOT_WAIST;
		break;
	case INVTYPE_LEGS:
		slots[0] = EQUIPMENT_SLOT_LEGS;
		break;
	case INVTYPE_FEET:
		slots[0] = EQUIPMENT_SLOT_FEET;
		break;
	case INVTYPE_WRISTS:
		slots[0] = EQUIPMENT_SLOT_WRISTS;
		break;
	case INVTYPE_HANDS:
		slots[0] = EQUIPMENT_SLOT_HANDS;
		break;
	case INVTYPE_FINGER:
		slots[0] = EQUIPMENT_SLOT_FINGER1;
		slots[1] = EQUIPMENT_SLOT_FINGER2;
		break;
	case INVTYPE_TRINKET:
		slots[0] = EQUIPMENT_SLOT_TRINKET1;
		slots[1] = EQUIPMENT_SLOT_TRINKET2;
		break;
	case INVTYPE_CLOAK:
		slots[0] = EQUIPMENT_SLOT_BACK;
		break;
	case INVTYPE_WEAPON:
	{
		slots[0] = EQUIPMENT_SLOT_MAINHAND;

		// suggest offhand slot only if know dual wielding
		// (this will be replace mainhand weapon at auto equip instead unwonted "you don't known dual wielding" ...
		//if (CanDualWield())
		//	slots[1] = EQUIPMENT_SLOT_OFFHAND;
		break;
	}
	case INVTYPE_SHIELD:
		slots[0] = EQUIPMENT_SLOT_OFFHAND;
		break;
	case INVTYPE_RANGED:
		slots[0] = EQUIPMENT_SLOT_RANGED;
		break;
	case INVTYPE_2HWEAPON:
		slots[0] = EQUIPMENT_SLOT_MAINHAND;
		//if (Item* mhWeapon = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
		//{
		//	if (ItemTemplate const* mhWeaponProto = mhWeapon->GetTemplate())
		//	{
		//		if (mhWeaponProto->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM || mhWeaponProto->SubClass == ITEM_SUBCLASS_WEAPON_STAFF)
		//		{
		//			const_cast<Player*>(this)->AutoUnequipOffhandIfNeed(true);
		//			break;
		//		}
		//	}
		//}

		//if (GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
		//{
		//	if (proto->SubClass == ITEM_SUBCLASS_WEAPON_POLEARM || proto->SubClass == ITEM_SUBCLASS_WEAPON_STAFF)
		//	{
		//		const_cast<Player*>(this)->AutoUnequipOffhandIfNeed(true);
		//		break;
		//	}
		//}
		//if (CanDualWield() && CanTitanGrip() && proto->SubClass != ITEM_SUBCLASS_WEAPON_POLEARM && proto->SubClass != ITEM_SUBCLASS_WEAPON_STAFF)
		//	slots[1] = EQUIPMENT_SLOT_OFFHAND;
		break;
	case INVTYPE_TABARD:
		slots[0] = EQUIPMENT_SLOT_TABARD;
		break;
	case INVTYPE_WEAPONMAINHAND:
		slots[0] = EQUIPMENT_SLOT_MAINHAND;
		break;
	case INVTYPE_WEAPONOFFHAND:
		slots[0] = EQUIPMENT_SLOT_OFFHAND;
		break;
	case INVTYPE_HOLDABLE:
		slots[0] = EQUIPMENT_SLOT_OFFHAND;
		break;
	case INVTYPE_THROWN:
		slots[0] = EQUIPMENT_SLOT_RANGED;
		break;
	case INVTYPE_RANGEDRIGHT:
		slots[0] = EQUIPMENT_SLOT_RANGED;
		break;
		/*case INVTYPE_BAG:
			slots[0] = INVENTORY_SLOT_BAG_START + 0;
			slots[1] = INVENTORY_SLOT_BAG_START + 1;
			slots[2] = INVENTORY_SLOT_BAG_START + 2;
			slots[3] = INVENTORY_SLOT_BAG_START + 3;
			break;*/
	case INVTYPE_RELIC:
	{
		/*switch (proto->SubClass)
		{
		case ITEM_SUBCLASS_ARMOR_LIBRAM:
			if (playerClass == CLASS_PALADIN)
				slots[0] = EQUIPMENT_SLOT_RANGED;
			break;
		case ITEM_SUBCLASS_ARMOR_IDOL:
			if (playerClass == CLASS_DRUID)
				slots[0] = EQUIPMENT_SLOT_RANGED;
			break;
		case ITEM_SUBCLASS_ARMOR_TOTEM:
			if (playerClass == CLASS_SHAMAN)
				slots[0] = EQUIPMENT_SLOT_RANGED;
			break;
		case ITEM_SUBCLASS_ARMOR_MISC:
			if (playerClass == CLASS_WARLOCK)
				slots[0] = EQUIPMENT_SLOT_RANGED;
			break;
		case ITEM_SUBCLASS_ARMOR_SIGIL:
			if (playerClass == CLASS_DEATH_KNIGHT)
				slots[0] = EQUIPMENT_SLOT_RANGED;
			break;
		}*/
		break;
	}
	default:
		return NULL_SLOT;
	}

	if (slot != NULL_SLOT)
	{
		if (!equip_slots[slot])
		{
			for (uint8 i = 0; i < 4; ++i)
			{
				if (slots[i] == slot)
					return slot;
			}
		}
	}
	else
	{
		// search free slot at first
		for (uint8 i = 0; i < 4; ++i)
		{
			if (slots[i] != NULL_SLOT && !equip_slots[slots[i]])
			{
				return slots[i];
			}
		}
	}

	// no free position
	return NULL_SLOT;
}

}

