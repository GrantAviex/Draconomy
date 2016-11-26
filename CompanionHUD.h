#ifndef COMPANIONHUD_H
#define COMPANIONHUD_H

#include "EventSystem.h"
#include <vector>
#include <map>

class CRenderGUI;
class CGUIElement;

class CCompanionHUD : public IEventSubscriber
{
public:
	CCompanionHUD(CRenderGUI* RenderGUI, int Width, int Heigh, int ID);
	~CCompanionHUD();

	void CreateCompanionStats(int ID,std::string Name, int MaxHealth, float color);
	void UpdateCompanionHealth(int ID, float Health, float MaxHealth);

	virtual void ReceiveEvent(EventID id, TEventData const* data) override;

	void SetVisibility(bool Visivility);
private:

	std::map<int, CGUIElement*>		m_Players;
	std::map<int, CGUIElement*>		m_PlayerReady;

	CRenderGUI*						m_RenderGUI;

	std::vector<CGUIElement*>		m_ElementContainer;

	int								m_Width;
	int								m_Height;
	int								m_MyID;

	float							m_YOffset;
};

#endif;