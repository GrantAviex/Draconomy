#pragma once
#include "Component.h"
#include <vector>
#include <DirectXMath.h>
class CActivatable :
	public CComponent
{ 
	float m_fCoolDown;
	float m_fTimer;
	float m_fFireTime;
	float m_fReloadTime;
	int	  m_ID;
	int   m_animation;
	int   m_oldAnimation;
	CGameObject* spawnedObject;
	bool setup = false;
public:
	void SetCooldown(float _fCooldown) { m_fCoolDown = _fCooldown; }
	void SetID(int id);
	virtual CActivatable* clone() const;
	bool GetUseable();
	void Update(float _dt);
	void Activate();
	CActivatable(CGameObject* _Owner);
	~CActivatable();
};

