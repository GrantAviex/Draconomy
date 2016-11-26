#ifndef ENEMYHEALTHBAR_H
#define ENEMYHEALTHBAR_H
#include "Component.h"
#include <DirectXMath.h>
#include <vector>

class CRenderBillboard;
class CRenderer;
class CEnemyHealthBar : public CComponent
{
public:
	CEnemyHealthBar(CGameObject* Owner);
	~CEnemyHealthBar();

	virtual CEnemyHealthBar* clone() const;
	void OnEnable() override;
	void OnDisable() override;
	void Update(float Delta) override;
	void ReceiveEvent(EventID id, TEventData const* data) override;
	void SetOffset(float Offset) { m_Offset = Offset; }

private:
	CGameObject*						m_Owner;
	std::vector<CRenderBillboard*>		m_Billboards;
	CRenderBillboard*					m_Health;
	DirectX::XMFLOAT2					m_HealthImageSize;
	float								m_Offset;

	bool								m_bInit;
};

#endif;