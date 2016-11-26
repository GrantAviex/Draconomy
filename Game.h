
//Game.h
//Created May 2015
#include "EventSystem.h"
#include <Windows.h>
#ifndef GAME_H
#define GAME_H

enum CurrentStateID
{
	GameplayStateID, MainMenuStateID, HTPStateID
};
class CButton;
class IGameState;
class AudioSystemWwise;
class CAssetManager;
class CCollisionManager;
class CRenderer;
class XTime;
class CObjectManager;
class CClient;
class CLocalUpdateManager;
class CGameObject;
class CGame
{
public:
	static CGame*	GetInstance(void);
	static void		DeleteInstance(void);
	static XTime*	GetTimer(void);
	static void		DeleteTimer(void);
	PROCESS_INFORMATION* GetProcessInfo() { return &pi; }
	float GetDelta();
	void ChangeState(IGameState* pNewState);
	bool Initialize();
	int	 Update();
	void Terminate(void);
	void PreTerminate() { m_bTerminated = true; }
	void ToggleHTP() { m_bHTP = !m_bHTP; }
	bool GetHTP() { return m_bHTP; }
	CObjectManager* GetObjectManager() { return m_ObjMan; }
	CAssetManager* GetAssestManager() { return m_assetManager; }
	CRenderer* GetRenderer() { return m_pRenderer; }
	void SetRenderer(CRenderer* renderer) { m_pRenderer = renderer; }
	CEventSystem* GetEventSystem() { return eventSystem; }
	CClient* GetClient() { return m_pClient; }
	AudioSystemWwise* GetAudioSystem() { return m_pAudioSystem; }
	CurrentStateID GetCurrentState() { return stateID; }
	unsigned char GetMaxPlayers() { return m_uMaxPlayers; }
	void SetMaxPlayers(unsigned char _uMaxPlayers) { m_uMaxPlayers = _uMaxPlayers; }
	void SetMyPlayerID(unsigned int _uMyPlayer);
	unsigned int GetMyPlayerID();
	CLocalUpdateManager* GetLocalUpdateManager() { return m_pLocalUpdateManager; }
	void SetLocalUpdateManager( CLocalUpdateManager* _LUM ) { m_pLocalUpdateManager = _LUM; }
	void DeleteObjectManager();

	void	SetLoadout(int Loadout)		{ loadout = Loadout; }
	int		GetLoadout()				{ return loadout; }

	//Config data
	int m_Options_StartupCount;
	int m_Options_ResolutionWidth;
	int m_Options_ResolutionHeight;
	int m_Options_SoundVolume;
	int m_Options_MusicVolume;

private:
	bool menu = true;
	unsigned int m_uMyPlayer;
	unsigned char m_uMaxPlayers;
	PROCESS_INFORMATION pi;
	bool m_bHTP = false;
	bool m_bTerminated = false;
	CObjectManager* m_ObjMan;
	CEventSystem* eventSystem;
	CAssetManager* m_assetManager;
	static XTime* s_pTimer;
	static CGame*	s_pInstance;
	double ClientUpdater = 0;
	float TimeBucket;
	IGameState* m_gameState;	
	CGame(void);
	~CGame(void) = default;
	CButton* daButt;
	CGame(const CGame&) = delete;
	CGame& operator= (const CGame&) = delete;
	AudioSystemWwise * m_pAudioSystem;
	CClient* m_pClient;
	CRenderer* m_pRenderer;
	CurrentStateID stateID;
	CLocalUpdateManager* m_pLocalUpdateManager = nullptr;
	CGameObject* dummyListener;
	void LoadConfig();
	void SaveConfig();

	int		loadout; 
};

#endif
