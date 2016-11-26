
//Camera.h
//Created May 2015

#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include <DirectXMath.h>
#include "CollisionManager.h"
#include "Component.h"

class CCamera : public CComponent
{
public:
	CCamera(CGameObject* owner);
	~CCamera();
	virtual CCamera* clone() const;
	void OnEnable() override;
	void OnDisable() override;
	void Update(float _dt) override;
	void PlayerDead();
	void PlayerAlive();
	DirectX::XMFLOAT4X4 projMatrix;
	float m_MaxFollowDistance;
	float m_MinFollowDistance;
	int m_FollowDistanceLevels;
	float m_FieldOfView;
	float m_NearClip;
	float m_FarClip;
	bool m_bAutoAdjustDistance = true;
	void ReceiveEvent(EventID id, TEventData const* data) override;

private:
	bool shake;
	int shakeAmt;
	float shakeTimer;
	XMFLOAT4X4 resetMat;
	float turnSensitivity;
	bool playerDead = false;
	CCollisionManager* m_pCollision;
	float m_fDistanceTarget;
};

#endif
