#include "AABBCollider.h"
#include "CollisionManager.h"
#include "SphereCollider.h"
#include "GameObject.h"
#include "Manager.h"

CAABBCollider::CAABBCollider(CGameObject* _Owner) : CComponent(_Owner, "AABBCollider")
{

}
CAABBCollider::~CAABBCollider()
{

}
void CAABBCollider::HandleCollision(ICollider* other)
{

}
CAABBCollider* CAABBCollider::clone()const
{
	CAABBCollider* aabb = new CAABBCollider(*this);
	CManager::GetInstance()->m_ColManager->aabbCColliders.push_back(aabb);

	CManager::GetInstance()->m_ColManager->gameObjects.push_back(aabb);
	return aabb;
}
bool CAABBCollider::CheckCollision(ICollider* other)
{
	if (other->GetType() == ColliderType::eSphere)
	{
		TSphere otherSphere;
		otherSphere.fRadius = ((CSphereCollider*)other)->m_Radius;
		otherSphere.vCenter = ((CSphereCollider*)other)->GetOwner()->GetPosition();

		float distance;
		XMVECTOR dir;
		bool Collision = CCollisionManager::SphereToAABB(otherSphere, *this, distance, dir);
		if (Collision)
		{
			XMVECTOR newPos = XMLoadFloat3(&otherSphere.vCenter);
			newPos += (XMVector3Normalize(dir) * distance);
			XMStoreFloat3(&otherSphere.vCenter, newPos);
			((CSphereCollider*)other)->GetOwner()->SetPosition(otherSphere.vCenter);
		}
		return Collision;
	}
	return false;
}
