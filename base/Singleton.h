/*
* Module: base
* Author: cloudyang@2015/04/05
* Copyright@cloudyang2015
*/

#ifndef _BASE_SINGLETON_H_
#define _BASE_SINGLETON_H_

#include "base/Noncopyable.h"
#include "base/Lock.h"

namespace base {

template<typename T>
class CreateAuto {
public:
    static T* create() 
    {
        return new T;
    }

    static void destroy(T* t)
    {
        delete t;
    }
};

template<typename T>
class CreateStatic {
public:
    static T* create()
    {
        char buffer[sizeof(T)];
        return new(&buffer) T;
    }

    static void destroy(T* t)
    {
        t->~T();
    }
};

template<typename T, typename C = CreateAuto<T> >
class Singleton {
    BASE_MAKE_NONCOPYABLE(Singleton);
public:
    static T* getInstance() 
    {
        if (m_instance)
            return static_cast<T*>(m_instance);

        m_instance = C::create();
            return static_cast<T*>(m_instance);
    }

protected:
    Singleton() { }

    virtual ~Singleton() 
    {
        C::destroy(static_cast<T*>(m_instance));
    }

private:
    static Lock m_instanceLock;
    static T* m_instance;
};

template<class T, typename CreatePolicy> Lock Singleton<T, CreatePolicy>::m_instanceLock;
template<class T, typename CreatePolicy> T* Singleton<T, CreatePolicy>::m_instance = NULL;

}

#endif
