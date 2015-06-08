/*
 * Module: util
 * Author: cloudyang@2015/04/17
 */

#include "util/EventObserver.h"

#include <unistd.h>

namespace CobraUtil {

namespace {
    
inline int getEpollEvent(int event)
{
    int ret = 0;
    if (event & Event::FD_EVENT_IN)
        ret |= EPOLLIN;
    if (event & Event::FD_EVENT_OUT)
        ret |= EPOLLOUT;
    if (event & Event::FD_EVENT_HUP)
        ret |= EPOLLHUP;
    if (event & Event::FD_EVENT_ERR)
        ret |= EPOLLERR;

    return ret;
}

inline int getObserverEvent(int event)
{
    int ret = 0;
    if (event & EPOLLIN)
        ret |= Event::FD_EVENT_IN;
    if (event & EPOLLOUT)
        ret |= Event::FD_EVENT_OUT;
    if (event & EPOLLHUP)
        ret |= Event::FD_EVENT_HUP;
    if (event & EPOLLERR)
        ret |= Event::FD_EVENT_ERR;

    return ret;
}

}

EventObserver::EventObserver(bool useET)
    : m_useET(useET)
    , m_epollFd(-1) 
    , m_maxConnect(sMaxConnect)
    , m_events(NULL)
{
    
}

EventObserver::~EventObserver()
{
    if (m_events)
        delete [] m_events;
    ::close(m_epollFd);
}

void EventObserver::create(int maxConnections) 
{
    m_epollFd = epoll_create(m_maxConnect+1);
    if (m_events)
        delete [] m_events;

    m_events = new epoll_event[m_maxConnect + 1];
}

void EventObserver::addEvent(const Event &e) const
{
    struct epoll_event ev;
    ev.data.u64 = e.m_data;
    ev.events = getEpollEvent(e.m_events);

    if (m_useET)
        ev.events |= EPOLLET;

    epoll_ctl(m_epollFd, EPOLL_CTL_ADD, e.m_fd, &ev);
}

void EventObserver::modifyEvent(const Event &e) const
{
    struct epoll_event ev;
    ev.data.u64 = e.m_data;
    ev.events = getEpollEvent(e.m_events);

    if (m_useET)
        ev.events |= EPOLLET;

    epoll_ctl(m_epollFd, EPOLL_CTL_MOD, e.m_fd, &ev);
}

void EventObserver::removeEvent(const Event &e) const
{
    struct epoll_event ev;
    ev.data.u64 = e.m_data;
    ev.events = getEpollEvent(e.m_events);

    if (m_useET)
        ev.events |= EPOLLET;

    epoll_ctl(m_epollFd, EPOLL_CTL_DEL, e.m_fd, &ev);
}

int EventObserver::waitEvent(int ms) const
{
    return epoll_wait(m_epollFd, m_events, m_maxConnect+1, ms); 
}

void EventObserver::getEvent(const int index, Event& ev) const
{
    epoll_event *event = m_events+index;
    ev.m_fd = -1; // invalid fd.
    ev.m_data = event->data.u64;
    ev.m_events = getObserverEvent(event->events);
}

}
