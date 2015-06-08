/*
 * Module: util
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_UTIL_EXCEPTION
#define COBRA_UTIL_EXCEPTION

#include <exception>
#include <string>

namespace CobraUtil {

class UtilException : public std::exception {
public:
    UtilException(const std::string& message, int code = 0)
        : m_message(message)
        , m_code(0)
    {
    }

    virtual ~UtilException() throw() { }


    virtual const char* what() const throw() { return m_message.c_str(); }

    virtual int code() const { return m_code; }

private:
    std::string m_message;
    int m_code;
};

}

#endif
