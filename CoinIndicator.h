
//CoinIndicator.h
//Created July 2015

#ifndef COININDICATOR_H
#define COININDICATOR_H

#include "Component.h"
class CGameObject;
class CCoinIndicator : public CComponent
{
public:
	CCoinIndicator(CGameObject* owner);
	~CCoinIndicator();
	virtual CCoinIndicator* clone() const { return new CCoinIndicator(*this); };
	void Update(float _dt) override;
	void SetIHasMoney(bool money) { iHasMoney = money; }

private:
	CGameObject* indicator;
	float m_fValue;
	float m_fBounce;
	float m_fFrequency;
	float m_fDisplacement;
	bool iHasMoney;
};

#endif
