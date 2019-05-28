#pragma once


#include "../grids/cell.h"
#include "../entities/object/position.h"

namespace zq{

class Map;
class ObjectGridLoader;

enum MapObjectCellMoveState
{
    MAP_OBJECT_CELL_MOVE_NONE, //not in move list
    MAP_OBJECT_CELL_MOVE_ACTIVE, //in move list
    MAP_OBJECT_CELL_MOVE_INACTIVE, //in move list but should not move
};

class MapObject
{
    friend class Map; //map for moving creatures
    friend class ObjectGridLoader; //grid loader for loading creatures

protected:
    MapObject() : _moveState(MAP_OBJECT_CELL_MOVE_NONE)
    {
        _newPosition.Relocate(0.0f, 0.0f, 0.0f, 0.0f);
    }

private:
    Cell _currentCell;
    Cell const& GetCurrentCell() const { return _currentCell; }
    void SetCurrentCell(Cell const& cell) { _currentCell = cell; }

    MapObjectCellMoveState _moveState;
    Position _newPosition;
    void SetNewCellPosition(float x, float y, float z, float o)
    {
        _moveState = MAP_OBJECT_CELL_MOVE_ACTIVE;
        _newPosition.Relocate(x, y, z, o);
    }
};

}
