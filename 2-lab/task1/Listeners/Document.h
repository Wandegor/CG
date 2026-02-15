#pragma once
#include <string>
#include <unordered_set>

#include "IDocumentListener.h"

class Document
{
    std::unordered_map<std::string, std::unordered_set<IDocumentListener*>> m_listeners;

public:
    void Subscribe(const std::string &eventType, IDocumentListener& listener)
    {
        m_listeners[eventType].insert(&listener);
    }

    void Unsubscribe(const std::string &eventType, IDocumentListener& listener)
    {
        auto it = m_listeners.find(eventType);
        if (it != m_listeners.end())
        {
            it->second.erase(&listener);
        }
    }

    void NotifyListeners(const std::string &eventType, void *data = nullptr)
    {
        if (const auto it = m_listeners.find(eventType); it != m_listeners.end())
        {
            for (auto* listener : it->second)
            {
                listener->Update(eventType, data);
            }
        }
    }
};