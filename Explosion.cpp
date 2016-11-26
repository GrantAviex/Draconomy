#include "Explosion.h"


CExplosion::CExplosion(CGameObject* _Owner) : CComponent(_Owner, "Explosion")
{
}


CExplosion::~CExplosion()
{
}
CExplosion* CExplosion::clone()const
{
	CExplosion* explosion = new CExplosion(*this);
	return explosion;
}
