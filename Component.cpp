
//Component.cpp
//Created May 2015

#include "Component.h"
#include "GameObject.h"
CComponent::CComponent(CGameObject* _Owner, string _Name) : m_Owner{ nullptr }
{
	m_Owner = _Owner;
	m_Name = _Name;
}
void CComponent::AddSubscription(EventID id)
{
	m_vSubscribedEvents.push_back(id); 
}

void CComponent::Subscribe()
{
	if (m_Owner->GetEventSystem() == nullptr)
		return;
	for (size_t i = 0; i < m_vSubscribedEvents.size(); i++)
	{
		m_Owner->GetEventSystem()->Subscribe(m_vSubscribedEvents[i], this);
	}

}
void CComponent::Unsubscribe()
{
	if (m_Owner->GetEventSystem() == nullptr)
		return;
	for (size_t i = 0; i < m_vSubscribedEvents.size(); i++)
	{
		m_Owner->GetEventSystem()->Unsubscribe(m_vSubscribedEvents[i], this);
	}

}