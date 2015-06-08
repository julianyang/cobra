/*
* Module: base
* Author: cloudyang@2015/04/02
* Copyright@cloudyang2015
*/

#ifndef _BASE_THREAD_LOCAL_H_
#define _BASE_THREAD_LOCAL_H_

#include "base/BuildConfig.h"
#include "base/Noncopyable.h"

#if defined(OS_POSIX)
#include <pthread.h>
#endif

namespace base {
namespace internal {

struct ThreadLocalPlatform {
#if defined(OS_WIN)
    typedef unsigned long SlotType;
#elif defined(OS_POSIX)
    typedef pthread_key_t SlotType;
#endif

    static void allocateSlot(SlotType& slot);
    static void freeSlot(SlotType& slot);
    static void* getValueFromSlot(SlotType& slot);
    static void setValueInSlot(SlotType& slot, void* value);
};

} // namespace internal

template<typename T>
class ThreadLocalPointer {
    BASE_MAKE_NONCOPYABLE(ThreadLocalPointer);
public:
    ThreadLocalPointer() 
        : m_slot() 
    {
        internal::ThreadLocalPlatform::allocateSlot(m_slot);
    }

    ~ThreadLocalPointer()
    {
        internal::ThreadLocalPlatform::freeSlot(m_slot);
    }

    T* get()
    {
        return static_cast<T*>(internal::ThreadLocalPlatform::getValueFromSlot(m_slot));
    }

    void set(T* ptr)
    {
        internal::ThreadLocalPlatform::setValueInSlot(m_slot, const_cast<void*>(static_cast<const void*>(ptr)));
    }

private:
    internal::ThreadLocalPlatform::SlotType m_slot;
};

class ThreadLocalBoolean {
    BASE_MAKE_NONCOPYABLE(ThreadLocalBoolean);
public:
    ThreadLocalBoolean() { }
    ~ThreadLocalBoolean() { }

    bool get()
    {
        return m_tlp.get() != NULL;
    }

    void set(bool val)
    {   
        m_tlp.set(val ? this : NULL);
    }

private:
    ThreadLocalPointer<void> m_tlp;
};

}

#endif
