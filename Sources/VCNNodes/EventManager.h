///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#pragma once

#include "Event.h"

#include "VCNUtils\Assert.h"
#include "VCNUtils\Chrono.h"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

class VCNNode;
class VCNIEvent;

class VCNEventManager
{
public:
    typedef std::shared_ptr<VCNEventManager> Handle;
    typedef std::function<void (VCNNode*)> NodeFunction;

    static Handle Create();

    ~VCNEventManager();

    /* Registers a member function callback to an event
    - if track : the object will return a handle that will unregister this callback when it is destroyed
        (YOU MUST KEEP A COPY OF IT, or you will immediately be unregistered!)

    - if !track : it is the callers responsibility to call UnRegisterListener
    */
    template<typename EventType, typename ListenerType>
    VCNIEvent::Handle RegisterListener(ListenerType* listener, void (ListenerType::*callback)(const EventType&), bool track = true);

    /* Registers a function object to an event
    the function will stay registered for the lifespan of the returned Handle
    */
    template<typename EventType>
    VCNIEvent::Handle RegisterListener(const std::function<void (const EventType&)>& callback);

    //Unregisters a listener from all the untracked events he is currently registered to
    void UnRegisterListener(VCNIListener* listener);

    //Fires a specific event to all listeners
    template<typename EventType>
    void Fire(const EventType& evt) const;

    //pass function to this to call it at the beginning of the next frame
    void InvokeLater(const NodeFunction& func);

    void FireInvokedFunctions(VCNNode* target);

    std::size_t GetEventCount() const;
    std::size_t GetCallbackCount() const;

private:
    VCNEventManager();

    friend void UnregisterOnHandleDestroyed(std::weak_ptr<VCNEventManager> handle, VCNIEvent* eventHandle);

    typedef std::weak_ptr< VCNIEvent::Handle::element_type > WeakHandle;
    typedef std::vector< VCNIEvent::Handle > ListenerRefContainer;
    typedef std::vector< WeakHandle > ListenerContainer;
    typedef std::unordered_map<std::string, ListenerContainer> EventMap;
    typedef std::pair<const std::string, ListenerContainer> EventPair;

    typedef std::vector< NodeFunction > InvokeContainer;

    VCNEventManager(VCNEventManager& other)
    {} //no copying

    void RemoveTargetFromContainer(ListenerContainer& container, VCNIComponent* target);

    EventMap m_EventMap;
    ListenerRefContainer m_UntrackedListeners;
    InvokeContainer m_DelayedInvokes;

    std::function<void (VCNIEvent*)> m_CleanUpFunction;
};

///////////////////// IMPLEMENTATION AHEAD

template<typename EventType, typename ListenerType>
VCNIEvent::Handle VCNEventManager::RegisterListener(ListenerType* listener, void (ListenerType::*callback)(const EventType&), bool track)
{
    VCNEvent<EventType>* handle = new VCNEvent<EventType>(listener, callback);

    VCNIEvent::Handle safeHandle = VCNIEvent::Handle(handle, m_CleanUpFunction);

    m_EventMap[ handle->GetEventName() ].push_back(safeHandle);

    if (!track) //keep a reference to make sure listener isn't immediately unregistered
        m_UntrackedListeners.push_back(safeHandle);

    return safeHandle;
}

template<typename EventType>
VCNIEvent::Handle VCNEventManager::RegisterListener(const std::function<void (const EventType&)>& callback)
{
    VCNEvent<EventType>* handle = new VCNEvent<EventType>(callback);

    VCNIEvent::Handle safeHandle = VCNIEvent::Handle(handle, m_CleanUpFunction);

    m_EventMap[ handle->GetEventName() ].push_back(safeHandle);

    return safeHandle;
}

template<typename EventType>
void VCNEventManager::Fire(const EventType& evt) const
{
    auto eventItr = m_EventMap.find(VCN::ExtractTypeName<EventType>());
    if (eventItr == std::end(m_EventMap))
        return;

    const ListenerContainer& listeners = eventItr->second;

    std::for_each( std::begin(listeners), std::end(listeners), [&evt](const WeakHandle& abstractListener)
    {
        VCNEvent<EventType>* listener = static_cast<VCNEvent<EventType>*>(abstractListener.lock().get());
        VCN_ASSERT(listener);
        if (listener)
            listener->Fire(evt);
    });
}
