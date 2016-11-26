#ifndef BUTTON_H
#define BUTTON_H

#include <string>

class CRenderGUI;
class CGUIElement;
class CUIFrame;

enum class FrameType; 
class CButton
{
public:
	CButton(CRenderGUI* RenderGui, std::wstring TextureName,float Layer,int Width, int Height, bool Frame = false, FrameType = (FrameType)0);
	~CButton();
	void Update(float Delta);
	void SetPosition(float X, float Y);
	bool GetSelected();

	void Fading(bool Fading);
	void SetScale(float Scale);

	void Clicked();
	void SetVisibility(bool Visibility);
private:
	CGUIElement*		m_Button;
	CRenderGUI*			m_RenderGUI;

	bool				m_bClicked;
	float				m_ClickedTimer;
	bool				m_bFading;
	bool				m_bActive;

	CUIFrame*			m_Frame;
	int					m_FrameType;
	int					m_Width;
	int					m_Height;
	float				m_Layer;
};

#endif;