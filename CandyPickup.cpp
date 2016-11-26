
//CandyPickup.cpp
//Created May 2015

#include "CandyPickup.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "Manager.h"
#include "Server.h"
#include "EventSystem.h"

CCandyPickup::CCandyPickup(CGameObject* _Owner)
	: CComponent(_Owner, "CandyPickup")
{
	AddSubscription(EventID::BuildPhaseStart);
	m_bIsReturning = false;
	m_pGoldPile = nullptr;
	m_fReturnTimer = 0.0f;
}

CCandyPickup::~CCandyPickup()
{
}

void CCandyPickup::ReceiveEvent(EventID id, TEventData const* data)
{
	switch (id)
	{
	case EventID::BuildPhaseStart:
	{
		StartReturn();
		break;
	}
	}
}

void CCandyPickup::OnEnable()
{
	m_bIsReturning = false;
}

void CCandyPickup::StartReturn()
{
	if (m_pGoldPile == nullptr)
	{
		m_pGoldPile = m_Owner->GetObjMan()->GetPool("Barrel")[0];
	}

	m_f3ReturnStart = m_Owner->GetPosition();
	m_f3ReturnEnd = m_pGoldPile->GetPosition();

	XMVECTOR start = XMLoadFloat3(&m_f3ReturnStart);
	XMVECTOR end = XMLoadFloat3(&m_f3ReturnEnd);
	XMFLOAT3 distance;
	XMStoreFloat3(&distance, XMVector3Length(end - start));
	m_fReturnMaxTime = distance.x + 1000.0f;
	m_fReturnTimer = 0.0f;
	m_fHeight = (distance.x * 0.36f) + 35.0f;
	if (m_fHeight > 950.0f)
	{
		m_fHeight = 950.0f;
	}

	m_bIsReturning = true;
}

void CCandyPickup::Update(float _dt)
{
	float dt = CManager::GetInstance()->GetDelta();

	//Spin
	if (!m_bIsReturning)
	{
		m_Owner->Rotate(0, _dt * 90, 0);
	}

	//Return to gold pile
	if (m_bIsReturning)
	{
		m_fReturnTimer += dt;
		float distanceScale = 0.0004f;
		float t = m_fReturnTimer / (m_fReturnMaxTime * distanceScale);

		XMVECTOR start = XMLoadFloat3(&m_f3ReturnStart);
		XMVECTOR end = XMLoadFloat3(&m_f3ReturnEnd);
		XMVECTOR lerpPosition = XMVectorLerp(start, end, t);

		XMFLOAT3 newPosition;
		XMStoreFloat3(&newPosition, lerpPosition);
		float h = (t * 2.0f) - 1.0f;
		newPosition.y += (1.0f - (h * h)) * m_fHeight;
		m_Owner->SetPosition(newPosition);

		//End
		if (t >= 1.0f)
		{
			TEventData data;
			data.i = 1;
			data.s = "CandyReturn";
			CManager::GetInstance()->GetServer()->GetEventSystem()->SendEventImmediate(EventID::CandyReturned, &data);

			data.i = m_pGoldPile->GetID();
			CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetEmitterActive, data);

			data.i = 0;
			XMFLOAT4X4 posMatrix;
			XMStoreFloat4x4(&posMatrix, m_pGoldPile->GetWorldMatrix());
			memcpy(data.m, posMatrix.m, sizeof(float) * 16);
			CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, data);
			m_Owner->SetActive(false);
		}
	}
}
