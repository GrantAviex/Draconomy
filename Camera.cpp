
//Camera.cpp
//Created May 2015

#include "Camera.h"
#include "Game.h"
#include "Renderer.h"
//#include "AudioSystemWwise.h"
#include "GlobalDef.h"
#ifdef IARESERVER
#include "Manager.h"
#endif
#include "GameObject.h"

CCamera::CCamera(CGameObject* _Owner)
	: CComponent(_Owner, "Camera")
{
	turnSensitivity = .5f;
	m_FieldOfView = 65;
	m_NearClip = 0.1f;
	m_FarClip = 12500.0f;
	float fov = XMConvertToRadians(m_FieldOfView);
	float aspect = GlobalDef::windowWidth / float(GlobalDef::windowHeight);
	auto m = XMMatrixPerspectiveFovLH(fov, aspect, m_NearClip, m_FarClip);
	XMStoreFloat4x4(&projMatrix, m);
	//	AudioSystemWwise::Get()->RegisterListener(_Owner, "Main Camera");
	//	AudioSystemWwise::Get()->RegisterEntity(_Owner, "Main Object");
	AddSubscription(EventID::MouseWheel);
	m_fDistanceTarget = m_Owner->GetPosition().z;
	AddSubscription(EventID::RotateCamera);
	AddSubscription(EventID::CameraShake);
#ifdef IARESERVER
	m_pCollision = CManager::GetInstance()->m_ColManager;
#endif
}

CCamera::~CCamera()
{
}

CCamera* CCamera::clone() const
{
	CCamera* m = new CCamera(*this);
#ifndef IARESERVER
	
	//CGame::GetInstance()->GetRenderer()->SetCamera(m);
#endif
	return m;
};

void CCamera::OnDisable()
{
}

void CCamera::OnEnable()
{
	shake = false;
	shakeAmt = 0;
}

void CCamera::PlayerAlive()
{
	playerDead = false;

}
void CCamera::PlayerDead()
{
	playerDead = true;
}
void CCamera::ReceiveEvent(EventID id, TEventData const* data)
{
	switch (id)
	{
	case EventID::MouseWheel:
	{
		if (data->i > 0.0f)
		{
			if (m_fDistanceTarget < -350.0f)
			{
				m_fDistanceTarget += 70;
			}
		}
		else
		{
			if (m_fDistanceTarget > -1000.0f)
			{
				m_fDistanceTarget -= 70;
			}
		}

		break;
	}
	case EventID::CameraShake:
	{
		shake = true;
		shakeAmt = data->i;
		shakeTimer = 0.5f;
		resetMat = m_Owner->GetMatrix();
		break;
	}


	default:
	{
		//error
		break;
	}
	}
}

void CCamera::Update(float _dt)
{
#ifdef IARESERVER
	//float dt = (float)(CManager::GetInstance()->GetDelta());
	//m_Owner->Translate(XMFLOAT3(0.0f, 0.0f, dt * -100.0f));

	float dt = (float)(CManager::GetInstance()->GetDelta());
	float tolerance = 10.0f;
	float distanceSpeed = 1500.0f;
	float currentDist = m_fDistanceTarget;

	XMVECTOR start = m_Owner->GetParent()->GetWorldMatrix().r[3];
	start = start + XMLoadFloat3(&XMFLOAT3(0.0f, m_Owner->GetPosition().y, 0.0f));
	XMVECTOR dir = m_Owner->GetWorldMatrix().r[3] - start;
	float maxDist = 1100.0f;
	if (m_bAutoAdjustDistance)
	{
		TRayCastHit hit;
		if (m_pCollision->RayCastToTri(start, dir, maxDist, hit))
		{
			float hitdist = -hit.fDistance + 110.0f;
			if (hitdist > currentDist)
			{
				currentDist = hitdist;

				XMFLOAT3 pos = m_Owner->GetPosition();
				if (pos.z + 50 < currentDist)
				{
					pos.z = currentDist;
					m_Owner->SetPosition(pos);
				}
			}
		}

		if (m_Owner->GetPosition().z > currentDist + tolerance)
		{
			m_Owner->Translate(XMFLOAT3(0.0f, 0.0f, -distanceSpeed * dt));
		}
		if (m_Owner->GetPosition().z < currentDist - tolerance)
		{
			m_Owner->Translate(XMFLOAT3(0.0f, 0.0f, distanceSpeed * dt));
		}
	}
	if (shake)
	{
		shakeTimer -= _dt;
		if (shakeTimer <= 0.0f)
		{
			shake = false;
			m_Owner->SetMatrix(&XMLoadFloat4x4(&resetMat));
		}
		int offsetX = rand() % 50 - 25;
		int offsetY = rand() % 50 - 25;
		m_Owner->Translate(XMFLOAT3(offsetX * shakeAmt * (shakeTimer), offsetY * shakeAmt * (shakeTimer), 0));
		
	}
#endif
}
