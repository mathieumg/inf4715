///
/// Copyright (C) 2012 - All Rights Reserved
/// All rights reserved. http://www.equals-forty-two.com
///

#include "Precompiled.h"
#include "EventManager.h"

#include "Node.h"

#include <numeric>

VCNEventManager::Handle VCNEventManager::Create()
{
    Handle created = Handle(new VCNEventManager());

    std::weak_ptr<VCNEventManager> weakHandle = created;

    //keep a weak reference in lambda to know if ever we get deleted elsewhere without
    //stopping people from living their lives as they would.
    created->m_CleanUpFunction = [weakHandle](VCNIEvent* handle)
    {
        //call friend function to access all our data
        //made this friend to not pollute public class interface with implementation details
        //and not keep a raw this ptr in lambda (which could point to anything if object is destroyed
        UnregisterOnHandleDestroyed(weakHandle, handle);
    };

    return created;
}

VCNEventManager::VCNEventManager()
{}

void UnregisterOnHandleDestroyed(std::weak_ptr<VCNEventManager> eventManager, VCNIEvent* eventHandle)
{
    // if this is called from an empty shared ptr
    if (eventHandle == nullptr)
        return;

    // get a hard ref to self
    std::shared_ptr<VCNEventManager> manager = eventManager.lock();

    if (!manager)   //the event manager handling said event handle is destroyed, no cleanup necessary
        return;     //example a node is destroyed and another node which was listening to events from the first one is destroyed

    auto& eventContainer = manager->m_EventMap[eventHandle->GetEventName()];

    size_t size = eventContainer.size();
    auto last = std::remove_if( eventContainer.begin(), eventContainer.end(), [](VCNEventManager::WeakHandle& handle)
    {
        return handle.expired();
    });

    eventContainer.erase(last, eventContainer.end());

    VCN_ASSERT_MSG( eventContainer.size() == size - 1, "WEAK HANDLE NOT REMOVED FROM EVENTMAP");

    delete eventHandle;
}

VCNEventManager::~VCNEventManager()
{
    m_UntrackedListeners.clear();
    m_EventMap.clear();
}

std::size_t VCNEventManager::GetEventCount() const
{
    return m_EventMap.size();
}

std::size_t VCNEventManager::GetCallbackCount() const
{
    return std::accumulate( std::begin(m_EventMap), std::end(m_EventMap), 0U, [](size_t accum, const EventPair& pair) -> std::size_t
    {
        return accum + pair.second.size();
    });
}

void VCNEventManager::UnRegisterListener(VCNIListener* listener)
{
    size_t size = m_UntrackedListeners.size();
    for(int i = size - 1; i >= 0; --i)
    {
        if (m_UntrackedListeners[i]->GetTarget() != listener)
            continue;

        m_UntrackedListeners.erase( m_UntrackedListeners.begin() + i );
    }

    VCNAtom* atom = dynamic_cast<VCNAtom*>(listener);
    VCN_ASSERT_MSG(size != m_UntrackedListeners.size(), "Unregister listener was unable to unregister listener of type %s", atom != nullptr ? atom->GetTypeName() : "UNKNOWN");
}

//pass function to this to call it at the beginning of the next frame
void VCNEventManager::InvokeLater(const NodeFunction& func)
{
    m_DelayedInvokes.push_back(func);
}

void VCNEventManager::FireInvokedFunctions(VCNNode* target)
{
    std::for_each(m_DelayedInvokes.begin(), m_DelayedInvokes.end(), [target](NodeFunction& func)
    {
        func(target);
    });

    m_DelayedInvokes.clear();
}