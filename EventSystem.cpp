
//EventSystem.cpp
//Created May 2015

#include "EventSystem.h"
#include <algorithm>

CEventSystem::CEventSystem()
{

}
CEventSystem::~CEventSystem()
{
	int x = 5;
}
void CEventSystem::Subscribe(EventID id, IEventSubscriber* subscriber)
{
	for (auto& sub : m_Subscribers[id])
	{
		if (sub == subscriber)
			return;
	}
	if (m_bDispatchInProgress == false)
	{
		m_Subscribers[id].push_back(subscriber);
	}
	else
	{
		PendingSubscription pending(id, subscriber);
		m_PendingSubscriptions.push_back(pending);
	}
}

void CEventSystem::Unsubscribe(EventID id, IEventSubscriber* subscriber)
{
	if (m_bDispatchInProgress == false)
	{
		auto& subs = m_Subscribers[id];
		subs.erase(remove(subs.begin(), subs.end(), subscriber), subs.end());
	}
	else
	{
		PendingSubscription pending(id, subscriber);
		m_PendingUnsubscriptions.push_back(pending);
	}
}

void CEventSystem::SendEvent(EventID id, TEventData const* data)
{
	m_StoredEvents[id].push_back(*data);
}

void CEventSystem::SendEventImmediate(EventID id, TEventData const* data)
{
	m_bDispatchInProgress = true;

	//For each subscriber of this event type
	SubscriberList const& subs = m_Subscribers[id];
	for (auto const& sub : subs)
	{
		sub->ReceiveEvent(id, data);
	}

	m_bDispatchInProgress = false;
	ProcessPending();
}

void CEventSystem::Dispatch()
{
	m_bDispatchInProgress = true;

	//For each event type
	for (auto const& v : m_Subscribers)
	{
		EventID const& id = v.first;
		SubscriberList const& subs = v.second;

		//For each stored eventdata of this event type
		for (auto const& data : m_StoredEvents[id])
		{
			//For each subscriber of this event type
			for (auto const& sub : subs)
			{
				sub->ReceiveEvent(id, &data);
			}
		}
	}

	m_StoredEvents.clear();
	m_bDispatchInProgress = false;
	ProcessPending();
}

void CEventSystem::ProcessPending()
{
	for (auto const& sub : m_PendingSubscriptions)
	{
		Subscribe(sub.first, sub.second);
	}
	m_PendingSubscriptions.clear();

	for (auto const& unsub : m_PendingUnsubscriptions)
	{
		Unsubscribe(unsub.first, unsub.second);
	}
	m_PendingUnsubscriptions.clear();
}
unsigned int CEventSystem::GetSubscriberCount(EventID id)
{
	for (auto const& v : m_Subscribers)
	{
		EventID const& _id = v.first;
		SubscriberList const& subs = v.second;
		if (_id == id)
		{
			return subs.size();
		}
	}
	return 0;
}