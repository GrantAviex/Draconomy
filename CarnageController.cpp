
//CarnageController.cpp
//Created May 2015

#include "CarnageController.h"
#include "GameObject.h"
#include "Game.h"
#include "CollisionManager.h"
#include "LocalUpdateManager.h"

CCarnageController::CCarnageController(CGameObject* _Owner) 
	: CComponent(_Owner, "CarnageController")
{	
}

CCarnageController::~CCarnageController()
{
#ifndef IARESERVER
	CGame::GetInstance()->GetLocalUpdateManager()->RemoveCarnage(this);
#endif
}
void CCarnageController::Update(float _dt)
{
	for (auto& limb : limbs)
	{
		XMFLOAT3 direction = limb.second;
		direction.y += -1650 * _dt;
		XMFLOAT3 velocity = XMFLOAT3(direction.x * _dt, direction.y * _dt, direction.z * _dt);
		limb.second = direction;
		if (limb.first->GetPosition().y <= limbStop[limb.first] + 5 && limb.first->GetPosition().y >= limbStop[limb.first])
		{
			if (direction.y <= 0.0f)
			{
				velocity.y *= -.5f;
				limb.second.y *= -.5f;
			}
		}
		limb.first->Translate(velocity);
		limb.first->Rotate(velocity.x * 0.5f, velocity.y * 0.5f, velocity.z * 0.5f);
	}
} 
void CCarnageController::OnEnable()
{
	if (limbs.size() > 0)
	{
		for (auto& limb : limbs)
		{
			limb.first->SetMatrix(&XMMatrixIdentity());
		}
		limbs.clear();
		limbStop.clear();
	}
	for (auto child : m_Owner->GetChildren())
	{
		float startingY = child->GetPosition().y;
		limbStop[child] = startingY;
		int xOffset = rand() % 100 - 50;
		int yOffset = rand() % 100 + 60;
		int zOffset = rand() % 100 - 50;

		child->SetPosition(XMFLOAT3((float)xOffset, (float)yOffset, (float)zOffset));
		
		xOffset = rand() % 360;
		yOffset = rand() % 360;
		zOffset = rand() % 360;

		child->Rotate((float)xOffset, (float)yOffset, (float)zOffset);

		xOffset = rand() % 400 - 200;
		yOffset = 950;
		zOffset = rand() % 400 - 200;

		limbs[child] = XMFLOAT3((float)xOffset, (float)yOffset, (float)zOffset);
	}
}
CCarnageController* CCarnageController::clone() const
{
	CCarnageController* m = new CCarnageController(*this);


#ifndef IARESERVER
	CGame::GetInstance()->GetLocalUpdateManager()->AddCarnage(m);

#endif
	return m;
}