/*
* Module: base
* Author: cloudyang@2015/04/04
* Copyright@cloudyang2015
*/

#ifndef _BASE_LOGGER_H_
#define _BASE_LOGGER_H_

#include <iostream>
#include <string>

namespace base {

class LoggerStream {
public:
    LoggerStream(bool isOut, std::string& header)
        : m_isOut(isOut)
        , m_header(header)
    {
        if (m_isOut)
            std::cout << header;
    }

    ~LoggerStream()
    {
        if (m_isOut)
            std::cout.flush();
    }

    template<typename P>
    LoggerStream& operator<<(const P &t)
    {
        if (m_isOut)
            std::cout << t;
        return *this;
    }

    typedef std::ostream &(*F)(std::ostream& os);
    LoggerStream& operator<<(F f)
    {
        if (m_isOut)
            (f)(std::cout);

        return *this;
    }

    LoggerStream(const LoggerStream& o) 
    {
        if (this == &o)
            return;

        if (m_isOut)
            std::cout.flush();
        
        m_isOut = o.m_isOut;
        m_header = o.m_header;
    }

    LoggerStream& operator=(const LoggerStream &o)
    {
        if (this == &o)
            return *this;

        if (m_isOut)
            std::cout.flush();
        
        m_isOut = o.m_isOut;
        m_header = o.m_header;
        return *this;
    }

private:
    bool m_isOut;
    std::string m_header;
};

class Logger {
public:
    enum LoogerLevel {
        E_LOG_ERROR,
        E_LOG_WARN,
        E_LOG_DEBUG,
        E_LOG_INFO
    };

    Logger() : m_level(E_LOG_DEBUG) { }
    Logger(int level) : m_level(level) { }
    virtual ~Logger() { }

    virtual void setLoggerLevel(int level) 
    {
        if (isInvalidLevel(level))
            m_level = level;
    }

    virtual LoggerStream error() const { return getOutputStream(E_LOG_ERROR); }
    virtual LoggerStream warn() const { return getOutputStream(E_LOG_WARN); }
    virtual LoggerStream debug() const { return getOutputStream(E_LOG_DEBUG); }
    virtual LoggerStream info() const { return getOutputStream(E_LOG_INFO); }

private:
    bool isInvalidLevel(int level) { return (level <= E_LOG_INFO && level >= E_LOG_ERROR); }
    void constructHeader(char* header, int level) const;
    LoggerStream getOutputStream(int level) const;

    volatile int m_level;
};

}

#endif
