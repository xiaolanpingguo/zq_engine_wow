#pragma once


#include "object_registry.hpp"
#include "type_list.hpp"


namespace zq{

/** FactoryHolder holds a factory object of a specific type
 */
template<class T, class O, class Key = std::string>
class FactoryHolder
{
public:
    typedef ObjectRegistry<FactoryHolder<T, O, Key>, Key> FactoryHolderRegistry;

    explicit FactoryHolder(Key const& k) : _key(k) { }
    virtual ~FactoryHolder() { }

    void RegisterSelf() { FactoryHolderRegistry::instance()->InsertItem(this, _key); }

    /// Abstract Factory create method
    virtual T* Create(O* object = nullptr) const = 0;
private:
    Key const _key;
};

/** Permissible is a classic way of letting the object decide
 * whether how good they handle things.  This is not retricted
 * to factory selectors.
 */
template<class T>
class Permissible
{
public:
    virtual ~Permissible() { }
    virtual int32 Permit(T const*) const = 0;
};

}
