#include "CompanionHUD.h"
#include "RenderGUI.h"

CCompanionHUD::CCompanionHUD(CRenderGUI* RenderGUI, int Width, int Height, int ID)
{
	m_RenderGUI = RenderGUI;
	m_RenderGUI->LoadTexture("GSHealthFrame", L"Assets/Textures/GSHealthBarFrame.dds");
	m_RenderGUI->LoadTexture("GSHealth", L"Assets/Textures/GSHealthbar.dds");
	m_RenderGUI->LoadTexture("Ready", L"Assets/Textures/PlayerReady.dds");
	m_RenderGUI->LoadTexture("BaseHead", L"Assets/Textures/DragonHeadIconBase.dds");
	m_RenderGUI->LoadTexture("GSHead", L"Assets/Textures/DragonHeadIcon.dds");

	m_Width = Width;
	m_Height = Height;

	m_YOffset = 190.f; // 125.f

	m_MyID = ID;

	//CreateCompanionStats(12, "Gerzain", 1203, 1);
	//CreateCompanionStats(12, "Gerzain", 1203, 2);
	//CreateCompanionStats(12, "Gerzain", 1203, 3);
}

CCompanionHUD::~CCompanionHUD()
{
	for (unsigned int i = 0; i < m_ElementContainer.size(); i++)
	{
		m_RenderGUI->removeGUIElement(m_ElementContainer[i]);
		delete m_ElementContainer[i];
	}
}

void CCompanionHUD::ReceiveEvent(EventID id, TEventData const* data)
{
	int ID = (int)data->m[0];
	if (m_MyID != ID)
	{
		switch (data->i)
		{
		case 0:
		{
			CreateCompanionStats(ID, data->s, (int)data->m[1], data->m[2]);
			break;
		}
		case 1:
		{
			UpdateCompanionHealth(ID, data->m[1], data->m[2]);
			break;
		}
		case 2:
		{
			m_PlayerReady[ID]->RenderActive(!!data->m[1]);
			break;
		}
		default:
			break;
		}
	}
}

void CCompanionHUD::CreateCompanionStats(int ID, std::string Name,int MaxHealth, float Color)
{
	float x = 100.f;

	CGUIElement* frame = new CGUIElement(m_RenderGUI->GetTextureSRV("GSHealthFrame"), Layer(10), 0, 0, 512, 64,false);
	frame->SetScale(0.5f);
	frame->SetScreenPos(x, m_YOffset);
	m_RenderGUI->AddGUIElement(frame);
	m_ElementContainer.push_back(frame);


	// health
	CGUIElement* health = new CGUIElement(m_RenderGUI->GetTextureSRV("GSHealth"), Layer(9), 0, 0, 512, 64, false);
	health->SetScale(0.5f);
	health->SetScreenPos(x, m_YOffset);
	health->SetColor(DirectX::Colors::Red);
	m_ElementContainer.push_back(health);
	m_RenderGUI->AddGUIElement(health);

	std::wstring name;
	name.assign(Name.begin(), Name.end());

	float lenght = name.size() * 0.5f;
	
	// Player name
	CGUIElement* playerName = new CGUIElement(name, Layer(8), 0, 0, 32, 32);
	playerName->SetFontSize(0.50f);
	playerName->SetScreenPos(x + 123.f - lenght *12, m_YOffset+5.f);
	m_ElementContainer.push_back(playerName);
	m_RenderGUI->AddGUIElement(playerName);


	// Ready check mark
	CGUIElement* ready = new CGUIElement(m_RenderGUI->GetTextureSRV("Ready"), Layer(10), 0, 0, 80, 72, false);
	ready->SetScale(0.5f);
	ready->SetScreenPos(40.f, m_YOffset);
	ready->RenderActive(false);
	m_RenderGUI->AddGUIElement(ready);
	m_ElementContainer.push_back(ready);

	// Character icon
	CGUIElement* baseHead = new CGUIElement(m_RenderGUI->GetTextureSRV("BaseHead"), Layer(11), 0, 0, 272, 400, false);
	baseHead->SetScale(0.25f);
	baseHead->SetScreenPos(32.f, m_YOffset-50.f);
	m_RenderGUI->AddGUIElement(baseHead);
	m_ElementContainer.push_back(baseHead);

	CGUIElement* GSHead = new CGUIElement(m_RenderGUI->GetTextureSRV("GSHead"), Layer(11), 0, 0, 272, 400, false);
	GSHead->SetScale(0.25f);
	GSHead->SetScreenPos(32.f, m_YOffset - 50.f);

	if (Color == 1)
		GSHead->SetColor(DirectX::Colors::DeepSkyBlue);
	else if(Color == 2)
		GSHead->SetColor(DirectX::Colors::Red);
	else if (Color == 3)
		GSHead->SetColor(DirectX::Colors::LawnGreen);

	m_RenderGUI->AddGUIElement(GSHead);
	m_ElementContainer.push_back(GSHead);

	m_YOffset += 80.f;

	m_Players[ID] = health;
	m_PlayerReady[ID] = ready;
}

void CCompanionHUD::UpdateCompanionHealth(int ID, float Health, float MaxHealth)
{
	float healthRatio = (Health / MaxHealth);
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.bottom = 512;
	rect.right = LONG(512 * healthRatio);

	m_Players[ID]->SetScreenRec(rect);
}

void CCompanionHUD::SetVisibility(bool Visibility)
{
	for (unsigned int i = 0; i < m_ElementContainer.size(); i++)
		m_ElementContainer[i]->RenderActive(Visibility);
}