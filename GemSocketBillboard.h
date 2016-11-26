#ifndef GEMSOCKETBILLBOARD_H
#define GEMSOCKETBILLBOARD_H

#include "Component.h"
#include <DirectXMath.h>
#include <vector>

class CRenderBillboard;

class CGemSocketBillboard : public CComponent
{
public:
	CGemSocketBillboard(CGameObject* Owner);
	~CGemSocketBillboard();

	virtual CGemSocketBillboard* clone() const;
	void OnEnable() override;
	void OnDisable() override;
	void Update(float Delta) override;
	void ReceiveEvent(EventID id, TEventData const* data) override;
	void SetOffset(float Offset) { m_Offset = Offset; }
private:
	CGameObject*						m_Owner;
	std::vector<CRenderBillboard*>		m_Billboards;
	CRenderBillboard*					m_FirstGem;
	CRenderBillboard*					m_SecondGem;

	float								m_Offset;

	bool								m_bInit;
};

#endif;