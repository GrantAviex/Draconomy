
//GameplayState.h
//Created May 2015

#ifndef GAMEPLAYSTATE_H
#define GAMEPLAYSTATE_H

#include "IGameState.h"
//#include "GUI.h"
#include "ObjectManager.h"
#include "EventSystem.h"

class CClient;
class CGamePlayHUD;
class CGUIElement;
class CRenderBillboard;
class ILight;

class CGameplayState : public IGameState, public IEventSubscriber
{
public:
	/**********************************************************/
	// Singleton Accessor
	static CGameplayState* GetInstance(void);
	CObjectManager* GetObjectManager() { return m_ObjMan; }
	void SetObjectManager(CObjectManager* _ObhMan){ m_ObjMan = _ObhMan; }

	/**********************************************************/
	// IGameState Interface:
	virtual void	Enter(void)				override;	// load resources
	virtual void	Exit(void)				override;	// unload resources 

	virtual bool	Input(void)				override;	// handle user input
	virtual void	Update()	override;	// update entites
	virtual void	Render(void)				override;	// render entities / menu
	virtual void HandleKey(char _key) override;
	virtual void HandleMouseClick(int _mouseClick) override;
	CClient* GetClient() { return m_pClient; }
	void YouWin();
	void YouLose();
	void ReceiveEvent(EventID id, TEventData const* data) override;
	void ToggleHUD() { bShowHUD = !bShowHUD; }
	bool GetGameOver(){ return m_bGameOver; }
	void SetCursorVisible(bool _visible){ m_bShowCursor = _visible; }

	void SpawnExplosion(float X, float Y, float Z);
private:
	bool closing;
	int switchOption;
	bool clientStarted = false;
	bool m_bGameOver;
	bool m_bShowCursor = false;
	//HWND hwndButton; 
	CClient* m_pClient;
	char prevKey;
	int prevMouseClick;
	int nState;
	CObjectManager* m_ObjMan;
	POINT prevMouse;
	CGameplayState();
	virtual ~CGameplayState();
	CGameplayState(const CGameplayState&) = delete;
	CGameplayState& operator=(const CGameplayState&) = delete;

	CGamePlayHUD*		HUD;
	bool				bShowHUD;

	CGUIElement*	Cursor;

	CGUIElement*	TotalLights;
	std::vector<ILight*>			m_Lights;
};

#endif
