
//EventSystem.h
//Created May 2015

#ifndef EVENTSYSTEM_H
#define EVENTSYSTEM_H

#include <map>
#include <vector>
#include "Events.h"
using namespace std;

struct TEventData;
class IEventSubscriber;
typedef vector<IEventSubscriber*> SubscriberList;
typedef vector<TEventData> StoredEventList;
typedef pair<EventID, IEventSubscriber*> PendingSubscription;

class CEventSystem
{
public:
	CEventSystem();
	~CEventSystem();

	void Subscribe(EventID id, IEventSubscriber* subscriber);
	void Unsubscribe(EventID id, IEventSubscriber* subscriber);
	void SendEvent(EventID id, TEventData const* data);
	void SendEventImmediate(EventID id, TEventData const* data);
	void Dispatch();
	unsigned int GetSubscriberCount(EventID id);

private:

	map<EventID, SubscriberList> m_Subscribers;
	map<EventID, StoredEventList> m_StoredEvents;
	vector<PendingSubscription> m_PendingSubscriptions;
	vector<PendingSubscription> m_PendingUnsubscriptions;
	bool m_bDispatchInProgress;
	void ProcessPending();
};

//=========================================================


class IEventSubscriber
{
public:
	virtual ~IEventSubscriber() {}
	virtual void ReceiveEvent(EventID id, TEventData const* data) = 0;
};

#endif
