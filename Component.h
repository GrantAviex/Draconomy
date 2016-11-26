
//Component.h
//Created May 2015

#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
using namespace std;

#include "EventSystem.h"
class CGameObject;

class CComponent : public IEventSubscriber
{
public:
	CComponent(CGameObject* owner, string name);
	virtual CComponent* clone() const = 0;
	~CComponent() {}
	virtual void Update(float _dt) { }
	CGameObject* GetOwner() { return m_Owner; }
	void SetOwner(CGameObject* owner) { m_Owner = owner; }
	string GetName() { return m_Name; }
	virtual void Subscribe();
	virtual void Unsubscribe();
	virtual void OnEnable(){}
	virtual void OnDisable(){}
	void AddSubscription(EventID id);
	virtual void ReceiveEvent(EventID id, TEventData const* data){}

protected:
	CGameObject* m_Owner;
	string m_Name;
private:
	vector<EventID> m_vSubscribedEvents;
};

#endif
