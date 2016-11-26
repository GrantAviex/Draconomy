
//BulletController.cpp
//Created May 2015

#include "BulletController.h"
#include "GameObject.h"
#ifdef IARESERVER
#include "Manager.h"
#include "CollisionManager.h"
#include "PlayerGunController.h"
#include "ObjectManager.h"
#include "VortexController.h"
#include "Server.h"
#include "Wwise_IDs.h"
#endif
#include "XTime.h"

CBulletController::CBulletController(CGameObject* _Owner)
	: CComponent(_Owner, "BulletController")
{
	m_MoveSpeed = 10.0f; //Placeholder
	myShooter = nullptr;
	m_fSpin = 00.0f;
	m_fDisplacement = 5.0f;
	m_fFrequency = 6.0f;
	vel = XMFLOAT3(0, 0, 0);
	gravity = false;
	m_InitialDamage = 0;
}

CBulletController::~CBulletController()
{
}

CBulletController* CBulletController::clone() const
{
	CBulletController* other = new CBulletController(*this);
	return other;
}

void CBulletController::OnEnable()
{
	m_fSpin = 0.0f;
	myTarget = nullptr;
	m_bVortex = false;
	m_nBouncesLeft = m_nMaxBounces;
	dmgMod = 1.0f;
	amrMod = 1.0f;
	vel = XMFLOAT3(0, 0, 0);
	gravity = false;
	m_bDecay = false;

	if (!m_InitialDamage)
		m_InitialDamage = m_nDamage;
}
void CBulletController::OnDisable()
{
	CreateHitEffect();
	SetExploding(0);
	SetBouncing(false);
	vcEnemiesHit.clear();

	m_fSlow = 0.0f;
	m_nExploding = 0;
	m_bBouncing = false;
	m_bBurning = false;

	//m_InitialDamage = 0;
}
void CBulletController::CreateHitEffect()
{
	XMFLOAT4X4 transform;
	XMStoreFloat4x4(&transform, XMMatrixIdentity());
	CGameObject* pSplosions = m_Owner->GetObjMan()->CreateObject(m_sHitEffect, transform, m_Owner->GetEventSystem());
	pSplosions->SetPosition(m_Owner->GetPosition());
	if (gravity)
	{
		CVortexController* vCont = 	(CVortexController*)(pSplosions->GetComponent("VortexController"));
		vCont->SetLifeTime(3.0f);

		TEventData data;
		data.i = AK::EVENTS::PLAY_SFX_VORTEX;
		data.m[0] = vCont->GetOwner()->GetWorldMatrix().r[3].m128_f32[0];
		data.m[1] = vCont->GetOwner()->GetWorldMatrix().r[3].m128_f32[1];
		data.m[2] = vCont->GetOwner()->GetWorldMatrix().r[3].m128_f32[2];

		CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, data);
	}
}
bool CBulletController::CanHit(CGameObject* _col)
{
	for (CGameObject* targHits : vcEnemiesHit)
	{
		if (targHits == _col)
			return false;
	}
	return true;
}
void CBulletController::Bounce()
{
	if (m_bPiercing == TRUE)
	{
		CreateHitEffect();
	}
	else if (m_nBouncesLeft <= 0 || m_bBouncing == false)
		m_Owner->SetActive(false);
	else
	{
		std::vector<CGameObject*> vcTargets = CManager::GetInstance()->m_ColManager->OverlapSphere(m_Owner->GetPosition(), 1200.0f, CapsuleType::eCapEnemy);
		for (CGameObject* targs : vcTargets)
		{
			bool bNewTarget = true;
			for (CGameObject* hitTargs : vcEnemiesHit)
			{
				if (targs == hitTargs)
				{
					bNewTarget = false;
				}
			}
			if (bNewTarget == TRUE)
			{
				float dt = (float)(CManager::GetInstance()->GetDelta());
				XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
				XMFLOAT3 movement{ 0.0f, 0.0f, 0.0f };
				XMVECTOR targPos = XMLoadFloat3(&targs->GetPosition());
				XMVECTOR myPos = XMLoadFloat3(&m_Owner->GetPosition());

				forward = targPos - myPos;
				forward = XMVector3Normalize(forward);
				XMFLOAT4X4 tempRotMat = m_Owner->GetMatrix();
				tempRotMat._31 = -forward.m128_f32[0]; //REVERSE
				tempRotMat._32 = -forward.m128_f32[1]; //REVERSE
				tempRotMat._33 = -forward.m128_f32[2]; //REVERSE
				tempRotMat._34 = 0;
				XMMATRIX storeTempRot = XMLoadFloat4x4(&tempRotMat);
				m_Owner->SetMatrix(&storeTempRot);
				XMStoreFloat3(&movement, forward * m_MoveSpeed * dt);
				
				myTarget = targs;
				m_nBouncesLeft--;
				CreateHitEffect();
				return;
			}
		}
		m_Owner->SetActive(false); //Couldn't find a target within range(DESTROY)

	}
}
void CBulletController::Update(float _dt)
{
#ifdef IARESERVER
	float dt = (float)(CManager::GetInstance()->GetDelta());
	fLiveTimer -= (float)(CManager::GetInstance()->GetDelta());
	if (fLiveTimer <= 0)
	{
		m_Owner->SetActive(false);
		
		if (GetWhoShotMe()->GetComponent("PlayerGunController"))
			reinterpret_cast<CPlayerGunController*>(GetWhoShotMe()->GetComponent("PlayerGunController"))->MissedEnemy();
	}
	else
	{
		XMVECTOR forward = XMVectorSet(0.0f,0.0f,0.0f,0.0f);
		XMFLOAT3 movement{0.0f,0.0f,0.0f};
		if (m_bHoming == TRUE)
		{
			if (myTarget == nullptr)
			{
				std::vector<CGameObject*> vcTargets = CManager::GetInstance()->m_ColManager->OverlapSphere(m_Owner->GetPosition(), 700.0f, CapsuleType::eCapEnemy);

				for (CGameObject* trueTargs : vcTargets)
				{
					if (CanHit(trueTargs))
					{
						myTarget = trueTargs;
						break;
					}
				}
			
				if (myTarget == nullptr)
				{
					forward = XMLoadFloat3(&(m_Owner->GetAxis(2)));
					XMStoreFloat3(&movement, -forward * m_MoveSpeed * _dt); //REVERSE
				}
			}
			
				if (myTarget != nullptr && myTarget->GetActive() == true)
				{
					XMVECTOR targPos = XMLoadFloat3(&myTarget->GetPosition());
					XMVECTOR myPos = XMLoadFloat3(&m_Owner->GetPosition());

					forward = targPos - myPos;
					forward = XMVector3Normalize(forward);
					XMFLOAT4X4 tempRotMat = m_Owner->GetMatrix();
					tempRotMat._31 = -forward.m128_f32[0]; //REVERSE
					tempRotMat._32 = -forward.m128_f32[1]; //REVERSE
					tempRotMat._33 = -forward.m128_f32[2]; //REVERSE
					tempRotMat._34 = 0;
					XMMATRIX storeTempRot = XMLoadFloat4x4(&tempRotMat);
					m_Owner->SetMatrix(&storeTempRot);
					XMStoreFloat3(&movement, forward * m_MoveSpeed * dt);
				}
				else
				{
					myTarget = nullptr;
				}
			
		}
		else
		{
			if (setDir)
			{
				setDir = false;
			}
			if (gravity)
			{
				vel.y += -19.61f * _dt;
				m_Owner->Translate(vel);
			}
			dir = m_Owner->GetAxis(2);
			forward = XMLoadFloat3(&(dir));
			XMStoreFloat3(&movement, -forward * m_MoveSpeed * _dt); //REVERSE
			m_fSpin += _dt * m_fFrequency;
			float z = sinf(m_fSpin) * m_fDisplacement;
			m_Owner->Rotate(0,0, z* 3);
		}
		m_Owner->Translate(movement);
	}
#endif
}

void CBulletController::KilledEnemy()
{
	if (myShooter->GetComponent("PlayerGunController"))
		reinterpret_cast<CPlayerGunController*>(myShooter->GetComponent("PlayerGunController"))->AddEnemiesKilled(1);
}