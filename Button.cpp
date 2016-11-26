#include "Button.h"
#include "RenderGUI.h"
#include "UIFrame.h"
#include <DirectXColors.h>

CButton::CButton(CRenderGUI* RenderGui, std::wstring TextureName, float Layer, int Width, int Height, bool Frame, FrameType Type)
{
	m_RenderGUI = RenderGui;
	m_bActive = true;
	std::string texture(TextureName.begin(), TextureName.end());
	m_RenderGUI->LoadTexture(texture, TextureName);

	m_Button = new CGUIElement(m_RenderGUI->GetTextureSRV(texture), Layer, 0, 0, Width, Height, false, true);
	m_RenderGUI->AddGUIElement(m_Button);

	m_bClicked = false;
	m_bFading = false;

	m_Width = Width;
	m_Height = Height;

	m_Layer = Layer;

	if (Frame)
	{
		m_FrameType = int(Type);
		m_Frame = new CUIFrame(RenderGui);
		m_Frame->SetFrame(Type, Width, Height);
		m_Frame->SetLayer(Layer);
		m_Frame->SetBackgroundColor(DirectX::Colors::Transparent);
	}
	else
		m_Frame = nullptr;
}

CButton::~CButton()
{
	delete m_Frame;
	m_RenderGUI->removeGUIElement(m_Button);
	//delete m_Button;
}

void CButton::Update(float Delta)
{
	if (!m_bFading)
	{
		if (m_bClicked)
		{
			if ((m_ClickedTimer += Delta) > 0.1f)
				m_bClicked = false;
		}
		else
		{
			if (m_Button->GetSelected())
			{
				DirectX::XMVECTOR c;
				c.m128_f32[0] = 1.3f;
				c.m128_f32[1] = 1.3f;
				c.m128_f32[2] = 1.3f;
				c.m128_f32[3] = 1.f;
				m_Button->SetColor(c);
			}
			else
				m_Button->SetColor(DirectX::Colors::White);
		}
	}
}

void CButton::SetPosition(float X, float Y)
{
	m_Button->SetScreenPos(X, Y);
	if (m_Frame)
		m_Frame->SetPosition(X, Y);
}

bool CButton::GetSelected()
{
	if (!m_bActive)
	{
		return false;
	}
	return m_Button->GetSelected();
}

void CButton::Clicked()
{
	m_bClicked = true;
	m_ClickedTimer = 0.f;
	m_Button->SetColor(DirectX::Colors::Red);

}

// Used to when fading in and out of menus
void CButton::Fading(bool Fading)
{
	m_bFading = Fading;
	m_Button->SetColor(DirectX::Colors::White);
}

void CButton::SetScale(float Scale)
{
	m_Button->SetScale(Scale);
	
	if (m_Frame)
	{
		delete m_Frame;
		
		m_Frame = new CUIFrame(m_RenderGUI);
		m_Frame->SetFrame((FrameType)m_FrameType, int(m_Width * Scale),int(m_Height * Scale));
		m_Frame->SetPosition(m_Button->GetScreenPosX(), m_Button->GetScreenPosY());
		m_Frame->SetLayer(m_Layer);
		m_Frame->SetBackgroundColor(DirectX::Colors::Transparent);
	}
}

void CButton::SetVisibility(bool Visibility)
{
	m_bActive = Visibility;
	m_Button->RenderActive(Visibility);
	if (m_Frame)
		m_Frame->SetVisibility(Visibility);
}