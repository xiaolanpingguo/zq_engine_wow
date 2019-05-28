#pragma once


#include "../common/reference.hpp"

namespace zq{

template<class OBJECT>
class GridRefManager;

template<class OBJECT>
class GridReference : public Reference<GridRefManager<OBJECT>, OBJECT>
{
protected:
    void targetObjectBuildLink() override
    {
        // called from link()
        this->getTarget()->insertFirst(this);
        this->getTarget()->incSize();
    }
    void targetObjectDestroyLink() override
    {
        // called from unlink()
        if (this->isValid()) this->getTarget()->decSize();
    }
    void sourceObjectDestroyLink() override
    {
        // called from invalidate()
        this->getTarget()->decSize();
    }
public:
    GridReference() : Reference<GridRefManager<OBJECT>, OBJECT>() { }
    ~GridReference() { this->unlink(); }
    GridReference* next() { return (GridReference*)Reference<GridRefManager<OBJECT>, OBJECT>::next(); }
};

}
