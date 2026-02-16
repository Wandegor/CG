#pragma once
#include <string>
#include <unordered_set>

#include "IEventListener.h"

class EventManager
{
    std::unordered_map<std::string, std::unordered_set<IEventListener *> > m_listeners;

public:
    void Subscribe(const std::string &eventType, IEventListener &listener)
    {
        m_listeners[eventType].insert(&listener);
    }

    void Unsubscribe(const std::string &eventType, IEventListener &listener)
    {
        auto it = m_listeners.find(eventType);
        if (it != m_listeners.end())
        {
            it->second.erase(&listener);
            if (it->second.empty())
            {
                m_listeners.erase(it);
            }
        }
    }

    void NotifyListeners(const std::string &eventType, const sf::Event &event)
    {
        if (const auto it = m_listeners.find(eventType); it != m_listeners.end())
        {
            for (auto *listener: it->second)
            {
                listener->Update(eventType, event);
            }
        }
    }
};
