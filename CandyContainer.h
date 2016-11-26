
//CandyContainer.h
//Created May 2015

#ifndef CANDYCONTAINER_H
#define CANDYCONTAINER_H

#include "Component.h"

class CCandyContainer : public CComponent
{
public:
	CCandyContainer(CGameObject* owner);
	~CCandyContainer();
	virtual CCandyContainer* clone() const;
	void ReceiveEvent(EventID id, TEventData const* data) override;

	void SetCandy(int _candy) { m_MaxCandy = _candy; m_CurrCandy = _candy; }
	int m_MaxCandy;
	int m_CurrCandy;

private:
};

#endif
