
#ifndef CREDITSSTATE_H
#define CREDITSSTATE_H

#include <Windows.h>
#include "IGameState.h"
#include "EventSystem.h"
#include <vector>
class CRenderGUI;
class CGUIElement;
class CButton;

class CCreditsState : public IGameState, public IEventSubscriber
{
public:
	/**********************************************************/
	// Singleton Accessor
	static CCreditsState* GetInstance(void);

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
	void HostDisconnected();
	/**********************************************************/
	// IEventSubscriber Interface:
private:
	//HWND hwndButton; 
	bool closing;
	bool hostDisconnected;
	char prevKey;
	int prevMouseClick;
	POINT prevMouse;
	//CGUI m_GUI;
	CCreditsState();
	virtual ~CCreditsState();
	CCreditsState(const CCreditsState&) = delete;
	CCreditsState& operator= (const CCreditsState&) = delete;
	CRenderGUI* guiInterface = nullptr;
	CGUIElement* mmBackGroundImg = nullptr;
	CGUIElement* mmDisconnectedImg = nullptr;

	CButton*	QuitButton;

	CGUIElement*	Cursor;
};

#endif