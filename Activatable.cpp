#include "Activatable.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "SphereTrigger.h"
#include "CapsuleCollider.h"
#include "BulletController.h"
#include "UnitStats.h"
#include "CollisionManager.h"
#include "Manager.h"
#include "EnemyController.h"
#include "Wwise_IDs.h"
#include "Server.h"

CActivatable::CActivatable(CGameObject* _Owner) : CComponent(_Owner, "Activatable")
{
	m_fCoolDown = 1.0f;
	m_fTimer = 0.0f;
	spawnedObject = nullptr;
	setup = false;
	m_animation = 0;
	m_oldAnimation = 0;
}


CActivatable::~CActivatable()
{
}
void CActivatable::SetID(int id)
{
	m_ID = id;
	if (m_ID == 1)
	{
		m_fFireTime = 0.1f;
		m_fReloadTime = 0.8f;
		m_oldAnimation = 1;
	}
}
void CActivatable::Update(float _dt)
{
	if (!setup)
	{
		m_fTimer = m_fCoolDown;
		setup = true;
	}
	if (spawnedObject == nullptr)
		m_fTimer += _dt;
	else
	{
		if (spawnedObject->GetActive() == false)
		{
			spawnedObject = nullptr;
		}
	}
	if (m_ID == 0 || m_ID == 2)
	{
		bool active = m_fTimer > m_fCoolDown;
		m_Owner->GetComponentInChildren("MeshRenderer")->GetOwner()->SetActive(active);
	}
	if (m_ID == 1)
	{
		if (m_animation != m_oldAnimation)
		{
			string name = "";
			switch (m_animation)
			{
			case 0:
			{
				name = "Static";
				break;
			}
			case 1:
			{
				name = "Fire";
				break;
			}
			case 2:
			{
				name = "Reload";
				break;
			}
			}

			for (auto& child : m_Owner->GetChildren())
			{
				child->SetActive(child->GetName() == name);
			}
			m_oldAnimation = m_animation;
		}
		if (m_fTimer >= m_fFireTime && m_fTimer <= m_fReloadTime)
		{
			m_animation = 2;
		}
		else if (m_fTimer >= m_fReloadTime )
		{
			m_animation = 0;
		}
	}
	
}
void CActivatable::Activate()
{
	if (m_fTimer >= m_fCoolDown)
	{
		if (m_ID == 0)
		{
			XMMATRIX spawnMatrix = m_Owner->GetWorldMatrix();
			spawnMatrix;
			XMFLOAT4X4 spawn;
			XMFLOAT3 trans = m_Owner->GetAxis(2);
			XMMATRIX translation = XMMatrixTranslation(trans.x * -400, trans.y * -400, trans.z * -400);
			XMStoreFloat4x4(&spawn, spawnMatrix * translation);
			spawnedObject = m_Owner->GetObjMan()->CreateObject("FallenTree", spawn, m_Owner->GetEventSystem());
			CSphereTrigger* trigger = (CSphereTrigger*)(spawnedObject->GetComponent("SphereTrigger"));
			trigger->eTriggerType = TriggerType::eTreeWall;
			CCollisionManager* manager = CManager::GetInstance()->m_ColManager;
			vector<CGameObject*> explosionVec = manager->OverlapSphere(spawnedObject->GetPosition(), 240, CapsuleType::eCapEnemy);
			for (CGameObject* explodeOBj : explosionVec)
			{
				CUnitStats* someStats = ((CUnitStats*)explodeOBj->GetComponent("UnitStats"));
				if (someStats != nullptr)
				{
					someStats->TakeDamage((float)-170);
					int statusInt = 0;
					//float slowAmt = 80;
					//if (slowAmt > 0.0f)
					//{
					//	((CEnemyController*)explodeOBj->GetComponent("EnemyController"))->SetSlowedStatus(slowAmt);
					//}
				}
			}
	}
	else if (m_ID == 1)
	{
		XMMATRIX bulletMat = m_Owner->GetWorldMatrix();
		XMFLOAT4X4 m;
		XMStoreFloat4x4(&m, bulletMat);
		CGameObject* pBullet = m_Owner->GetObjMan()->CreateObject("Bolt", m, m_Owner->GetEventSystem());
		pBullet->Translate(XMFLOAT3(0, 122, 0));
		auto bulletTrigger = (CSphereTrigger*)pBullet->GetComponent("SphereTrigger");
		bulletTrigger->eTriggerType = TriggerType::eBullet;
		CBulletController* bull = (CBulletController*)pBullet->GetComponent("BulletController");
		bull->SetWhoShotMe(m_Owner);
		bull->SetPiercing(true);
		bull->SetLifeTime(2.5f);
		bull->SetBouncing(false);
		m_animation = 1;
	}
	else if (m_ID == 2)
	{
		XMMATRIX bulletMat = m_Owner->GetWorldMatrix();
		XMFLOAT4X4 m;
		XMStoreFloat4x4(&m, bulletMat);
		CGameObject* pBullet = m_Owner->GetObjMan()->CreateObject("FallingStalactite", m, m_Owner->GetEventSystem());
		pBullet->Rotate(-90, 0, 0);
		auto bulletTrigger = (CSphereTrigger*)pBullet->GetComponent("SphereTrigger");
		bulletTrigger->eTriggerType = TriggerType::eBullet;
		CBulletController* bull = (CBulletController*)pBullet->GetComponent("BulletController");
		bull->SetWhoShotMe(m_Owner);
		bull->SetPiercing(true);
		float height = m_Owner->GetPosition().y;
		bull->SetLifeTime(1.0f * height / 1950.0f);
		bull->SetBouncing(false);
		bull->SetExploding(1);
	}
	else if (m_ID = 3)
	{
		float pushbackZ = -1350;
		CCollisionManager* manager = CManager::GetInstance()->m_ColManager;
		XMFLOAT3 collisionPos = XMFLOAT3(2900.0f, 25.0f, -3500.0f);
		vector<CGameObject*> pushVec = manager->OverlapSphere(collisionPos, 4000, CapsuleType::eCapEnemy);
		for (CGameObject* pushOBj : pushVec)
		{
			if (pushOBj->GetPosition().z < pushbackZ)
			{
				CUnitStats* someStats = ((CUnitStats*)pushOBj->GetComponent("UnitStats"));
				if (someStats != nullptr)
				{
					someStats->TakeDamage((float)-170);
					int statusInt = 0;
					float slowAmt = .80f;
					if (slowAmt > 0.0f)
					{
						((CEnemyController*)pushOBj->GetComponent("EnemyController"))->SetSlowedStatus(slowAmt,6.0f);
					}
				}
				((CEnemyController*)pushOBj->GetComponent("EnemyController"))->pushbackTimer = 4.5f;
			}
		}
		XMFLOAT4X4 t;
		XMMATRIX tMat = XMMatrixTranslation(-1500.0f, 250.0f, -2500.0f);
		XMStoreFloat4x4(&t, tMat);
		CGameObject* pWater = m_Owner->GetObjMan()->CreateObject("WaterWave", t, m_Owner->GetEventSystem());

		tMat = XMMatrixTranslation(-1500.0f, 250.0f, -3000.0f);
		XMStoreFloat4x4(&t, tMat);
		pWater = m_Owner->GetObjMan()->CreateObject("WaterWave", t, m_Owner->GetEventSystem());

		tMat = XMMatrixTranslation(-1500.0f, 250.0f, -3750.0f);
		XMStoreFloat4x4(&t, tMat);
		pWater = m_Owner->GetObjMan()->CreateObject("WaterWave", t, m_Owner->GetEventSystem());

		tMat = XMMatrixTranslation(-1500.0f, 250.0f, -4500.0f);
		XMStoreFloat4x4(&t, tMat);
		pWater = m_Owner->GetObjMan()->CreateObject("WaterWave", t, m_Owner->GetEventSystem());


		TEventData tdata;
		tdata.i = AK::EVENTS::PLAY_SFX_WATER_RUSH;
		CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, tdata);
	}
	m_fTimer = 0.0f;
}
}
bool CActivatable::GetUseable()
{
	return m_fTimer > m_fCoolDown;
}
CActivatable* CActivatable::clone() const
{
	CActivatable* m = new CActivatable(*this);

	return m;
};
