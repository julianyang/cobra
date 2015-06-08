/*
* Module: base
* Author: cloudyang@2015/03/28
* Copyright@cloudyang2015
*/

#ifndef _BASE_NONCOPYABLE_H_
#define _BASE_NONCOPYABLE_H_


namespace base {

#define BASE_MAKE_NONCOPYABLE(ClassName) \
    private: \
        ClassName(const ClassName &); \
        ClassName& operator=(const ClassName&); \

}

#endif
