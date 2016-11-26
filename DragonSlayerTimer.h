
//DragonSlayerTimer.h
//Created June 2015

#ifndef DRAGONSLAYERTIMER_H
#define DRAGONSLAYERTIMER_H
#include "Component.h"
#include <vector>
#include <unordered_map>
#include "SharedDefinitions.h"
using namespace std;

#include <DirectXMath.h>
using namespace DirectX;
class CDragonSlayerTimer : public CComponent
{
public:
	CDragonSlayerTimer(CGameObject* owner);
	~CDragonSlayerTimer();
	CDragonSlayerTimer* clone() const { return new CDragonSlayerTimer(*this); };
	void Update(float REMOVE_THIS_PARAMETER) override;
	void OnDisable() override;
	void OnEnable() override;
	void SetTimeLimit(float timeLimit) { m_fTimeLimit = timeLimit; }
	void SetTarget(int _target) { target = _target == 1; }
private:
	bool target = false;
	float m_fTimer;
	float m_fTimeLimit;
	CGameObject* dragonSlayer;
	bool  spawned = false;
};

#endif
