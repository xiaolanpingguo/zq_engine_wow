#pragma once


#include "interface_header/base/platform.h"
#include <iterator>


namespace zq{


template <class T>
class DBCStorageIterator : public std::iterator<std::forward_iterator_tag, T>
{
public:
    DBCStorageIterator() : _index(nullptr), _pos(0), _end(0) { }
    DBCStorageIterator(T** index, uint32 size, uint32 pos = 0) : _index(index), _pos(pos), _end(size)
    {
        if (_pos < _end)
        {
            while (_pos < _end && !_index[_pos])
                ++_pos;
        }
    }

    T const* operator->() { return _index[_pos]; }
    T const* operator*() { return _index[_pos]; }

    bool operator==(DBCStorageIterator const& right) const { /*ASSERT(_index == right._index, "Iterator belongs to a different container")*/ return _pos == right._pos; }
    bool operator!=(DBCStorageIterator const& right) const { return !(*this == right); }

    DBCStorageIterator& operator++()
    {
        if (_pos < _end)
        {
            do
                ++_pos;
            while (_pos < _end && !_index[_pos]);
        }

        return *this;
    }

    DBCStorageIterator operator++(int)
    {
        DBCStorageIterator tmp = *this;
        ++*this;
        return tmp;
    }

private:
    T** _index;
    uint32 _pos;
    uint32 _end;
};

}