#include "AnimatedBillboard.h"
#include "RenderBillboard.h"
#include "Renderer.h"
#include "Game.h"
#include "GameObject.h"
#include <DirectXColors.h>
CAnimatedBillboard::CAnimatedBillboard(CGameObject* Owner)
:CComponent(Owner, "AnimatedBillboard")
{
	m_bInit = false;
}
CAnimatedBillboard::~CAnimatedBillboard()
{
	CGame::GetInstance()->GetRenderer()->RemoveBillboard(m_Billboard);
}

CAnimatedBillboard* CAnimatedBillboard::clone() const
{
	CAnimatedBillboard* clone = new CAnimatedBillboard(*this);
	return clone;
}

void CAnimatedBillboard::OnEnable()
{
	if (!m_bInit)
	{
		m_bInit = true;

		m_Billboard = new CRenderBillboard(CGame::GetInstance()->GetRenderer(),m_FileName, m_Width, m_Height,GetOwner());
		
		BILLBOARD_ANIMATION animation;
		animation.bAnimate = true;
		animation.bLoop = true;			
		animation.cellWidth = m_CellWidth;
		animation.cellHeight = m_CellHeight;
		animation.imageWidth = m_Width;
		animation.imageHeight = m_Height;
		animation.duration = m_Duration;		
		animation.cellCount = m_CellCount;		
		m_Billboard->SetAnimation(animation);
		m_Billboard->SetScale(m_ScaleX, m_ScaleY, true);
		m_Billboard->SetOffset(m_Offset);
		m_Billboard->SetRelativePosition(m_RelativePosition.x, m_RelativePosition.y, m_RelativePosition.z);
		m_Billboard->SetUseWorld(m_UseWorld);
		m_Billboard->SetRotation(m_Rotation);
		CGame::GetInstance()->GetRenderer()->AddBillboard(m_Billboard);
	}
	else
	{
		m_Billboard->SetOwner(m_Owner);
	}
}

void CAnimatedBillboard::OnDisable()
{
	if (m_Billboard)
	{
		m_Billboard->SetVisibility(false);
		m_Billboard->SetOwner(nullptr);
	}
}

void CAnimatedBillboard::Update(float Delta)
{

}

void CAnimatedBillboard::ReceiveEvent(EventID id, TEventData const* data)
{

}
void CAnimatedBillboard::SetRelativeLocation(float X, float Y, float Z)
{
	if (m_Billboard)
		m_Billboard->SetRelativePosition(X, Y, Z);
	else
	{
		m_RelativePosition.x = X;
		m_RelativePosition.y = Y;
		m_RelativePosition.z = Z;
	}
}

void CAnimatedBillboard::SetUseWorld(float World)
{
	if (m_Billboard)
		m_Billboard->SetUseWorld(World);
	else
		m_UseWorld = World;
}

void CAnimatedBillboard::SetRotation(float Rotation)
{
	if (m_Billboard)
		m_Billboard->SetRotation(Rotation);
	else 
		m_Rotation = Rotation;
}
