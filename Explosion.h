#pragma once
#include "Component.h"
class CExplosion :
	public CComponent
{
	float m_fExplosionRadius;
public:
	float GetExplosionRadius(){ return m_fExplosionRadius; }
	void SetExplosionRadius(float _radius){ m_fExplosionRadius = _radius; }
	CExplosion(CGameObject* _Owner);
	virtual CExplosion* clone() const;

	virtual ~CExplosion();
};

