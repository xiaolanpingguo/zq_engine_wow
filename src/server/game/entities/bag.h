#pragma once


#include "item.h"


namespace zq{


#define MAX_BAG_SIZE 36  

class Bag : public Item
{
public:
    Bag();
    ~Bag();


protected:

    Item* m_bagslot[MAX_BAG_SIZE];
};


}
