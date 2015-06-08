/*
* Module: base
* Author: cloudyang@2015/03/28
* Copyright@cloudyang2015
*/

#ifndef _BASE_OWNPTR_H_
#define _BASE_OWNPTR_H_

#include <assert.h>
#include <algorithm>
#include "base/PtrBase.h"

namespace base {

template<typename T> class OwnPtr {
public:
    typedef T ValueType;
    typedef T * ValueTypePtr;

    OwnPtr() : m_ptr(NULL) { }
    OwnPtr(T *p) : m_ptr(p) { }
    template<typename U> OwnPtr(U*);

    ~OwnPtr() { deleteOwnedPtr(m_ptr); }

    ValueTypePtr get() const { return m_ptr; }

    void clear();

    ValueType& operator*() const { assert(m_ptr); return *m_ptr; }
    ValueTypePtr operator->() const { assert(m_ptr); return m_ptr; }
    bool operator!() const { return m_ptr; }

    // This conversion operator allows implicit conversion to bool but no to other integer types.
    typedef ValueTypePtr OwnPtr::*UnspecifiedBoolType;
    operator UnspecifiedBoolType() const { return m_ptr ? &OwnPtr::m_ptr : 0; }
    
    OwnPtr& operator=(OwnPtr&);
    template<typename U> OwnPtr& operator=(OwnPtr<U>&);
    void swap(OwnPtr& o) { std::swap(m_ptr, o.m_ptr); }

private:
    // We should never have two OwnPtrs for the same underlying object.
    template<typename U> bool operator==(const OwnPtr<U>&);
    template<typename U> bool operator!=(const OwnPtr<U>&);
    template<typename U> bool operator==(const U*);
    template<typename U> bool operator!=(const U*);

    OwnPtr& operator=(const T*);
    template<typename U> OwnPtr& operator=(const U*);

    ValueTypePtr m_ptr;
};

template<typename T> template<typename U> inline OwnPtr<T>::OwnPtr(U* p) 
    : m_ptr(static_cast<T*>(p))
{
    
}

template<typename T> inline void OwnPtr<T>::clear()
{
    ValueTypePtr ptr = m_ptr;
    m_ptr = 0;
    if (ptr)
        delete ptr;
}

template<typename T> inline OwnPtr<T>& OwnPtr<T>::operator=(OwnPtr<T>& o)
{
    if (this == &o)
        return *this;

    ValueTypePtr ptr = m_ptr;
    m_ptr = o.m_ptr;
    o.m_ptr = NULL;
    deleteOwnedPtr(ptr);
    return *this;
}

template<typename T> template<typename U> inline OwnPtr<T>& OwnPtr<T>::operator=(OwnPtr<U>& o)
{
    ValueTypePtr ptr = m_ptr;
    m_ptr = static_cast<T*>(o.m_ptr);
    o.m_ptr = NULL;
    deleteOwnedPtr(ptr);
    return *this;
}

template<typename T> inline void swap(OwnPtr<T>& a, OwnPtr<T>& b)
{
    a.swap(b);
}

template<typename T, typename U> inline bool operator==(const OwnPtr<T>&a, U* b)
{
    return a.get() == b;
}

template<typename T, typename U> inline bool operator==(T* a, const OwnPtr<U>& b)
{
    return a == b.get();
}

template<typename T, typename U> inline bool operator!=(const OwnPtr<T>&a, U* b)
{
    return a.get() != b;
}

template<typename T, typename U> inline bool operator!=(T* a, const OwnPtr<U>& b)
{
    return a != b.get();
}
}

#endif
