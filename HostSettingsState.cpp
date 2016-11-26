
//HostSettingsState.cpp
//Created June 2015

#include "HostSettingsState.h"
#include <windows.h>
#include "InputManager.h"
#include <iostream>
#include "Game.h"
#include "GameplayState.h"
#include "Renderer.h"
#include "GlobalDef.h"

CHostSettingsState::CHostSettingsState()
{
	prevMouse = POINT{ 0, 0 };
}

CHostSettingsState::~CHostSettingsState()
{
}
void CHostSettingsState::ReceiveEvent(EventID id, TEventData const* data)
{
	if (id == EventID::Keypress)
	{
		if (m_GUI.m_buttons.size() <= 0)
			return;
		if (data->i == 1)
		{
			if (m_GUI.m_buttons[1].selected == TRUE)
			{
				CGame::GetInstance()->ChangeState(CGameplayState::GetInstance());
			}
			//EXIT BUTTON SELECTED AND BUTTON HAS BEEN PRESSED
			else if (m_GUI.m_buttons[2].selected == TRUE)
			{
				CGame::GetInstance()->PreTerminate();
			}
		}
	}
}
 CHostSettingsState* CHostSettingsState::GetInstance(void)
{
	static CHostSettingsState s_Instance;

	return &s_Instance;
}

void CHostSettingsState::Enter()
{
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::Keypress, this);
	CButton bgTex(false);
	bgTex.SetRect(UIRectangle{ 0, 0, (float)GlobalDef::windowWidth, (float)GlobalDef::windowHeight });
	bgTex.SetDepth( 0.95f );
	bgTex.SetTextureIndex( 0 );
	float width = (float)GlobalDef::windowWidth;
	float height = (float)GlobalDef::windowHeight;
	CButton playButt(false);
	playButt.SetRect(UIRectangle{ 0.12207f * width, 0.77994f * height, 0.37792f * width, 0.88802f * height });
	playButt.SetTextureCoordinates( 0.0f, 0.0f, 1, 1 );
	playButt.SetDepth( 0.98f );
	playButt.SetTextureIndex( 3 );

	CButton ExitBut(false);
	ExitBut.SetRect(UIRectangle{ 0.62109f * width, 0.77994f * height, 0.87792f * width, 0.88802f * height });
	ExitBut.SetTextureCoordinates( 0.0f, 0.0f,1,1 );
	ExitBut.SetDepth( 0.97f );
	ExitBut.SetTextureIndex( 3 );

	m_GUI.SetRenderer( CGame::GetInstance()->GetRenderer() );
	m_GUI.LoadTexture("Assets/Textures/HostSettings.dds");
	m_GUI.m_buttons.push_back(bgTex);
	m_GUI.m_buttons.push_back(playButt);
	m_GUI.m_buttons.push_back(ExitBut);
	m_GUI.CreateRenderResources();
	m_GUI.RenderGUI( true );
}

void CHostSettingsState::Exit()
{
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::Keypress, this);
	m_GUI.ResetDataMembers();

}

bool CHostSettingsState::Input()
{
	return true;
}

void CHostSettingsState::Render()
{
	CGame::GetInstance()->GetRenderer()->RenderAll();
}
void CHostSettingsState::HandleKey(char _key)
{
	prevKey = _key;
}
void CHostSettingsState::HandleMouseClick(int _mouseClick)
{
	prevMouseClick = _mouseClick;
}
void CHostSettingsState::Update(float _dt)
{
	m_GUI.Update();
	CInputManager::GetInstance()->Update();

	POINT dapoint = CInputManager::GetInstance()->GetMousePos();
	if (prevMouse.x != dapoint.x || prevMouse.y != dapoint.y)
	{
		std::string temp = "X: ";
		temp += std::to_string(dapoint.x);
		temp += " Y: ";
		temp += std::to_string(dapoint.y);
		printf("\n");
		prevMouse = dapoint;
	}
	

	prevKey = ' ';
	prevMouseClick = 0;
	//	if (InputManager::GetInstance()->keyPressed == 'k' && InputManager::GetInstance()->)
}