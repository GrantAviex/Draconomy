
//Game.cpp
//Created May 2015

#include "Game.h"
#include <stdio.h>
#include <fstream>
#include "shlobj.h"
//#include "GUI.h"
#include "Client.h"
#include "IGameState.h"
#include "GameObject.h"
#include "IntroState.h"
#include "GameplayState.h"
#include "AudioSystemWwise.h"
#include "AssetManager.h"
#include "AssetLoader.h"
#include "Wwise_IDs.h"
#include "Deformer.h"
#include "Animation.h"
#include "Interpolator.h"
#include "FBXLoader.h"
#include "Renderer.h"
#include "XTime.h"
#include "ObjectManager.h"
#include "InputManager.h"
#include "LocalUpdateManager.h"
#include "GlobalDef.h"
//#include "vld.h"
using namespace std;
CGame* CGame::s_pInstance = nullptr;
XTime* CGame::s_pTimer = nullptr;

CGame* CGame::GetInstance(void)
{
	if (s_pInstance == nullptr)
		s_pInstance = new CGame();

	return s_pInstance;
}

XTime* CGame::GetTimer(void)
{
	if (s_pTimer == nullptr)
		s_pTimer = new XTime();

	return s_pTimer;
}

float CGame::GetDelta()
{
	return TimeBucket;
}

void CGame::ChangeState(IGameState* pNewState)
{
	// Exit the old state
	if (m_gameState != nullptr)
		m_gameState->Exit();

	// Store the new state
	m_gameState = pNewState;

	if (pNewState == CGameplayState::GetInstance())
	{
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_MENUBACKGROUND);
		m_pAudioSystem->UnRegisterListener(dummyListener);
		AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_GAMEBACKGROUND);
		stateID = CurrentStateID::GameplayStateID;
		menu = false;
	}
	else
	{
		if (dummyListener && !menu)
		{
			m_pAudioSystem->RegisterListener(dummyListener, "test");
			AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_MENUBACKGROUND);
			menu = true;
		}
		stateID = CurrentStateID::MainMenuStateID;
	}

	// Enter the new state
	if (m_gameState != nullptr)
		m_gameState->Enter();
}

void CGame::DeleteInstance(void)
{
	delete s_pInstance;
	s_pInstance = nullptr;
}
void CGame::DeleteTimer(void)
{
	delete s_pTimer;
	s_pTimer = nullptr;
}
void CGame::DeleteObjectManager()
{
	delete m_ObjMan;
	m_ObjMan = new CObjectManager();
	m_ObjMan->SetAssetManager(CGame::GetInstance()->GetAssestManager());
}

bool CGame::Initialize()
{

	ZeroMemory(&pi, sizeof(pi));
	stateID = CurrentStateID::MainMenuStateID;
	eventSystem = new CEventSystem();
	m_assetManager = new CAssetManager();
	LoadPrefabs(m_assetManager, "../Resources/");
	LoadModels(m_assetManager, "Assets//Models/");
	//_assetManager->LoadModel("Assets/Models/LevelTown1.fbx", 0);
	//_assetManager->LoadModel("Assets/Models/LevelForest.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelForest.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelForestFloor.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelForestBackdrop.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelFort.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelFortFloor.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelPath.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelSewer.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelSewerGrate.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelSewerFloor.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelTown1.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelTown2.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelTown3.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelTown4.fbx", 0);
	//m_assetManager->LoadModel("Assets/Models/LevelTownFloor.fbx", 0);
	m_assetManager->ExportAnimationsToBinary();
	m_assetManager->ExportMeshesToBinary();
	m_pRenderer->FinishQueue_RDM();

	m_pAudioSystem = new AudioSystemWwise;
	m_pAudioSystem->Initialize();
	m_pAudioSystem->SetBasePath(L"SoundBanks/"); //Path Specific
	bool	tru = m_pAudioSystem->LoadSoundBank(L"Init.bnk");
	tru = m_pAudioSystem->LoadSoundBank(L"SoundBank.bnk");
	m_pAudioSystem->SetWorldScale(50);

	LoadConfig();
	ChangeState(CIntroState::GetInstance());

	m_pRenderer->SetAssetManager(m_assetManager);
	m_pRenderer->InitializeDeubgRendering(m_assetManager);

	ClientUpdater = 0;
	TimeBucket = 1.0f / 60.0f;
	m_pClient = new CClient();
	m_ObjMan = new CObjectManager();
	m_ObjMan->SetAssetManager(CGame::GetInstance()->GetAssestManager());
	GetTimer()->Signal();
	dummyListener = m_assetManager->GetPrefab("Level2Collision");
	m_pAudioSystem->RegisterListener(dummyListener, "test");
	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_MENUBACKGROUND);
	CInputManager::GetInstance()->SuscribeToEventSystem();
	CInputManager::GetInstance()->CursorVisibility(false);

	return true;
}

void CGame::LoadConfig()
{
	//Default values
	int startupCount = 0;
	int width = 0;
	int height = 0;
	float soundVolume = 0.0f;
	float musicVolume = 0.0f;

	//Load file
	PWSTR str = nullptr;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &str);
	wstring directory(str);
	directory += L"\\Draconomy\\";
	string d(directory.begin(), directory.end());
	CreateDirectory(d.c_str(), NULL);
	wstring s(directory);
	s += L"config.txt";
	ifstream file(s);
	if (file.is_open())
	{
		char l1[20];
		char l2[20];
		char l3[20];
		char l4[20];
		char l5[20];
		file.getline(l1, 20);
		file.getline(l2, 20);
		file.getline(l3, 20);
		file.getline(l4, 20);
		file.getline(l5, 20);
		int i1 = stoi(l1);
		int i2 = stoi(l2);
		int i3 = stoi(l3);
		int i4 = stoi(l4);
		int i5 = stoi(l5);
		startupCount = i1;
		width = i2;
		height = i3;
		soundVolume = (float)i4;
		musicVolume = (float)i5;
		file.close();
	}
	else
	{
		//Create file
		ofstream f(s);
		if (f.is_open())
		{
			f << "0\n";
			f << "1920\n";
			f << "1080\n";
			f << "80\n";
			f << "80\n";
			f.close();
		}
	}

	//Use default values
	if (width == 0 || height == 0)
	{
		startupCount = 0;
		width = 1920;
		height = 1080;
		soundVolume = 80.0f;
		musicVolume = 80.0f;
	}

	//Set global data
	m_Options_StartupCount = startupCount;
	m_Options_ResolutionWidth = width;
	m_Options_ResolutionHeight = height;
	m_Options_SoundVolume = (int)soundVolume;
	m_Options_MusicVolume = (int)musicVolume;

	//Set audio volume
	CGame::GetInstance()->GetAudioSystem()->SetRTCPValue(AK::GAME_PARAMETERS::FX_VOLUME, soundVolume);
	CGame::GetInstance()->GetAudioSystem()->SetRTCPValue(AK::GAME_PARAMETERS::MX_VOLUME, musicVolume);

	//Set resolution
	CGame::GetInstance()->GetRenderer()->ChangeResolution(width, height);
}

void CGame::SaveConfig()
{
	//Load file
	PWSTR str = nullptr;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, 0, &str);
	wstring s(str);
	s += L"\\Draconomy\\config.txt";
	ofstream file(s, ofstream::trunc);
	if (file.is_open())
	{
		file << m_Options_StartupCount + 1 << "\n";
		file << m_Options_ResolutionWidth << "\n";
		file << m_Options_ResolutionHeight << "\n";
		file << m_Options_SoundVolume << "\n";
		file << m_Options_MusicVolume;
		file.close();
	}
}

int CGame::Update()
{
	GetTimer()->Signal();
	ClientUpdater += GetTimer()->Delta();
	if (m_bTerminated)
		return 1;


	if (m_gameState->Input() == false)
		return 1;


	if (m_bTerminated)
		return 1;
	eventSystem->Dispatch();

	m_pClient->Update();
	IGameState* currentState = m_gameState;
	if (m_gameState == currentState)
	{
		if (ClientUpdater > TimeBucket)
		{
			m_gameState->Update();
			m_gameState->Render();
			m_pAudioSystem->Update();

			if (!m_bTerminated && GetCurrentState() == CurrentStateID::GameplayStateID)
			{
				m_pLocalUpdateManager->UpdateEmitters(TimeBucket);
				m_pLocalUpdateManager->UpdateAnimators(TimeBucket);
				m_pLocalUpdateManager->UpdateCarnage(TimeBucket);
			}
			ClientUpdater -= TimeBucket;
		}
	}
	if (m_bTerminated)
	{
		return 1;
	}
	return 0;
}

void CGame::Terminate(void)
{
	SaveConfig();

	if (m_pClient)
	{
		delete m_pClient;
	}
	if (m_ObjMan)
	{
		delete m_ObjMan;
		m_ObjMan = nullptr;
	}
	if (GlobalDef::exportingToBinary)
	{
		m_assetManager->ExportMeshesToBinary();
		m_assetManager->ExportAnimationsToBinary();
	}
	m_pAudioSystem->Shutdown();
	m_bTerminated = true;
	m_gameState->Exit();
	delete eventSystem;
	delete m_pAudioSystem;
	delete m_assetManager;
	CInputManager::GetInstance()->CursorVisibility(true);
}

void CGame::SetMyPlayerID(unsigned int _uMyPlayer)
{
	m_uMyPlayer = _uMyPlayer;
}

unsigned int CGame::GetMyPlayerID()
{
	return m_uMyPlayer;
}

CGame::CGame(void)
{
	m_Options_StartupCount = 0;
	m_Options_ResolutionWidth = 1920;
	m_Options_ResolutionHeight = 1080;
	m_Options_SoundVolume = 100;
	m_Options_MusicVolume = 100;
};
