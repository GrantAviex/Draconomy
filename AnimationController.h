
//AnimationController.h
//Created August 2015

#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <DirectXMath.h>
using namespace DirectX;
#include "Component.h"

class CAnimationController : public CComponent
{
public:
	CAnimationController(CGameObject* owner);
	~CAnimationController();
	virtual CAnimationController* clone() const { return new CAnimationController(*this); };
	void Update(float _dt) override;
	void ReceiveEvent(EventID id, TEventData const* data) override;
	void OnEnable() override;
	void OnDisable() override;
	void SetAnimation(string name);
	void SetStartAnimation(string name);
	string GetCurrentAnimation() { return currentAnimation; }
	void Kill();
	void Revive();
private:
	float m_fKillTimer = 0.0f;
	string currentAnimation;
	string startAnimation;
	string nextAnimation;
};

#endif
