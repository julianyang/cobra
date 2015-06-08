/*
 * Module: util
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_UTIL_TOOLS
#define COBRA_UTIL_TOOLS

#include <sstream>
#include <string>
#include <stdlib.h>

namespace CobraUtil {

class UtilTools {
public:

    template<typename T>
    static std::string toString(const T& v) 
    {
        std::ostringstream os;
        os << v;
        return os.str();
    }

    template<typename T>
    static T stringTo(const std::string& v);

};

namespace p
{

template<typename D>
struct strto1
{
    D operator()(const std::string &sStr)
    {
        std::string s = "0";

        if(!sStr.empty())
        {
            s = sStr;
        }

        std::istringstream sBuffer(s);

        D t;
        sBuffer >> t;

        return t;
    }
};

template<>
struct strto1<char>
{
    char operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return sStr[0];
        }
        return 0;
    }
};

template<>
struct strto1<short>
{
    short operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return atoi(sStr.c_str());
        }
        return 0;
    }
};

template<>
struct strto1<unsigned short>
{
    unsigned short operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return strtoul(sStr.c_str(), NULL, 10);
        }
        return 0;
    }
};

template<>
struct strto1<int>
{
    int operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return atoi(sStr.c_str());
        }
        return 0;
    }
};

template<>
struct strto1<unsigned int>
{
    unsigned int operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return strtoul(sStr.c_str(), NULL, 10);
        }
        return 0;
    }
};

template<>
struct strto1<long>
{
    long operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return atol(sStr.c_str());
        }
        return 0;
    }
};

template<>
struct strto1<long long>
{
    long long operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return atoll(sStr.c_str());
        }
        return 0;
    }
};

template<>
struct strto1<unsigned long>
{
    unsigned long operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return strtoul(sStr.c_str(), NULL, 10);
        }
        return 0;
    }
};

template<>
struct strto1<float>
{
    float operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return atof(sStr.c_str());
        }
        return 0;
    }
};

template<>
struct strto1<double>
{
    double operator()(const std::string &sStr)
    {
        if(!sStr.empty())
        {
            return atof(sStr.c_str());
        }
        return 0;
    }
};

template<typename D>
struct strto2
{
    D operator()(const std::string &sStr)
    {
        std::istringstream sBuffer(sStr);

        D t;
        sBuffer >> t;

        return t;
    }
};

template<>
struct strto2<std::string>
{
    std::string operator()(const std::string &sStr)
    {
        return sStr;
    }
};

template<bool flag, typename U, typename V>
struct TypeSelect
{
    typedef U Result;
};

template<typename U, typename V>
struct TypeSelect<false, U, V>
{
    typedef V Result;
};

// type list
class NullType;

//空类型
struct EmptyType { };

template<typename Head, typename Tail>
struct TypeList
{
    typedef Head H;
    typedef Tail T;
};

#define TYPELIST_1(T1) TypeList<T1, NullType>
#define TYPELIST_2(T1, T2) TypeList<T1, TYPELIST_1(T2)>
#define TYPELIST_3(T1, T2, T3) TypeList<T1, TYPELIST_2(T2, T3)>
#define TYPELIST_4(T1, T2, T3, T4) TypeList<T1, TYPELIST_3(T2, T3, T4)>
#define TYPELIST_5(T1, T2, T3, T4, T5) TypeList<T1, TYPELIST_4(T2, T3, T4, T5)>
#define TYPELIST_6(T1, T2, T3, T4, T5, T6) TypeList<T1, TYPELIST_5(T2, T3, T4, T5, T6)>
#define TYPELIST_7(T1, T2, T3, T4, T5, T6, T7) TypeList<T1, TYPELIST_6(T2, T3, T4, T5, T6, T7)>
#define TYPELIST_8(T1, T2, T3, T4, T5, T6, T7, T8) TypeList<T1, TYPELIST_7(T2, T3, T4, T5, T6, T7, T8)>
#define TYPELIST_9(T1, T2, T3, T4, T5, T6, T7, T8, T9) TypeList<T1, TYPELIST_8(T2, T3, T4, T5, T6, T7, T8, T9)>
#define TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) TypeList<T1, TYPELIST_9(T2, T3, T4, T5, T6, T7, T8, T9, T10)>

/**
 * * Length: 取TypeList的长度
 * */
template<class TList> struct Length;
template<> struct Length<NullType>
{
    enum { value = 0 };
};

template<class Head, class Tail> struct Length<TypeList<Head, Tail> >
{
    enum { value = 1 + Length<Tail>::value };
};

/**
 * * TypeAt, 取链表在i位置上的类型
 * */
template<class TList, unsigned int i> struct TypeAt;
template<class Head, class Tail> struct TypeAt<TypeList<Head, Tail>, 0>
{
    typedef Head Result;
};

template<class Head, class Tail, unsigned int i> struct TypeAt<TypeList<Head, Tail>, i>
{
    typedef typename TypeAt<Tail, i-1>::Result Result;
};

/**
 * * TypeAt, 取链表在i位置上的类型, i超出了返回, 则返回DefaultType
 * */
template<class TList, unsigned int index, typename DefaultType = NullType> struct TypeAtNonStrict
{
    typedef DefaultType Result;
};

template <class Head, class Tail, typename DefaultType> struct TypeAtNonStrict<TypeList<Head, Tail>, 0, DefaultType>
{
    typedef Head Result;
};

template <class Head, class Tail, unsigned int i, typename DefaultType> struct TypeAtNonStrict<TypeList<Head, Tail>, i, DefaultType>
{
    typedef typename TypeAtNonStrict<Tail, i - 1, DefaultType>::Result Result;
};

/**
 * * 取链表上类型为T的序号, 没有则返回-1
 * */
template<class TList, class T> struct IndexOf;
template<class T> struct IndexOf<NullType, T>
{
    enum { value = -1 };
};

template<class Tail, class T> struct IndexOf<TypeList<T,Tail>, T>
{
    enum { value = 0 };
};

template<class Head, class Tail, class T> struct IndexOf<TypeList<Head, Tail>, T>
{
    private:
        enum { temp = IndexOf<Tail, T>::value };
    public:
        enum { value = temp == -1 ? -1 : 1 + temp };
};

/**
 * * Append, 添加到链表尾部
 * */
template<class TList, class T> struct Append;
template<> struct Append<NullType, NullType>
{
    typedef NullType Result;
};

template<class T> struct Append<NullType,T>
{
    typedef TYPELIST_1(T) Result;
};

template<class Head, class Tail> struct Append<NullType, TypeList<Head, Tail> >
{
    typedef TypeList<Head, Tail> Result;
};

template<class Head, class Tail, class T> struct Append<TypeList<Head, Tail>, T>
{
    typedef TypeList<Head, typename Append<Tail, T>::Result> Result;
};


/**
 * * Erase 删除
 * */
template<class TList, class T> struct Erase;
template<class T> struct Erase<NullType, T>
{
    typedef NullType Result;
};

template<class T, class Tail> struct Erase<TypeList<T, Tail>, T>
{
    typedef Tail Result;
};

template<class Head, class Tail, class T> struct Erase<TypeList<Head, Tail>, T>
{
    typedef TypeList<Head, typename Erase<Tail, T>::Result> Result;
};

/**
 * * EraseAll 删除
 * */
template<class TList, class T> struct EraseAll;
template<class T> struct EraseAll<NullType, T>
{
    typedef NullType Result;
};

template<class T, class Tail> struct EraseAll<TypeList<T, Tail>, T>
{
    typedef typename EraseAll<Tail, T>::Result Result;
};

template<class Head, class Tail, class T> struct EraseAll<TypeList<Head, Tail>, T>
{
    typedef TypeList<Head, typename EraseAll<Tail, T>::Result> Result;
};


/**
 * * 生成TypeList类型
 * */
template<class T1=NullType,  class T2=NullType,  class T3=NullType,  class T4=NullType,  class T5=NullType,
    class T6=NullType,  class T7=NullType,  class T8=NullType,  class T9=NullType,  class T10=NullType>
    struct TLMaker {
private:
    typedef TYPELIST_10(T1, T2, T3, T4, T5, T6, T7, T8, T9, T10) TmpList;
public:
    typedef typename EraseAll<TmpList, NullType>::Result Result;
};

template<typename T>
class TypeTraits {
private:

    template<class U>
    struct ReferenceTraits
    {
        enum { result = false };
        typedef U Result;
    };

    template<class U>
    struct ReferenceTraits<U&>
    {
        enum { result = true };
        typedef U Result;
    };

    template<class U>
    struct PointerTraits
    {
        enum { result = false };
        typedef NullType Result;
    };

    template<class U>
    struct PointerTraits<U*>
    {
        enum { result = true };
        typedef U Result;
    };

    template<class U>
    struct PointerTraits<U*&>
    {
        enum { result = true };
        typedef U Result;
    };

    template<typename U>
    struct PointerToMemberTraits
    {
        enum { result = false };
    };

    template<class U, class V>
    struct PointerToMemberTraits<U V::*>
    {
        enum { result = true };
    };

    template<class U, class V>
    struct PointerToMemberTraits<U V::*&>
    {
        enum { result = true };
    };

    template<typename U>
    struct UnConstTraits
    {
        enum { result = false };
        typedef U Result;
    };
    template<typename U>
    struct UnConstTraits<const U>
    {
        enum { result = true };
        typedef U Result;
    };
    template<typename U>
    struct UnConstTraits<const U&>
    {
        enum { result = true };
        typedef U& Result;
    };


    template<typename U>
    struct UnVolatileTraits
    {
        enum { result = false };
        typedef U Result;
    };
    template<typename U>
    struct UnVolatileTraits<volatile U>
    {
        enum { result = true };
        typedef U Result;
    };
    template<typename U>
    struct UnVolatileTraits<volatile U&>
    {
        enum { result = true };
        typedef U& Result;
    };
public:

    enum { isPointer        = PointerTraits<T>::result };

    enum { isReference      = ReferenceTraits<T>::result };

    enum { isMemberPointer  = PointerToMemberTraits<T>::result };

    enum { isConst          = UnConstTraits<T>::result };

    enum { isVolatile       = UnVolatileTraits<T>::result };

    typedef typename PointerTraits<T>::Result           PointeeType;

    typedef typename ReferenceTraits<T>::Result         ReferencedType;

    typedef typename UnConstTraits<T>::Result           NonConstType;

    typedef typename UnVolatileTraits<T>::Result        NonVolatileType;

    typedef typename UnVolatileTraits<typename UnConstTraits<T>::Result>::Result UnqualifiedType;

public:

    typedef TLMaker<unsigned char, unsigned short, unsigned int, unsigned long, unsigned long long>::Result UnsignedInts;
    typedef TLMaker<signed char, short, int, long, long long>::Result SignedInts;
    typedef TLMaker<bool, char, wchar_t>::Result OtherInts;
    typedef TLMaker<float, double, long double>::Result Floats;
    typedef TYPELIST_2(EmptyType, NullType) NullTypes;

    enum { isStdUnsignedInt = IndexOf<UnsignedInts, T>::value >= 0 };

    enum { isStdSignedInt   = IndexOf<SignedInts, T>::value >= 0 };

    enum { isStdInt         = isStdUnsignedInt || isStdSignedInt || IndexOf<OtherInts, T>::value >= 0 };

    enum { isStdFloat       = IndexOf<Floats, T>::value >= 0 };

    enum { isStdArith       = isStdInt || isStdFloat };

    enum { isStdFundamental = isStdArith || IndexOf<TYPELIST_1(void), T>::value >= 0};

    enum { isNullType       = IndexOf<NullTypes, T>::value >= 0 };

    enum { isBaseType       = isStdArith || isPointer || isMemberPointer };

    typedef typename TypeSelect<isBaseType, T, ReferencedType&>::Result ReferenceType;

    typedef typename TypeSelect<!isReference, T, ReferencedType&>::Result ParameterType;
};

} // namespace p

template <typename T>
T UtilTools::stringTo(const std::string& v) 
{
    typedef typename p::TypeSelect<p::TypeTraits<T>::isStdArith, p::strto1<T>, p::strto2<T> >::Result strto_type;

    return strto_type()(v);
}


} // namespace CobraUtil


#endif
