//#pragma once
//
//
//#include "grid_defines.h"
//#include "ngrid.h"
//
//
//namespace zq{
//
//class Map;
//class GridState
//{
//public:
//	virtual ~GridState() { };
//	virtual void Update(Map &, NGridType&, GridInfo &, uint32 t_diff) const = 0;
//};
//
//class InvalidState : public GridState
//{
//public:
//    void Update(Map &, NGridType &, GridInfo &, uint32 t_diff) const override;
//};
//
//class ActiveState : public GridState
//{
//public:
//    void Update(Map &, NGridType &, GridInfo &, uint32 t_diff) const override;
//};
//
//class IdleState : public GridState
//{
//public:
//	void Update(Map &, NGridType &, GridInfo &, uint32 t_diff) const override;
//};
//
//class RemovalState : public GridState
//{
//public:
//	void Update(Map &, NGridType &, GridInfo &, uint32 t_diff) const override;
//};
//
//
//}
