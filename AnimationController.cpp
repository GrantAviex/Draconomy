
//AnimationController.cpp
//Created August 2015

#include "AnimationController.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include "Manager.h"
#include "Server.h"
#include "EventSystem.h"
#include "AnimationComponent.h"

CAnimationController::CAnimationController(CGameObject* _Owner)
	: CComponent(_Owner, "AnimationController")
{
	currentAnimation = "Unset";
	m_fKillTimer = 0.0f;
}

CAnimationController::~CAnimationController()
{
}

void CAnimationController::ReceiveEvent(EventID id, TEventData const* data)
{
}

void CAnimationController::OnEnable()
{
	currentAnimation = "Unset";
	for (auto& child : m_Owner->GetChildren())
	{
		child->SetActive(true);
		child->SetActive(false);
	}
}
void CAnimationController::OnDisable()
{
	for (auto& child : m_Owner->GetChildren())
	{
		child->SetActive(false);
	}
}
void CAnimationController::Kill()
{
	m_fKillTimer = 1.45f;
	nextAnimation = "Death";
}
void CAnimationController::Revive()
{
	m_Owner->SetActive(true);
	m_fKillTimer = 0.0f;
	nextAnimation = "Idle";
}
void CAnimationController::Update(float _dt)
{
	if (m_fKillTimer > 0.0f)
	{
		m_fKillTimer -= _dt;
		if (m_fKillTimer < 0.0f)
		{
			m_Owner->SetActive(false);
			CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID::PlayerDead, TEventData(), m_Owner->GetFollower()->GetID());
		}
	}
	if (currentAnimation != nextAnimation)
	{
		if (currentAnimation == "Unset")
		{
			for (auto& child : m_Owner->GetChildren())
			{
				CAnimationComponent* anim = (CAnimationComponent*)child->GetComponent("AnimationComponent");
				if (anim)
				{
					child->SetActive(false);
				}
				if (child->GetName() == nextAnimation)
				{
					child->SetActive(true);
				}
				currentAnimation = nextAnimation;
			}
		}
		else if (currentAnimation != nextAnimation)
		{
			for (auto& child : m_Owner->GetChildren())
			{
				if (child->GetName() == currentAnimation)
				{
					child->SetActive(false);
				}
				if (child->GetName() == nextAnimation)
				{
					child->SetActive(true);
				}
			}
			currentAnimation = nextAnimation;
		}
	}
}
void CAnimationController::SetStartAnimation(string name)
{
	startAnimation = name;
	nextAnimation = name;
}
void CAnimationController::SetAnimation(string name)
{ 
	nextAnimation = name;
}