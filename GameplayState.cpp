
//GameplayState.cpp
//Created May 2015

#include "GameplayState.h"
#include <Windows.h>
#include "Game.h"
#include "Client.h"
#include <iostream>
#include <sstream>
#include "InputManager.h"
#include <iostream>
#include "MainMenuState.h"
#include "Renderer.h"
#include "RenderGUI.h"
#include "GlobalDef.h"
#include "GamePlayHUD.h"
#include "AudioSystemWwise.h"
#include "Wwise_IDs.h"
#include "MeshRenderer.h"
#include "RenderBillboard.h"
#include "RenderBillboardManager.h"
#include "Deferred/DirectionalLight.h"
#include "Deferred/PointLight.h"
#include "Deferred/LightManager.h"

using namespace GlobalDef;
CGameplayState::CGameplayState()
{
}

CGameplayState::~CGameplayState()
{

}
void CGameplayState::ReceiveEvent(EventID id, TEventData const* data)
{
	if (id == EventID::CreateExplosion)
	{
		SpawnExplosion(data->m[0], data->m[1], data->m[2]);
	}
	else if (id == EventID::ChangeTextureColor)
	{
		CGameObject* object = m_ObjMan->GetObjectByID(data->i);
		if (object == nullptr)
		{
			return;
		}
		CMeshRenderer* 	meshRenderer = (CMeshRenderer*)object->GetComponent("MeshRenderer");

		if (meshRenderer == nullptr)
		{
			for (CGameObject* child  : object->GetChildren())
			{
				meshRenderer = ((CMeshRenderer*)child->GetComponent("MeshRenderer"));
				if (meshRenderer)
				{
					if (child->GetName() != "CandyCounter")
					{
						meshRenderer->SetColorModifier(data->m[0], data->m[1], data->m[2], data->m[3]);
						meshRenderer = nullptr;
					}
				}
			}
			
		}
		else
		{
			meshRenderer->SetColorModifier(data->m[0], data->m[1], data->m[2], data->m[3]);
		}
	}
	else if (id == EventID::CursorVisibility)
	{
		if (data->i)
		{
			Cursor->RenderActive(true);
		}
		else
		{
			Cursor->RenderActive(false);

		}
	}
	else if (id == EventID::HostDisconnected)
	{
		HUD->GetRenderGUI()->FadeToBlack(0.3f);
		closing = true;
		switchOption = 1;
		CMainMenuState::GetInstance()->HostDisconnected();
	}
	else if (id == EventID::Keypress)
	{
		if (data->i == 'C')
		{
			if (!HUD->UpgradeMenuOpen() && !HUD->PauseMenuOpen())
			{
				HUD->ToggleUpgradeMenu();
				Cursor->RenderActive(HUD->PlayerUpgradeOpen());
			}
		}
		if (data->i == eInputID::eLeftClickDown)
		{
			if (m_bGameOver)
			{
				if (HUD->GetExitButtonPressed())
				{
					HUD->GetRenderGUI()->FadeToBlack(0.3f);
					closing = true;
					switchOption = 1;

					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_LOSEGAME);
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_WINGAME);
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUCLICK);
				}
			}
			else if (HUD->PauseMenuOpen())
			{
				if (HUD->GetExitButtonPressed(true))
				{
					HUD->GetRenderGUI()->FadeToBlack(0.3f);
					HUD->TogglePauseMenu();
					closing = true;
					switchOption = 1;

					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_LOSEGAME);
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_WINGAME);
					AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUCLICK);
				}
				else if (HUD->GetResumeButtonPressed())
				{
					HUD->TogglePauseMenu();
					if (!HUD->UpgradeMenuOpen() && !HUD->PlayerUpgradeOpen())
					{
						//SetCursorVisible(false);
						//while (ShowCursor(m_bShowCursor) >= 0);
						Cursor->RenderActive(false);
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_MENUCLICK);
					}
				}
				else if (HUD->GetSELessButtonPressed())
				{
				
				}
				else if (HUD->GetSEMoreButtonPressed())
				{
				}
				else if (HUD->GetMusicLessButtonPressed())
				{
				}
				else if (HUD->GetMusicMoreButtonPressed())
				{
				}
			}
			
			else if (HUD->PlayerUpgradeOpen())
			{
				HUD->UpdatePlayerUpgrade(id,data);
			}
		}
		else if (data->i == eInputID::eEscape)
		{
			if (!m_bGameOver)
			{
				HUD->TogglePauseMenu();
				if (HUD->PauseMenuOpen())
				{
					//SetCursorVisible(true);
					//while (ShowCursor(m_bShowCursor) < 0);

					Cursor->RenderActive(true);
				}
				else if (!HUD->UpgradeMenuOpen() && !HUD->PlayerUpgradeOpen())
				{
					//SetCursorVisible(false);
					//while (ShowCursor(m_bShowCursor) >= 0);
					Cursor->RenderActive(false);
				}
			}
		}
	}
	else if (id == EventID::UpdateDragonSouls)
	{
		HUD->UpdatePlayerUpgrade(id, data);
	}
	
}
/*static*/ CGameplayState* CGameplayState::GetInstance(void)
{
	static CGameplayState s_Instance;

	return &s_Instance;
}

void CGameplayState::Enter()
{
	closing = false;
	switchOption = 0;
	SetCursorVisible(false);
	ShowCursor(m_bShowCursor);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::Keypress, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::CursorVisibility, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::GamePlayStateShutdown, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::HostDisconnected, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::ChangeTextureColor, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::CreateExplosion, this);
	CGame::GetInstance()->GetEventSystem()->Subscribe(EventID::UpdateDragonSouls, this);


	m_bGameOver = false;

	prevMouse = POINT{ 0, 0 };
	m_ObjMan = CGame::GetInstance()->GetObjectManager();
	m_pClient = CGame::GetInstance()->GetClient();
	HUD = new CGamePlayHUD();
	bShowHUD = true;

	HUD->GetRenderGUI()->LoadTexture("Cursor", L"Assets/Textures/Cursor.dds");
	Cursor = new CGUIElement(HUD->GetRenderGUI()->GetTextureSRV("Cursor"), Layer(0), 0, 0, 124, 224, false);
	Cursor->SetScale(0.25f);
	Cursor->SetRotation(-0.5f);
	Cursor->RenderActive(false);
	HUD->GetRenderGUI()->AddGUIElement(Cursor);


	CGame::GetInstance()->GetClient()->SendEventMessage(EventID::GameStarted, TEventData());


	CDirectionalLight* light = new CDirectionalLight(CGame::GetInstance()->GetRenderer()->GetLightManager());
	light->SetDirection(1, -1, 1);
	light->SetIntensity(0.01f);
	m_Lights.push_back(light);
	
	//Cave light
	CPointLight* Plight = new CPointLight(CGame::GetInstance()->GetRenderer()->GetLightManager());
	Plight->SetPosition(-4000, 1000, -1300);
	Plight->SetColor(0.9f, 0.9f, 1);
	Plight->SetRadius(4000);
	Plight->SetIntensity(0.8f);
	m_Lights.push_back(Plight);
	
	//Forest light
	CPointLight* Plight1;
	Plight1 = new CPointLight(CGame::GetInstance()->GetRenderer()->GetLightManager());
	Plight1->SetPosition(-700, 800, 2900);
	Plight1->SetColor(0.8f, 0.8f, 1.0f);
	Plight1->SetRadius(2500);
	Plight1->SetIntensity(0.5f);
	m_Lights.push_back(Plight1);
	
	//Gold pile light
	Plight1 = new CPointLight(CGame::GetInstance()->GetRenderer()->GetLightManager());
	Plight1->SetPosition(0.0f, 800, 0.0f);
	Plight1->SetColor(0.9f, 0.9f, 1.0f);
	Plight1->SetRadius(2000);
	Plight1->SetIntensity(1.5f);
	m_Lights.push_back(Plight1);
	
	//Sewer light 1
	Plight1 = new CPointLight(CGame::GetInstance()->GetRenderer()->GetLightManager());
	Plight1->SetPosition(500.0f, 1000.0f, -3200.0f);
	Plight1->SetColor(0.8f, 0.8f, 1.0f);
	Plight1->SetRadius(2000);
	Plight1->SetIntensity(0.8f);
	m_Lights.push_back(Plight1);
	
	

	//Town light
	Plight1 = new CPointLight(CGame::GetInstance()->GetRenderer()->GetLightManager());
	Plight1->SetPosition(2000.0f, 1000.f, 0.0f);
	Plight1->SetColor(0.8f, 0.8f, 1.0f);
	Plight1->SetRadius(1500);
	Plight1->SetIntensity(0.8f);
	m_Lights.push_back(Plight1);



}

void CGameplayState::YouWin()
{
	HUD->ShowWin();
	m_bGameOver = true;

	if (HUD->PauseMenuOpen())
		HUD->TogglePauseMenu();
}
void CGameplayState::YouLose()
{
	HUD->ShowLose();
	m_bGameOver = true;

	if (HUD->PauseMenuOpen())
		HUD->TogglePauseMenu();
}
void CGameplayState::Exit()
{
	ShowCursor(true);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::Keypress, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::CursorVisibility, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::GamePlayStateShutdown, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::HostDisconnected, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::ChangeTextureColor, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::CreateExplosion, this);
	CGame::GetInstance()->GetEventSystem()->Unsubscribe(EventID::UpdateDragonSouls, this);

	for (unsigned int i = 0; i < m_Lights.size(); i++)
		delete m_Lights[i];
	m_Lights.clear();
	//CGame::GetInstance()->GetRenderer()->ClearBillboardManager();
	
	CEventSystem* sys = CGame::GetInstance()->GetEventSystem();
	CGame::GetInstance()->GetRenderer()->SetCameraInd(0);
	CGame::GetInstance()->GetRenderer()->ClearRenderNodesMeshRendererRefs();
	CGame::GetInstance()->SetMyPlayerID(0);
	CGame::GetInstance()->DeleteObjectManager();
	system("taskkill /F /T /IM SirCuddlesServer.exe");
	delete Cursor; //CRASH FIX
	delete HUD;
	HUD = nullptr;
	
	m_bGameOver = false;
}

bool CGameplayState::Input()
{
	return true;
}

void CGameplayState::Render()
{
	if (bShowHUD)
		CGame::GetInstance()->GetRenderer()->RenderAll(HUD->GetRenderGUI());
	else
		CGame::GetInstance()->GetRenderer()->RenderAll();
}
void  CGameplayState::HandleKey(char _key)
{
	prevKey = _key;
}
void CGameplayState::HandleMouseClick(int _mouseClick)
{
	prevMouseClick = _mouseClick;
}
void CGameplayState::Update()
{
	//TotalLights->SetText(std::to_wstring(CGame::GetInstance()->GetRenderer()->GetLightManager()->GetLightsOnScreen()));
	HUD->Update();
	Cursor->SetScreenPos(CInputManager::GetInstance()->GetMousePos().x - 10.f, CInputManager::GetInstance()->GetMousePos().y + 10.f);
	
	if (closing)
	{
		if (HUD->GetRenderGUI()->IsFaded())
		{
			switch (switchOption)
			{
				case 1:
				{
					CGame::GetInstance()->GetRenderer()->ResetAnimationManager();
					CGame::GetInstance()->GetClient()->Disconnect();
					CGame::GetInstance()->GetClient()->Shutdown();
					CGame::GetInstance()->ChangeState(CMainMenuState::GetInstance());
					break;
				}
			
			}
		}
		return;
	}
	if (!m_bGameOver)
	{
		CInputManager::GetInstance()->Update();
	}
	else
	{
		//ShowCursor(true);
		Cursor->RenderActive(true);
		CInputManager::GetInstance()->Update();
		//m_GUI.RenderGUI(m_bGameOver);
	}
	if (!CGame::GetInstance()->GetObjectManager()->GetObjectByID(CGame::GetInstance()->GetMyPlayerID())->GetActive())
	{
		CGame::GetInstance()->GetRenderer()->CheckInput();
	}
}

void CGameplayState::SpawnExplosion( float X, float Y, float Z)
{
	

	DirectX::XMFLOAT3 pos;
	pos.x = X;
	pos.x = Y;
	pos.x = Z;
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_EXPLOSION, pos);

	CRenderBillboard* billboard = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"Explosion", 2040, 2040);
	BILLBOARD_ANIMATION animation;
	animation.bAnimate = true;
	animation.bLoop = false;			// Looping animation?
	animation.cellWidth = 340;
	animation.cellHeight = 340;
	animation.imageWidth = 2040;
	animation.imageHeight = 2040;
	animation.duration = 0.3f;		// Animation duration
	animation.cellCount = 36;		// How many cells in animation
	billboard->SetAnimation(animation);
	billboard->SetAutoDestroy(true);
	billboard->SetScale(1.0f, 1.0f, true);
	billboard->SetPosition(X,Y + 400,Z);
	CGame::GetInstance()->GetRenderer()->AddBillboard(billboard);



	billboard = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"Explosion", 2040, 2040);
	animation;
	animation.bAnimate = true;
	animation.bLoop = false;			// Looping animation?
	animation.cellWidth = 340;
	animation.cellHeight = 340;
	animation.imageWidth = 2040;
	animation.imageHeight = 2040;
	animation.duration = 0.5f;		// Animation duration
	animation.cellCount = 36;		// How many cells in animation
	billboard->SetAnimation(animation);
	billboard->SetAutoDestroy(true);
	billboard->SetScale(0.5f, 0.5f, true);
	billboard->SetPosition(X, Y + 200, Z);
	CGame::GetInstance()->GetRenderer()->AddBillboard(billboard);



	billboard = new CRenderBillboard(CGame::GetInstance()->GetRenderer(), L"Explosion", 2040, 2040);
	animation;
	animation.bAnimate = true;
	animation.bLoop = false;			// Looping animation?
	animation.cellWidth = 340;
	animation.cellHeight = 340;
	animation.imageWidth = 2040;
	animation.imageHeight = 2040;
	animation.duration = 0.5f;		// Animation duration
	animation.cellCount = 36;		// How many cells in animation
	billboard->SetAnimation(animation);
	billboard->SetAutoDestroy(true);
	billboard->SetScale(1.5f, 1.5f, true);
	billboard->SetPosition(X, Y + 400, Z);
	CGame::GetInstance()->GetRenderer()->AddBillboard(billboard);

	printf("Boom");
}