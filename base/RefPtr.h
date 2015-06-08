/*
* Module: base
* Author: cloudyang@2015/03/28
* Copyright@cloudyang2015
*/

#ifndef _BASE_REFPTR_H_
#define _BASE_REFPTR_H_

#include <algorithm>
#include "base/PtrBase.h"

namespace base {

template<typename T> class RefPtr {
public:
    typedef T ValueType;
    typedef T* ValueTypePtr;

    RefPtr() : m_ptr(NULL) { }
    RefPtr(T* ptr) : m_ptr(ptr) { refIfNotNull(m_ptr); }
    RefPtr(const RefPtr<T>& o) : m_ptr(o.get()) { refIfNotNull(m_ptr); }
    template<typename U> RefPtr(const RefPtr<U>& o) : m_ptr(o.get()) { refIfNotNull(m_ptr); }

    ~RefPtr() { derefIfNotNull(m_ptr); }

    T* get() const { return m_ptr; }
    void clear();

    T* release() { T* tmp = m_ptr; m_ptr = NULL; return tmp; }
    T& operator*() const { return *m_ptr; }
    T* operator->() const { return m_ptr; }
    bool operator!() const { return !m_ptr; }

    // This conversion operator allows implicit conversion to bool but not to other integer types.
    typedef T* RefPtr::*UnspecifiedBoolType;
    operator UnspecifiedBoolType() const { return m_ptr ? &RefPtr::m_ptr : NULL; }

    RefPtr& operator=(const RefPtr&);
    RefPtr& operator=(T*);
    template<typename U> RefPtr& operator=(const RefPtr<U>&);
    template<typename U> RefPtr& operator=(U*);
    
    void swap(RefPtr&);

private:
    ValueTypePtr m_ptr;
};

template<typename T> inline void RefPtr<T>::clear()
{
    T* ptr = m_ptr;
    m_ptr = NULL;
    derefIfNotNull(ptr);
}

template<typename T> inline RefPtr<T>& RefPtr<T>::operator=(const RefPtr& o)
{
    RefPtr ptr = o;
    swap(ptr);
    return *this;
}

template<typename T> inline RefPtr<T>& RefPtr<T>::operator=(T* p)
{
    RefPtr ptr = p;
    swap(ptr);
    return *this;
}

template<typename T> template<typename U> inline RefPtr<T>& RefPtr<T>::operator=(const RefPtr<U>& o)
{
    RefPtr ptr = o;
    swap(ptr);
    return *this;
}

template<typename T> template<typename U> inline RefPtr<T>& RefPtr<T>::operator=(U* p)
{
    RefPtr ptr = p;
    swap(ptr);
    return *this;
}

template<typename T> inline void RefPtr<T>::swap(RefPtr& o)
{
    std::swap(m_ptr, o.m_ptr);
}

template<typename T> inline void swap(RefPtr<T>& a, RefPtr<T>& b)
{
    a.swap(b);
}

template<typename T, typename U> inline bool operator==(const RefPtr<T>& a, const RefPtr<U>& b)
{
    return a.get() == b.get();
}

template<typename T, typename U> inline bool operator==(const RefPtr<T>& a, const U* b)
{
    return a.get() == b;
}

template<typename T, typename U> inline bool operator==(const T* a, const RefPtr<U>& b)
{
    return a == b.get();
}

template<typename T, typename U> inline bool operator!=(const RefPtr<T>& a, const RefPtr<U>& b)
{
    return a.get() != b.get();
}

template<typename T, typename U> inline bool operator!=(const RefPtr<T>& a, const U* b)
{
    return a.get() != b;
}

template<typename T, typename U> inline bool operator!=(const T* a, const RefPtr<U>& b)
{
    return a != b.get();
}

template<typename T, typename U> inline RefPtr<T> static_pointer_cast(const RefPtr<U>& p)
{
    return RefPtr<T>(static_cast<T*>(p.get()));
}

}

#endif
