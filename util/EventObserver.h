/* 
 * Moudule: util
 * Author: cloudyang@2015/04/17
 */

#ifndef COBRA_UTIL_EVENTOBSERVER
#define COBRA_UTIL_EVENTOBSERVER

#include <sys/epoll.h>

namespace CobraUtil {

struct Event {
    enum EventType {
        FD_EVENT_IN = 0x1,
        FD_EVENT_OUT = 0x2,
        FD_EVENT_HUP = 0x4,
        FD_EVENT_ERR = 0x8
    };

    Event() : m_fd(-1), m_events(0), m_data(0) { }

    int m_fd;
    int m_events;
    long long m_data;
};

class EventObserver {
public:
    EventObserver(bool useET = true);
    ~EventObserver();

    void create(int maxConnections = 1024);
    void addEvent(const Event& e) const;
    void modifyEvent(const Event& e) const;
    void removeEvent(const Event& e) const;
    int waitEvent(int millisecond) const;

    void getEvent(int index, Event& e) const;

private:
    bool m_useET;
    int m_epollFd;
    int m_maxConnect;
    struct epoll_event* m_events;

    static const int sMaxConnect = 1024;
};

}

#endif
