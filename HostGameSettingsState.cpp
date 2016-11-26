
//HostGameSettingsState.cpp
//Created June 2015

#include "HostGameSettingsState.h"
#include "LobbyState.h"
//#include <windows.h>
#include "Client.h"
#include "InputManager.h"
#include <iostream>
#include "Game.h"
#include "GameplayState.h"
#include "Renderer.h"
#include "GlobalDef.h"
#include "RenderGUI.h"
#include "PreGameState.h"
#include "AudioSystemWwise.h"
#include "Wwise_IDs.h"
#include "Button.h"

VOID startup(LPCTSTR lpApplicationName)
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcess(lpApplicationName,   // the path
		0,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block5
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi);           // Pointer to PROCESS_INFORMATION structure

	// Close process and thread handles. 
	CloseHandle(pi.hThread);
}
CHostGameSettingsState::CHostGameSettingsState()
{
}

CHostGameSettingsState::~CHostGameSettingsState()
{
}
void CHostGameSettingsState::ReceiveEvent(EventID id, TEventData const* data)
{
	if (id == EventID::Keypress)
	{
		if (data->i == 1)
		{
			if (LeftPlayerArrow->GetSelected())
			{
				LeftPlayerArrow->Clicked();
				if (MaxPlayers > 1)
				{
					MaxPlayers--;
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUTYPE);
				}
			}
			else if (RightPlayerArrow->GetSelected())
			{
				RightPlayerArrow->Clicked();
				if (MaxPlayers < 8)
				{
					MaxPlayers++;
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUTYPE);
				}
			}
			if (LeftStartArrow->GetSelected())
			{
				LeftStartArrow->Clicked();
				if (StartWave > 1)
				{
					StartWave--;
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUTYPE);
				}
			}
			else if (RightStartArrow->GetSelected())
			{
				RightStartArrow->Clicked();
				if (StartWave < EndWave)
				{
					StartWave++;
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUTYPE);
				}
			}
			if (LeftEndArrow->GetSelected())
			{
				LeftEndArrow->Clicked();
				if (EndWave > StartWave)
				{
					EndWave--;
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUTYPE);
				}
			}
			else if (RightEndArrow->GetSelected())
			{
				RightEndArrow->Clicked();
				if (EndWave < 99)
				{
					EndWave++;
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUTYPE);
				}
			}
			if (ContinueButton->GetSelected())
			{
				ContinueButton->Clicked();

				guiInterface->FadeToBlack(0.3f);

				LeftPlayerArrow->Fading(true);
				RightPlayerArrow->Fading(true);
				LeftMapArrow->Fading(true);
				RightMapArrow->Fading(true);
				LeftStartArrow->Fading(true);
				RightStartArrow->Fading(true);
				LeftEndArrow->Fading(true);
				RightEndArrow->Fading(true);
				ContinueButton->Fading(true);

				closing = true;
				switchOption = 1;
				CLobbyState::GetInstance()->SetPlayerName(name);
				CLobbyState::GetInstance()->SetHostName(name);
				CGame::GetInstance()->GetClient()->SetSettings(CLobbyState::GetInstance()->GetPlayerName(), MapIndex, MaxPlayers, StartWave - 1, EndWave);
				AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUCLICK);
			}
		}
		else if (data->i == eInputID::eEscape)
		{
			guiInterface->FadeToBlack(0.3f);

			LeftPlayerArrow->Fading(true);
			RightPlayerArrow->Fading(true);
			LeftMapArrow->Fading(true);
			RightMapArrow->Fading(true);
			LeftStartArrow->Fading(true);
			RightStartArrow->Fading(true);
			LeftEndArrow->Fading(true);
			RightEndArrow->Fading(true);
			ContinueButton->Fading(true);

			closing = true;
			switchOption = 2;
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUBACK);
		}

		else if (data->i == eInputID::eEnter)
		{
			guiInterface->FadeToBlack(0.3f);

			LeftPlayerArrow->Fading(true);
			RightPlayerArrow->Fading(true);
			LeftMapArrow->Fading(true);
			RightMapArrow->Fading(true);
			LeftStartArrow->Fading(true);
			RightStartArrow->Fading(true);
			LeftEndArrow->Fading(true);
			RightEndArrow->Fading(true);
			ContinueButton->Fading(true);

			closing = true;
			switchOption = 1;
			CLobbyState::GetInstance()->SetPlayerName(name);
			CLobbyState::GetInstance()->SetHostName(name);
			CGame::GetInstance()->GetClient()->SetSettings(CLobbyState::GetInstance()->GetPlayerName(), MapIndex, MaxPlayers, StartWave - 1, EndWave);
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUCLICK);
		}
	}
}
 CHostGameSettingsState* CHostGameSettingsState::GetInstance(void)
{
	static CHostGameSettingsState s_Instance;

	return &s_Instance;
}

void CHostGameSettingsState::Enter()
{
	prevMouse = POINT{ 0, 0 };
	guiInterface = new CRenderGUI(CGame::GetInstance()->GetRenderer());
	float width = (float)GlobalDef::windowWidth;
	float height = (float)GlobalDef::windowHeight;
	guiInterface->LoadTexture("backGround", L"Assets/Textures/HostSettings.dds");
	guiInterface->LoadTexture("Map", L"Assets/Textures/Map.dds");

	mBackGroundImg = new CGUIElement(guiInterface->GetTextureSRV("backGround"), .99f, 0, 0, 1920, 1080);
	mMapImg = new CGUIElement(guiInterface->GetTextureSRV("Map"), Layer(1), 0, 0, 275, 275);
	RECT rect;
	rect.top = 0;
	rect.left = 0;
	rect.right = (LONG)width;
	rect.bottom = (LONG)height;
	mBackGroundImg->SetScreenDisplayRect(rect);
	mBackGroundImg->SetScreenPos(0, 0.f);
	guiInterface->AddGUIElement(mBackGroundImg);
	guiInterface->AddGUIElement(mMapImg);
	mMapImg->SetScreenPos(0.4156f * width, 0.2648f * height);
	LeftMapArrow = new CButton(guiInterface, L"Assets/Textures/LeftArrow.dds", Layer(2), 65, 65);
	LeftMapArrow->SetPosition(0.385f * width, 0.36204f * height);
	LeftMapArrow->SetScale(0.5f);

	RightMapArrow = new CButton(guiInterface, L"Assets/Textures/RightArrow.dds", Layer(2), 65, 65);
	RightMapArrow->SetPosition(0.565f * width, 0.36204f * height);
	RightMapArrow->SetScale(0.5f);

	LeftPlayerArrow = new CButton(guiInterface, L"Assets/Textures/LeftArrow.dds", Layer(2), 65, 65);
	LeftPlayerArrow->SetPosition(0.495f * width, 0.555f * height);
	LeftPlayerArrow->SetScale(0.3f);

	RightPlayerArrow = new CButton(guiInterface, L"Assets/Textures/RightArrow.dds", Layer(2), 65, 65);
	RightPlayerArrow->SetPosition(0.585f * width, 0.555f * height);
	RightPlayerArrow->SetScale(0.3f);


	LeftStartArrow = new CButton(guiInterface, L"Assets/Textures/LeftArrow.dds", Layer(2), 65, 65);
	LeftStartArrow->SetPosition(0.495f * width, 0.625f * height);
	LeftStartArrow->SetScale(0.3f);

	RightStartArrow = new CButton(guiInterface, L"Assets/Textures/RightArrow.dds", Layer(2), 65, 65);
	RightStartArrow->SetPosition(0.585f * width, 0.625f * height);
	RightStartArrow->SetScale(0.3f);

	LeftEndArrow = new CButton(guiInterface, L"Assets/Textures/LeftArrow.dds", Layer(2), 65, 65);
	LeftEndArrow->SetPosition(0.495f * width, 0.695f * height);
	LeftEndArrow->SetScale(0.3f);

	RightEndArrow = new CButton(guiInterface, L"Assets/Textures/RightArrow.dds", Layer(2), 65, 65);
	RightEndArrow->SetPosition(0.585f * width, 0.695f * height);
	RightEndArrow->SetScale(0.3f);

	ContinueButton = new CButton(guiInterface, L"Assets/Textures/ContinueButton.dds", Layer(2), 250, 74);
	ContinueButton->SetPosition(0.425f * width, 0.75f * height);
	ContinueButton->SetScale(0.8f);

	mPlayerText = new CGUIElement(to_wstring(MaxPlayers), .96f, (int)(0.54f * width), (int)(0.545f * height), (int)(0.58f * width), (int)(0.585f * height), false, XMFLOAT2(0.75f, 0.75f), DirectX::Colors::Black);
	guiInterface->AddGUIElement(mPlayerText);
	mStartText = new CGUIElement(to_wstring(StartWave), .96f, (int)(0.54f * width), (int)(0.615f * height), (int)(0.58f * width), (int)(0.655f * height), false, XMFLOAT2(0.75f, 0.75f), DirectX::Colors::Black);
	guiInterface->AddGUIElement(mStartText);
	mEndText = new CGUIElement(to_wstring(EndWave), .96f, (int)(0.54f * width), (int)(0.685f * height), (int)(0.58f * width), (int)(0.725f * height), false, XMFLOAT2(0.75f, 0.75f), DirectX::Colors::Black);
	guiInterface->AddGUIElement(mEndText);

	guiInterface->LoadTexture("Cursor", L"Assets/Textures/Cursor.dds");
	Cursor = new CGUIElement(guiInterface->GetTextureSRV("Cursor"), Layer(0), 0, 0, 124, 224, false);
	Cursor->SetScale(0.25f);
	Cursor->SetRotation(-0.5f);
	guiInterface->AddGUIElement(Cursor);
	startup("../SirCuddlesServer/SirCuddlesServer.exe");
	closing = false;
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::Keypress, this);
	guiInterface->FadeIn();

	LeftPlayerArrow->Fading(false);
	RightPlayerArrow->Fading(false);
	LeftMapArrow->Fading(false);
	RightMapArrow->Fading(false);
	LeftStartArrow->Fading(false);
	RightStartArrow->Fading(false);
	LeftEndArrow->Fading(false);
	RightEndArrow->Fading(false);
	ContinueButton->Fading(false);
}
void CHostGameSettingsState::Exit()
{
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::Keypress, this);

	if (Cursor)
		delete Cursor;

	if (RightMapArrow)
	{
		delete RightMapArrow;
		RightMapArrow = nullptr;
	}
	if (LeftMapArrow)
	{
		delete LeftMapArrow;
		LeftMapArrow = nullptr;
	}
	if (RightStartArrow)
	{
		delete RightStartArrow;
		RightStartArrow = nullptr;
	}
	if (LeftStartArrow)
	{
		delete LeftStartArrow;
		LeftStartArrow = nullptr;
	}
	if (RightEndArrow)
	{
		delete RightEndArrow;
		RightEndArrow = nullptr;
	}
	if (LeftEndArrow)
	{
		delete LeftEndArrow;
		LeftEndArrow = nullptr;
	}
	if (RightPlayerArrow)
	{
		delete RightPlayerArrow;
		RightPlayerArrow = nullptr;
	}
	if (LeftPlayerArrow)
	{
		delete LeftPlayerArrow;
		LeftPlayerArrow = nullptr;
	}
	if (ContinueButton)
	{
		delete ContinueButton;
		ContinueButton = nullptr;
	}
	if (mBackGroundImg)
	{
		delete mBackGroundImg;
		mBackGroundImg = nullptr;
	}

	if (mEndText)
	{
		delete mEndText;
		mEndText = nullptr;
	}

	if (mStartText)
	{
		delete mStartText;
		mStartText = nullptr;
	}

	if (mPlayerText)
	{
		delete mPlayerText;
		mPlayerText = nullptr;
	}

	if (guiInterface)
	{
		delete guiInterface;
		guiInterface = nullptr;
	}
}

bool CHostGameSettingsState::Input()
{
	return true;
}

void CHostGameSettingsState::Render()
{
	CGame::GetInstance()->GetRenderer()->RenderAll(guiInterface);
}
void CHostGameSettingsState::HandleKey(char _key)
{
	prevKey = _key;
}
void CHostGameSettingsState::HandleMouseClick(int _mouseClick)
{
	prevMouseClick = _mouseClick;
}
void CHostGameSettingsState::Update()
{
	Cursor->SetScreenPos(CInputManager::GetInstance()->GetMousePos().x - 10.f, CInputManager::GetInstance()->GetMousePos().y + 10.f);

	guiInterface->UpdateButtons();

	float dt = CGame::GetInstance()->GetDelta();
	LeftPlayerArrow->Update(dt);
	RightPlayerArrow->Update(dt);
	LeftMapArrow->Update(dt);
	RightMapArrow->Update(dt);
	LeftStartArrow->Update(dt);
	RightStartArrow->Update(dt);
	LeftEndArrow->Update(dt);
	RightEndArrow->Update(dt);
	ContinueButton->Update(dt);

	if (closing)
	{
		if (guiInterface->IsFaded())
		{
			switch (switchOption)
			{
				case 1:
				{
					if (CGame::GetInstance()->GetClient()->ManuallyConnect("127.0.0.1"))
					{

						CGame::GetInstance()->ChangeState(CLobbyState::GetInstance());
					}
					else
					{
						guiInterface->FadeIn();
						closing = false;						
					}
					break;
				}
				case 2:
				{
					system("taskkill /F /T /IM SirCuddlesServer.exe");
					CGame::GetInstance()->ChangeState(CPreGameState::GetInstance());
					break;
				}
			}
		}
		return;
	}
	mPlayerText->SetText(to_wstring(MaxPlayers));
	mStartText->SetText(to_wstring(StartWave));
	mEndText->SetText(to_wstring(EndWave));
	CInputManager::GetInstance()->Update();

	//	if (InputManager::GetInstance()->keyPressed == 'k' && InputManager::GetInstance()->)
}