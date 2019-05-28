//#include "grid_states.h"
//#include "grid_notifiers.h"
//#include "../maps/map.h"
//#include "object_grid_loader.h"
//
//namespace zq{
//
//void InvalidState::Update(Map&, NGridType&, GridInfo&, uint32) const
//{
//}
//
//void ActiveState::Update(Map& map, NGridType& grid, GridInfo&  info, uint32 diff) const
//{
//    // Only check grid activity every (grid_expiry/10) ms, because it's really useless to do it every cycle
//    info.UpdateTimeTracker(diff);
//    if (info.getTimeTracker().Passed())
//    {
//        if (!grid.GetWorldObjectCountInNGrid<Player>() && !map.ActiveObjectsNearGrid(grid))
//        {
//            ObjectGridStoper worker;
//            TypeContainerVisitor<ObjectGridStoper, GridTypeMapContainer> visitor(worker);
//            grid.VisitAllGrids(visitor);
//            grid.SetGridState(GRID_STATE_IDLE);
//        }
//        else
//            map.ResetGridExpiry(grid, 0.1f);
//    }
//}
//
//void IdleState::Update(Map& map, NGridType& grid, GridInfo&, uint32) const
//{
//    map.ResetGridExpiry(grid);
//    grid.SetGridState(GRID_STATE_REMOVAL);
//}
//
//void RemovalState::Update(Map& map, NGridType& grid, GridInfo& info, uint32 diff) const
//{
//    if (!info.getUnloadLock())
//    {
//        info.UpdateTimeTracker(diff);
//        if (info.getTimeTracker().Passed() && !map.UnloadGrid(grid, false))
//        {
//            map.ResetGridExpiry(grid);
//        }
//    }
//}
//
//}
