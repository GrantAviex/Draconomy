#include "CapsuleCollider.h"
#include "GameObject.h"
#include "CollisionManager.h"
#include "AssetManager.h"
#include "Manager.h"
#include "Mesh.h"
#include "AABBCollider.h"
#include "Server.h"

CCapsuleCollider::CCapsuleCollider(CGameObject* _Owner) : CComponent(_Owner, "CapsuleCollider")
{
	m_BotSphere = new TSphere();
	m_TopSphere = new TSphere();
}
CCapsuleCollider* CCapsuleCollider::clone()const
{
	CCapsuleCollider* capsule = new CCapsuleCollider(*this);
	CManager::GetInstance()->m_ColManager->gameObjects.push_back(capsule);
	return capsule;
}

CCapsuleCollider::~CCapsuleCollider()
{
//	delete m_BotSphere; //NEEDS FIXED
//	delete m_TopSphere; //NEEDS FIXED
}
void CCapsuleCollider::Update(float _dt)
{
	SetUpSpheres();
}
void CCapsuleCollider::SetUpSpheres()
{
	m_BotSphere->vCenter = m_Owner->GetPosition();
	m_BotSphere->fRadius = m_fRadius;

	XMFLOAT3 offsetPosition = m_BotSphere->vCenter;
	offsetPosition.y += m_fOffset;
	m_TopSphere->vCenter = offsetPosition;
	m_TopSphere->fRadius = m_fRadius;
}
void CCapsuleCollider::HandleCollision(ICollider* other)
{

}
bool CCapsuleCollider::CheckCollision(ICollider* other)
{
	if (other->GetType() == ColliderType::eSphere)
	{

		TSphere otherSphere, mySphere;
		otherSphere.fRadius = ((CSphereCollider*)other)->m_Radius;
		otherSphere.vCenter = ((CSphereCollider*)other)->GetOwner()->GetPosition();
		//	otherSphere.vCenter.y += 60;
		mySphere.fRadius = m_fRadius;
		mySphere.vCenter = m_BotSphere->vCenter;
		//	mySphere.vCenter.y += 80;
		bool Collision = CCollisionManager::SphereToSphere(mySphere, otherSphere);
		if (Collision == FALSE)
		{
			mySphere.vCenter = m_TopSphere->vCenter;
			Collision = CCollisionManager::SphereToSphere(mySphere, otherSphere);
		}
		return Collision;
	}
	else if (other->GetType() == ColliderType::eCapsule)
	{
		if (eCapsuleType == CapsuleType::eCapPlayer)
		{
			CCapsuleCollider* capPtr = ((CCapsuleCollider*)other);
			if (capPtr->eCapsuleType == CapsuleType::eCapProps)
			{
				TCapsule tCap;
				tCap.botPos = capPtr->m_BotSphere->vCenter;
				tCap.topPos = capPtr->m_TopSphere->vCenter;
				tCap.fRadius = capPtr->m_fRadius;
				TSphere tempSphere;
				tempSphere.fRadius = m_fRadius;
				tempSphere.vCenter = m_BotSphere->vCenter;
				bool bCollission = CCollisionManager::CapsuleToSphere(tCap, tempSphere);
				if (bCollission)
				{
					capPtr->m_Owner->SetPosition(tempSphere.vCenter);
					capPtr->SetUpSpheres();
				}
			}
			return false;
		}
		if (eCapsuleType == CapsuleType::eCapProps)
		{
			CCapsuleCollider* capPtr = ((CCapsuleCollider*)other);
			SetUpSpheres();
			if (capPtr->eCapsuleType == CapsuleType::eCapPlayer)
			{
				TSphere tempSphere;
				tempSphere.fRadius = capPtr->m_fRadius;
				tempSphere.vCenter = capPtr->m_BotSphere->vCenter;
				TCapsule tCap;
				tCap.botPos = m_BotSphere->vCenter;
				tCap.topPos = m_TopSphere->vCenter;
				tCap.fRadius = m_fRadius;
				bool bCollission = CCollisionManager::CapsuleToSphere(tCap, tempSphere);
				if (bCollission)
				{
					capPtr->m_Owner->SetPosition(tempSphere.vCenter);
					capPtr->SetUpSpheres();
				}
			}
			return false;
		}
	}

	return false;
}
