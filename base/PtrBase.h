/*
* Module: base
* Author: cloudyang@2015/03/28
* Copyright@cloudyang2015
*/

#ifndef _BASE_PTR_BASE_H_
#define _BASE_PTR_BASE_H_


namespace base {

template<typename T> void deleteOwnedPtr(T* p)
{
    typedef char know[sizeof(T) ? 1 : -1];
    if (sizeof(know))
        delete p;
}

template<typename T> inline void refIfNotNull(T* p)
{
    if (p)
        p->ref();
}

template<typename T> inline void derefIfNotNull(T* p)
{
    if (p)
        p->deref();
}

}

#endif
