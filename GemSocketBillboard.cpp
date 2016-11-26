#include "GemSocketBillboard.h"
#include <DirectXColors.h>
#include "RenderBillboard.h"
#include "RenderBillboardManager.h"
#include "Renderer.h"
#include "Game.h"
#include "GameObject.h"

CGemSocketBillboard::CGemSocketBillboard(CGameObject* Owner)
	: CComponent(Owner, "GemSocketBillboard")
{
	m_bInit = false;
	AddSubscription(EventID::SetGemInBillboard);
}

CGemSocketBillboard::~CGemSocketBillboard()
{
	CRenderer * r = CGame::GetInstance()->GetRenderer();
	for (unsigned int i = 0; i < m_Billboards.size(); i++)
		r->RemoveBillboard(m_Billboards[i]);
}


CGemSocketBillboard* CGemSocketBillboard::clone() const
{
	CGemSocketBillboard* clone = new CGemSocketBillboard(*this);
	return clone;
}
void CGemSocketBillboard::OnEnable()
{
	m_Owner = GetOwner();
	if (!m_bInit)
	{
		m_FirstGem = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"GSGem", 128, 128, m_Owner);
		m_FirstGem->Set2DOfffset(DirectX::XMFLOAT2(-128.f*0.25f, 0.f));
		m_Billboards.push_back(m_FirstGem);

		m_SecondGem = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"GSGem", 128, 128, m_Owner);
		m_SecondGem->Set2DOfffset(DirectX::XMFLOAT2(128.f*0.25f, 0.f));
		m_Billboards.push_back(m_SecondGem);

		m_FirstGem->SetVisibility(false);
		m_SecondGem->SetVisibility(false);

		CRenderBillboard * billboard = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"GemSocket", 128, 128, m_Owner);
		billboard->Set2DOfffset(DirectX::XMFLOAT2(-128.f*0.25f, 0.f));
		m_Billboards.push_back(billboard);

		billboard = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"GemSocket", 128, 128, m_Owner);
		billboard->Set2DOfffset(DirectX::XMFLOAT2(128.f*0.25f, 0.f));
		m_Billboards.push_back(billboard);

		
		for (unsigned int i = 0; i < m_Billboards.size(); i++)
		{
			CGame::GetInstance()->GetRenderer()->AddBillboard(m_Billboards[i]);
			m_Billboards[i]->SetOffset(m_Offset);
			m_Billboards[i]->SetScale(0.25f);
		}

		m_bInit = true;
	}
	else
	{
		for (unsigned int i = 0; i < m_Billboards.size(); i++)
		{
			m_Owner = GetOwner();
			m_Billboards[i]->SetVisibility(true);
			m_Billboards[i]->SetOwner(m_Owner);
		}

		m_FirstGem->SetVisibility(false);
		m_SecondGem->SetVisibility(false);
	}

}
void CGemSocketBillboard::OnDisable()
{
	for (unsigned int i = 0; i < m_Billboards.size(); i++)
		m_Billboards[i]->SetVisibility(false);

}
void CGemSocketBillboard::Update(float Delta)
{
	if (m_Owner->GetActive())
		for (unsigned int i = 0; i < m_Billboards.size(); i++)
			m_Billboards[i]->SetPosition(m_Owner->GetPosition().x, m_Owner->GetPosition().y, m_Owner->GetPosition().z);
}
void CGemSocketBillboard::ReceiveEvent(EventID id, TEventData const* data)
{
	if (data->i == GetOwner()->GetID())
	{
		/*enum TGems
		{
			fireGem = 1,
			iceGem,
			lightningGem,
			rangeGem,
			aoeGem,
			multiShotGem,
			curseGem
		};*/

		if (data->m[0] == 0)
		{
			m_FirstGem->SetVisibility(true);
			if (data->m[1] == 1.f)
				m_FirstGem->SetColor(DirectX::Colors::Red);
			if (data->m[1] == 2.f)
				m_FirstGem->SetColor(DirectX::Colors::Blue);
			if (data->m[1] == 3.f)
				m_FirstGem->SetColor(DirectX::Colors::Yellow);
			if (data->m[1] == 4.f)
				m_FirstGem->SetColor(DirectX::Colors::Purple);
			if (data->m[1] == 5.f)
				m_FirstGem->SetColor(DirectX::Colors::Orange);
			if (data->m[1] == 6.f)
				m_FirstGem->SetColor(DirectX::Colors::Green);
			if (data->m[1] == 7.f)
				m_FirstGem->SetColor(DirectX::Colors::Gray);
		}
		else if (data->m[0] == 1)
		{
			m_SecondGem->SetVisibility(true);
			if (data->m[1] == 1.f)
				m_SecondGem->SetColor(DirectX::Colors::Red);
			if (data->m[1] == 2.f)
				m_SecondGem->SetColor(DirectX::Colors::Blue);
			if (data->m[1] == 3.f)
				m_SecondGem->SetColor(DirectX::Colors::Yellow);
			if (data->m[1] == 4.f)
				m_SecondGem->SetColor(DirectX::Colors::Purple);
			if (data->m[1] == 5.f)
				m_SecondGem->SetColor(DirectX::Colors::Orange);
			if (data->m[1] == 6.f)
				m_SecondGem->SetColor(DirectX::Colors::Green);
			if (data->m[1] == 7.f)
				m_SecondGem->SetColor(DirectX::Colors::Gray);
		}
	}
}