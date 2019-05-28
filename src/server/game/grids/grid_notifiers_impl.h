//#include "grid_notifiers.h"
//#include "../entities/corpse/corpse.h"
////#include "CreatureAI.h"
//#include "../entities/player/player.h"
//#include "SpellAuras.h"
//#include "UpdateData.h"
//#include "WorldPacket.h"
//#include "WorldSession.h"
//
//namespace zq{}
//
//template<class T>
//inline void VisibleNotifier::Visit(GridRefManager<T> &m)
//{
//    for (typename GridRefManager<T>::iterator iter = m.begin(); iter != m.end(); ++iter)
//    {
//        vis_guids.erase(iter->GetSource()->GetGUID());
//        i_player.UpdateVisibilityOf(iter->GetSource(), i_data, i_visibleNow);
//    }
//}
//
//// SEARCHERS & LIST SEARCHERS & WORKERS
//
//// WorldObject searchers & workers
//
//template<class Check>
//void WorldObjectSearcher<Check>::Visit(GameObjectMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
//        return;
//
//    // already found
//    if (i_object)
//        return;
//
//    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void WorldObjectSearcher<Check>::Visit(PlayerMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
//        return;
//
//    // already found
//    if (i_object)
//        return;
//
//    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void WorldObjectSearcher<Check>::Visit(CreatureMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
//        return;
//
//    // already found
//    if (i_object)
//        return;
//
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void WorldObjectSearcher<Check>::Visit(CorpseMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
//        return;
//
//    // already found
//    if (i_object)
//        return;
//
//    for (CorpseMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void WorldObjectSearcher<Check>::Visit(DynamicObjectMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
//        return;
//
//    // already found
//    if (i_object)
//        return;
//
//    for (DynamicObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//
//template<class Check>
//void WorldObjectLastSearcher<Check>::Visit(GameObjectMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
//        return;
//
//    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void WorldObjectLastSearcher<Check>::Visit(PlayerMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
//        return;
//
//    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void WorldObjectLastSearcher<Check>::Visit(CreatureMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
//        return;
//
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void WorldObjectLastSearcher<Check>::Visit(CorpseMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
//        return;
//
//    for (CorpseMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void WorldObjectLastSearcher<Check>::Visit(DynamicObjectMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
//        return;
//
//    for (DynamicObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void WorldObjectListSearcher<Check>::Visit(PlayerMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_PLAYER))
//        return;
//
//    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (i_check(itr->GetSource()))
//            Insert(itr->GetSource());
//}
//
//template<class Check>
//void WorldObjectListSearcher<Check>::Visit(CreatureMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CREATURE))
//        return;
//
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (i_check(itr->GetSource()))
//            Insert(itr->GetSource());
//}
//
//template<class Check>
//void WorldObjectListSearcher<Check>::Visit(CorpseMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_CORPSE))
//        return;
//
//    for (CorpseMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (i_check(itr->GetSource()))
//            Insert(itr->GetSource());
//}
//
//template<class Check>
//void WorldObjectListSearcher<Check>::Visit(GameObjectMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_GAMEOBJECT))
//        return;
//
//    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (i_check(itr->GetSource()))
//            Insert(itr->GetSource());
//}
//
//template<class Check>
//void WorldObjectListSearcher<Check>::Visit(DynamicObjectMapType &m)
//{
//    if (!(i_mapTypeMask & GRID_MAP_TYPE_MASK_DYNAMICOBJECT))
//        return;
//
//    for (DynamicObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (i_check(itr->GetSource()))
//            Insert(itr->GetSource());
//}
//
//// Gameobject searchers
//
//template<class Check>
//void GameObjectSearcher<Check>::Visit(GameObjectMapType &m)
//{
//    // already found
//    if (i_object)
//        return;
//
//    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void GameObjectLastSearcher<Check>::Visit(GameObjectMapType &m)
//{
//    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void GameObjectListSearcher<Check>::Visit(GameObjectMapType &m)
//{
//    for (GameObjectMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (itr->GetSource()->InSamePhase(i_phaseMask))
//            if (i_check(itr->GetSource()))
//                Insert(itr->GetSource());
//}
//
//// Unit searchers
//
//template<class Check>
//void UnitSearcher<Check>::Visit(CreatureMapType &m)
//{
//    // already found
//    if (i_object)
//        return;
//
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void UnitSearcher<Check>::Visit(PlayerMapType &m)
//{
//    // already found
//    if (i_object)
//        return;
//
//    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void UnitLastSearcher<Check>::Visit(CreatureMapType &m)
//{
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void UnitLastSearcher<Check>::Visit(PlayerMapType &m)
//{
//    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void UnitListSearcher<Check>::Visit(PlayerMapType &m)
//{
//    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (itr->GetSource()->InSamePhase(i_phaseMask))
//            if (i_check(itr->GetSource()))
//                Insert(itr->GetSource());
//}
//
//template<class Check>
//void UnitListSearcher<Check>::Visit(CreatureMapType &m)
//{
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (itr->GetSource()->InSamePhase(i_phaseMask))
//            if (i_check(itr->GetSource()))
//                Insert(itr->GetSource());
//}
//
//// Creature searchers
//
//template<class Check>
//void CreatureSearcher<Check>::Visit(CreatureMapType &m)
//{
//    // already found
//    if (i_object)
//        return;
//
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void CreatureLastSearcher<Check>::Visit(CreatureMapType &m)
//{
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Check>
//void CreatureListSearcher<Check>::Visit(CreatureMapType &m)
//{
//    for (CreatureMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (itr->GetSource()->InSamePhase(i_phaseMask))
//            if (i_check(itr->GetSource()))
//                Insert(itr->GetSource());
//}
//
//template<class Check>
//void PlayerListSearcher<Check>::Visit(PlayerMapType &m)
//{
//    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//        if (itr->GetSource()->InSamePhase(i_phaseMask))
//            if (i_check(itr->GetSource()))
//                Insert(itr->GetSource());
//}
//
//template<class Check>
//void PlayerSearcher<Check>::Visit(PlayerMapType &m)
//{
//    // already found
//    if (i_object)
//        return;
//
//    for (PlayerMapType::iterator itr=m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//        {
//            i_object = itr->GetSource();
//            return;
//        }
//    }
//}
//
//template<class Check>
//void PlayerLastSearcher<Check>::Visit(PlayerMapType& m)
//{
//    for (PlayerMapType::iterator itr = m.begin(); itr != m.end(); ++itr)
//    {
//        if (!itr->GetSource()->InSamePhase(i_phaseMask))
//            continue;
//
//        if (i_check(itr->GetSource()))
//            i_object = itr->GetSource();
//    }
//}
//
//template<class Builder>
//void LocalizedPacketDo<Builder>::operator()(Player* p)
//{
//    LocaleConstant loc_idx = p->GetSession()->GetSessionDbLocaleIndex();
//    uint32 cache_idx = loc_idx+1;
//    WorldPacket* data;
//
//    // create if not cached yet
//    if (i_data_cache.size() < cache_idx + 1 || !i_data_cache[cache_idx])
//    {
//        if (i_data_cache.size() < cache_idx + 1)
//            i_data_cache.resize(cache_idx + 1);
//
//        data = new WorldPacket();
//
//        i_builder(*data, loc_idx);
//
//        i_data_cache[cache_idx] = data;
//    }
//    else
//        data = i_data_cache[cache_idx];
//
//    p->SendDirectMessage(data);
//}
//
//template<class Builder>
//void LocalizedPacketListDo<Builder>::operator()(Player* p)
//{
//    LocaleConstant loc_idx = p->GetSession()->GetSessionDbLocaleIndex();
//    uint32 cache_idx = loc_idx+1;
//    WorldPacketList* data_list;
//
//    // create if not cached yet
//    if (i_data_cache.size() < cache_idx+1 || i_data_cache[cache_idx].empty())
//    {
//        if (i_data_cache.size() < cache_idx+1)
//            i_data_cache.resize(cache_idx+1);
//
//        data_list = &i_data_cache[cache_idx];
//
//        i_builder(*data_list, loc_idx);
//    }
//    else
//        data_list = &i_data_cache[cache_idx];
//
//    for (size_t i = 0; i < data_list->size(); ++i)
//        p->SendDirectMessage((*data_list)[i]);
//}
//
//}                                  
