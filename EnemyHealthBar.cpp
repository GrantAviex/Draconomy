#include "EnemyHealthBar.h"
#include "RenderBillboard.h"
#include "RenderBillboardManager.h"
#include "Renderer.h"
#include "Game.h"
#include "GameObject.h"
#include <DirectXColors.h>

CEnemyHealthBar::CEnemyHealthBar(CGameObject* Owner) 
	: CComponent(Owner, "EnemyHealthBar")
{
	m_bInit = false;
	AddSubscription(EventID::UpdateBillboardHealth);
}

CEnemyHealthBar::~CEnemyHealthBar()
{
	CRenderer * r = CGame::GetInstance()->GetRenderer();
	for (unsigned int i = 0; i < m_Billboards.size(); i++)
		r->RemoveBillboard(m_Billboards[i]);

}
CEnemyHealthBar* CEnemyHealthBar::clone() const
{
	CEnemyHealthBar* clone = new CEnemyHealthBar(*this);
	return clone;
}

void CEnemyHealthBar::OnEnable()
{
	m_Owner = GetOwner();
	if (!m_bInit)
	{
		//CRenderBillboard * billboard = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"HealthBar", 512, 512, m_Owner);
		//m_Billboards.push_back(billboard);
		m_HealthImageSize.x = 512.f;
		m_HealthImageSize.y = 64.f;
		m_Health = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"GSHealthBar", int(m_HealthImageSize.x), int(m_HealthImageSize.y), m_Owner);
		m_Health->SetColor(DirectX::Colors::Red);
		//RECT rect;
		//rect.left = 0;
		//rect.top = 0;
		//rect.right = 256;
		//rect.bottom = 512;
		//billboard->SetRect(rect);

		//float x = (100/512.f)*256.f - 256.f ;
		//billboard->Set2DOfffset(DirectX::XMFLOAT2(x, 0.f));
		m_Billboards.push_back(m_Health);

		CRenderBillboard * billboard = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"GSHealthBarFrame", 512, 64, m_Owner);
		m_Billboards.push_back(billboard);
		

		for (unsigned int i = 0; i < m_Billboards.size(); i++)
		{
			//m_Billboards[i]->SetSize(DirectX::XMFLOAT2(m_HealthImageSize.x, m_HealthImageSize.y));
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

		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = (long)m_HealthImageSize.x;
		rect.bottom = (long)m_HealthImageSize.y;
		m_Health->SetRect(rect);
		m_Health->SetSize(DirectX::XMFLOAT2((float)rect.right, (float)rect.bottom));
		m_Health->Set2DOfffset(DirectX::XMFLOAT2(0.f, 0.f));
	}

	m_Health->SetSize(DirectX::XMFLOAT2(m_HealthImageSize.x, m_HealthImageSize.y));
	m_Health->Set2DOfffset(DirectX::XMFLOAT2(0.f, 0.f));

}

void CEnemyHealthBar::OnDisable()
{
	for (unsigned int i = 0; i < m_Billboards.size(); i++)
		m_Billboards[i]->SetVisibility(false);
}

void CEnemyHealthBar::Update(float Delta)
{
	if (m_Owner->GetActive())
	for (unsigned int i = 0; i < m_Billboards.size(); i++)
		m_Billboards[i]->SetPosition(m_Owner->GetPosition().x, m_Owner->GetPosition().y, m_Owner->GetPosition().z);
}

void CEnemyHealthBar::ReceiveEvent(EventID id, TEventData const* data)
{
	if (data->i == GetOwner()->GetID())
	{
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = data->m[0] * m_HealthImageSize.x;
		rect.bottom = m_HealthImageSize.y;
		m_Health->SetRect(rect);
		m_Health->SetSize(DirectX::XMFLOAT2(rect.right, rect.bottom));
		float healthScale = m_HealthImageSize.x * m_Health->GetScale().x;
		float x = (data->m[0] * healthScale) - healthScale;
		m_Health->Set2DOfffset(DirectX::XMFLOAT2(x, 0.f));
	}
}