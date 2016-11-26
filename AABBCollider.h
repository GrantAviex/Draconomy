
//SphereCollider.h
//Created May 2015

#ifndef AABBCOLLIDER_H
#define AABBCOLLIDER_H

#include "Collider.h"
#include "Component.h"
#include <DirectXMath.h>
enum AABBType
{
//	eWall,
	eTurret
};
class CAABBCollider : public ICollider, public CComponent
{
public:
	DirectX::XMFLOAT3 m_min, m_max, m_center;
	CAABBCollider(CGameObject* _Owner);
	~CAABBCollider();
	virtual ColliderType GetType(){ return ColliderType::eAABB; }
	virtual CAABBCollider* clone() const;
	bool CheckCollision(ICollider* other) override;
	virtual void HandleCollision(ICollider* other) override;
	
//	AABBType eAABBType = eWall;
private:
};

#endif