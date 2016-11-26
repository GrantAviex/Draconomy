
//DragonSlayerTimer.cpp
//Created June 2015

#include "DragonSlayerTimer.h"
#ifdef IARESERVER
#include "Manager.h"
#include "XTime.h"
#include "ObjectManager.h"
#include "Server.h"
#include "Events.h"
#include "ManagerplayState.h"
#include "CollisionManager.h"
#endif

CDragonSlayerTimer::CDragonSlayerTimer(CGameObject* _Owner)
	: CComponent(_Owner, "DragonSlayerTimer")
{
	m_fTimer = 0.0f;
	dragonSlayer = nullptr;
}

CDragonSlayerTimer::~CDragonSlayerTimer()
{
}

void CDragonSlayerTimer::OnEnable()
{
	m_fTimer = 0.0f;
	spawned = false;
}

void CDragonSlayerTimer::OnDisable()
{
	m_fTimer = 0.0f;
}

void CDragonSlayerTimer::Update(float REMOVE_THIS_PARAMETER)
{
	float dt = REMOVE_THIS_PARAMETER;
	m_fTimer += dt;
	if (m_fTimer >= m_fTimeLimit)
	{
		m_Owner->SetActive(false);
		if (dragonSlayer)
		{
			m_Owner->GetObjMan()->CreateObject("DragonSlayerLanding", m_Owner->GetMatrix(), m_Owner->GetEventSystem());
			m_Owner->GetObjMan()->CreateObject("EnemyAssassin", m_Owner->GetMatrix(), m_Owner->GetEventSystem());
			vector<CGameObject*> explosionVec = CManager::GetInstance()->m_ColManager->OverlapSphere(m_Owner->GetPosition(), 120, CapsuleType::eCapPlayer);
			for (CGameObject* explodeOBj : explosionVec)
			{
				CUnitStats* someStats = ((CUnitStats*)explodeOBj->GetComponent("UnitStats"));
				if (someStats != nullptr)
				{
					someStats->TakeDamage((float)someStats->GetMaxHeatlh() / -2);
				}
			}
			TEventData data;
			data.i = 5.0f;
			CManager::GetInstance()->GetServer()->SendEventToAllPlayers(EventID::CameraShake, data);
		}
	}
	if (!target)
	{
		if (dragonSlayer)
		{
			float t = m_fTimer / (m_fTimeLimit);

			XMVECTOR start = XMVectorSet(0, 10000, 0, 0);
			XMVECTOR end = XMVectorSet(0, 0, 0, 0);
			XMVECTOR lerpPosition = XMVectorLerp(start, end, t);

			XMFLOAT3 newPosition;
			XMStoreFloat3(&newPosition, lerpPosition);
			dragonSlayer->SetPosition(newPosition);
		}
		else
		{
			for (auto& child : m_Owner->GetChildren())
			{
				if (child->GetName() == "DragonSlayer")
				{
					dragonSlayer = child;
				}
			}
		}
	}
	else
	{
		if (m_fTimer <= 2.0f && spawned == false)
		{
			m_Owner->GetObjMan()->CreateObject("DragonSlayerFalling", m_Owner->GetMatrix(), m_Owner->GetEventSystem());
			spawned = true;
		}
	}
}