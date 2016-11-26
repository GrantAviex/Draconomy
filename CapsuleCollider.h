#pragma once
#include "Collider.h"
#include "Component.h"
#include "SphereCollider.h"
struct TSphere;
enum CapsuleType
{
	eCapEnemy = 3,
	eCapPlayer,
	eCapStructure,
	eCapActivatable,
	eCapProps
};
class CCapsuleCollider :
	public ICollider, public CComponent
{
public:
	CCapsuleCollider(CGameObject* _Owner);
	virtual ~CCapsuleCollider();
	virtual ColliderType GetType(){ return ColliderType::eCapsule; }
	virtual CCapsuleCollider* clone() const;
	bool CheckCollision(ICollider* other) override;
	virtual void HandleCollision(ICollider* other) override;
	TSphere* m_BotSphere;
	TSphere* m_TopSphere;
	void SetUpSpheres();
	float m_fRadius;
	CapsuleType eCapsuleType = eCapEnemy;
	float m_fOffset = 0;
	void Update(float _dt) override;
};

