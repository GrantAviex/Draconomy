
//CarnageController.h
//Created May 2015

#ifndef CARNAGECONTROLLER_H
#define CARNAGECONTROLLER_H

#include "Component.h"
#include <unordered_map>
#include <DirectXMath.h>
class CCarnageController : public CComponent
{
public:
	CCarnageController(CGameObject* owner);
	~CCarnageController();

	void Update(float dt) override;
	void OnEnable() override;
	virtual CCarnageController*	clone() const;
	
private:
	std::unordered_map< CGameObject*, DirectX::XMFLOAT3 >limbs;
	std::unordered_map<CGameObject*, float> limbStop;
	float m_fTimer;
};

#endif
