
//HostGameSettingsState.h
//Created May 2015

#ifndef HOSTGAMESETTINGSSTATE_H
#define HOSTGAMESETTINGSSTATE_H

#include "IGameState.h"
#include "EventSystem.h"
#include <Windows.h>

class CRenderGUI;
class CGUIElement;
class CButton;

class CHostGameSettingsState : public IGameState, public IEventSubscriber
{
public:
	/**********************************************************/
	// Singleton Accessor
	static CHostGameSettingsState* GetInstance(void);

	/**********************************************************/
	// IGameState Interface:
	virtual void	Enter(void)				override;	// load resources
	virtual void	Exit(void)				override;	// unload resources 

	virtual bool	Input(void)				override;	// handle user input
	virtual void	Update()	override;	// update entites
	virtual void	Render(void)				override;	// render entities / menu
	virtual void HandleKey(char _key) override;
	virtual void HandleMouseClick(int _mousekey) override;
	void ReceiveEvent(EventID id, TEventData const* data);
	void SetHostName(wstring _name) { name = _name; }
	/**********************************************************/
	// IEventSubscriber Interface:
private:
	//HWND hwndButton; 
	bool closing;
	int switchOption;
	char prevKey;
	int prevMouseClick;
	int serversAvailable = 0;
	unsigned int MapIndex = 0;
	unsigned int MaxPlayers = 4;
	unsigned int StartWave = 1;
	unsigned int EndWave = 10;
	CRenderGUI* guiInterface = nullptr;
	CGUIElement* mBackGroundImg = nullptr;
	vector<CGUIElement*> buttonList;
	vector<unsigned char> m_lobbyMaxPlayers;
	CGUIElement* mMapImg = nullptr;

	CButton*		LeftPlayerArrow;
	CButton*		RightPlayerArrow;
	CButton*		LeftMapArrow;
	CButton*		RightMapArrow;
	CButton*		LeftStartArrow;
	CButton*		RightStartArrow;
	CButton*		LeftEndArrow;
	CButton*		RightEndArrow;
	CButton*		ContinueButton;

	CGUIElement* mPlayerText = nullptr;
	CGUIElement* mStartText = nullptr;
	CGUIElement* mEndText = nullptr;

	wstring name;
	POINT prevMouse;
	CHostGameSettingsState();
	virtual ~CHostGameSettingsState();
	CHostGameSettingsState(const CHostGameSettingsState&) = delete;
	CHostGameSettingsState& operator= (const CHostGameSettingsState&) = delete;

	CGUIElement*	Cursor;
};

#endif
