
//CandyPickup.h
//Created May 2015

#ifndef CANDYPICKUP_H
#define CANDYPICKUP_H

#include <DirectXMath.h>
using namespace DirectX;
#include "Component.h"

class CCandyPickup : public CComponent
{
public:
	CCandyPickup(CGameObject* owner);
	~CCandyPickup();
	virtual CCandyPickup* clone() const { return new CCandyPickup(*this); };
	void Update(float _dt) override;
	void ReceiveEvent(EventID id, TEventData const* data) override;
	void OnEnable() override;
	void StartReturn();
	bool IsReturning() { return m_bIsReturning; }

private:
	bool m_bIsReturning;
	CGameObject* m_pGoldPile;

	float m_fReturnTimer;
	float m_fReturnMaxTime;
	float m_fHeight;
	XMFLOAT3 m_f3ReturnStart;
	XMFLOAT3 m_f3ReturnEnd;
};

#endif
