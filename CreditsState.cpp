#include "CreditsState.h"
#include "InputManager.h"
#include <iostream>
#include "Game.h"
#include "PreGameState.h"
#include "Renderer.h"
#include "GlobalDef.h"
#include "RenderGUI.h"
#include "AudioSystemWwise.h"
#include "Wwise_IDs.h"
#include "Button.h"
#include "MainMenuState.h"

CCreditsState::CCreditsState()
{
	closing = false;
	float width = GlobalDef::windowWidth;
	float height = GlobalDef::windowHeight;
	guiInterface = new CRenderGUI(CGame::GetInstance()->GetRenderer());

	guiInterface->LoadTexture("backGround", L"Assets/Textures/Credits.dds");
	guiInterface->LoadTexture("exitButton", L"Assets/Textures/QuitButton.dds");
	guiInterface->LoadTexture("dcPopup", L"Assets/Textures/DisconnectedPopup.dds");



	mmBackGroundImg = new CGUIElement(guiInterface->GetTextureSRV("backGround"), .99f, 0, 0, 632, 3000, true, false);
	RECT rect;
	rect.top = GlobalDef::windowHeight;
	rect.bottom = 3000;
	rect.right = 632;
	rect.left = GlobalDef::windowWidth * .5f - 316;
	mmBackGroundImg->SetScreenDisplayRect(rect);
	guiInterface->AddGUIElement(mmBackGroundImg);


	QuitButton = new CButton(guiInterface, L"Assets/Textures/QuitButton.dds", Layer(2), 250, 76);
	QuitButton->SetPosition(0.825f * width, 0.85f * height);
	QuitButton->SetVisibility(false);
	mmDisconnectedImg = new CGUIElement(guiInterface->GetTextureSRV("dcPopup"), .98f, 0, 0, 356, 188);
	mmDisconnectedImg->SetScreenPos(0.41f * width, 0.25f * height);
	guiInterface->AddGUIElement(mmDisconnectedImg);

	guiInterface->LoadTexture("Cursor", L"Assets/Textures/Cursor.dds");
	Cursor = new CGUIElement(guiInterface->GetTextureSRV("Cursor"), Layer(0), 0, 0, 124, 224, false);
	Cursor->SetScale(0.25f);
	Cursor->SetRotation(-0.5f);
	guiInterface->AddGUIElement(Cursor);

}

CCreditsState::~CCreditsState()
{
	if (Cursor)
		delete Cursor;

	if (mmBackGroundImg)
	{
		delete mmBackGroundImg;
		mmBackGroundImg = nullptr;
	}

	if (QuitButton)
	{
		delete QuitButton;
		QuitButton = nullptr;
	}

	if (mmDisconnectedImg)
	{
		delete mmDisconnectedImg;
		mmDisconnectedImg = nullptr;
	}

	if (guiInterface)
	{
		delete guiInterface;
		guiInterface = nullptr;
	}
}
void CCreditsState::HostDisconnected()
{
	hostDisconnected = true;
}
void CCreditsState::ReceiveEvent(EventID id, TEventData const* data)
{
	if (id == EventID::Keypress)
	{
		if (hostDisconnected)
		{
			hostDisconnected = false;
		}
		if (data->i == 5)
		{
			QuitButton->Clicked();
			guiInterface->FadeToBlack(0.3f);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUBACK);
			closing = 1;
		}
		else if (data->i == 1)
		{
			// Step4: use CGUIElement::GetSelected method to check if the mouse is over the button
			if (CGame::GetInstance()->GetHTP())
			{
				CGame::GetInstance()->ToggleHTP();
			}
			else
			{
				if (QuitButton->GetSelected())
				{
					QuitButton->Clicked();
					guiInterface->FadeToBlack(0.3f);
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUBACK);
					closing = 1;

				}
			}

		}
	}
}
CCreditsState* CCreditsState::GetInstance(void)
{
	static CCreditsState s_Instance;

	return &s_Instance;
}

void CCreditsState::Enter()
{
	closing = false;
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::Keypress, this);
	guiInterface->FadeIn();
	RECT rect;
	rect.top = GlobalDef::windowHeight;
	rect.bottom = 3000;
	rect.right = 632;
	rect.left = GlobalDef::windowWidth * .5f - 316;
	mmBackGroundImg->SetScreenDisplayRect(rect);
	QuitButton->Fading(false);
	QuitButton->SetVisibility(false);
}

void CCreditsState::Exit()
{
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::Keypress, this);
}

bool CCreditsState::Input()
{
	return true;
}

void CCreditsState::Render()
{
	// Step2: Call the overloaded RenderAll that takes in a guiInterface
	CGame::GetInstance()->GetRenderer()->RenderAll(guiInterface);
}
void CCreditsState::HandleKey(char _key)
{
	prevKey = _key;
}
void CCreditsState::HandleMouseClick(int _mouseClick)
{
	prevMouseClick = _mouseClick;
}
void CCreditsState::Update()
{
	guiInterface->UpdateButtons();

	float dt = CGame::GetInstance()->GetDelta();
	
	RECT rect;
	rect = mmBackGroundImg->GetScreenRec();
	rect.top -= dt * 125;
	if (rect.top < -2568 && closing == FALSE)
	{
		QuitButton->SetVisibility(true);
		rect.top = -2568;
	//	closing = 1;
	//	guiInterface->FadeToBlack(0.3f);
	}
	//rect.bottom = 2568;
	//rect.right = GlobalDef::windowWidth;
	//rect.left = 0;
	//mmBackGroundImg->SetScreenRec(rect);
	mmBackGroundImg->SetScreenDisplayRect(rect);

	QuitButton->Update(dt);

	Cursor->SetScreenPos(CInputManager::GetInstance()->GetMousePos().x - 10.f, CInputManager::GetInstance()->GetMousePos().y + 10.f);

	mmDisconnectedImg->RenderActive(hostDisconnected);
	if (closing)
	{
		if (guiInterface->IsFaded())
		{
			CGame::GetInstance()->ChangeState(CMainMenuState::GetInstance());
			return;
		}
		return;
	}
	CInputManager::GetInstance()->Update();

}