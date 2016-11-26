
//CandyContainer.cpp
//Created May 2015

#include "CandyContainer.h"
#include "Manager.h"
#include "Server.h"
#include "GameObject.h"

CCandyContainer::CCandyContainer(CGameObject* _Owner) 
	: CComponent(_Owner, "CandyContainer")
{
	m_MaxCandy = 1;
	m_CurrCandy = m_MaxCandy;
	AddSubscription(EventID::CandyReturned);
}

CCandyContainer::~CCandyContainer()
{
}

CCandyContainer* CCandyContainer::clone() const
{
	CCandyContainer* other = new CCandyContainer(*this);
	CManager::GetInstance()->SetCandyContainer(other);
	return other;
}
void CCandyContainer::ReceiveEvent(EventID id, TEventData const* data)
{
	switch (id)
	{
	case EventID::CandyReturned:
	{
		m_CurrCandy += data->i;

		TEventData tdata;
		tdata.i = m_Owner->GetID();
		tdata.s = "CandyReturned";
		CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetEmitterActive, tdata);
		CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, tdata);
		break;
	}
	}
}