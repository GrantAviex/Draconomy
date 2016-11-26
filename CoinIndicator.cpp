
//CoinIndicator.cpp
//Created July 2015

#include "CoinIndicator.h"
#include "GameObject.h"

CCoinIndicator::CCoinIndicator(CGameObject* _Owner) 
	: CComponent(_Owner, "CoinIndicator")
{
	m_fValue = 0.0f;
	m_fBounce = 00.0f;
	m_fDisplacement = 5.0f;
	m_fFrequency = 6.0f;
	indicator = nullptr;
	iHasMoney = false;
}

CCoinIndicator::~CCoinIndicator()
{
}
void CCoinIndicator::Update(float _dt)
{
	if (indicator == nullptr)
	{
		for (auto child : m_Owner->GetChildren())
		{
			if (child->GetName() == "CandyCounter")
			{
				indicator = child;
			}
		}
	}
	else
	{
		indicator->SetActive(iHasMoney);
		m_fBounce += _dt;
		indicator->Rotate(0, m_fBounce * .2f, 0);
	}
}
