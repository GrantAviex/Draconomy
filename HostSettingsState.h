
//HostSettingsState.h
//Created May 2015

#ifndef HOSTSETTINGSSTATE_H
#define HOSTSETTINGSSTATE_H

#include "IGameState.h"
#include "GUI.h"
#include "EventSystem.h"
class CHostSettingsState : public IGameState, public IEventSubscriber
{
public:
	/**********************************************************/
	// Singleton Accessor
	static CHostSettingsState* GetInstance(void);

	/**********************************************************/
	// IGameState Interface:
	virtual void	Enter(void)				override;	// load resources
	virtual void	Exit(void)				override;	// unload resources 

	virtual bool	Input(void)				override;	// handle user input
	virtual void	Update(float elapsedTime)	override;	// update entites
	virtual void	Render(void)				override;	// render entities / menu
	virtual void HandleKey(char _key) override;
	virtual void HandleMouseClick(int _mousekey) override;
	void ReceiveEvent(EventID id, TEventData const* data);
	/**********************************************************/
	// IEventSubscriber Interface:
private:
	//HWND hwndButton; 
	char prevKey;
	int prevMouseClick;
	POINT prevMouse;
	CGUI m_GUI;
	CHostSettingsState();
	virtual ~CHostSettingsState();
	CHostSettingsState(const CHostSettingsState&) = delete;
	CHostSettingsState& operator= (const CHostSettingsState&) = delete;
};

#endif
