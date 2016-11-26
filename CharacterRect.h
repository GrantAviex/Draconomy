#ifndef CHARACTERRECT_H
#define CHARACTERRECT_H

#include "RenderGUI.h"

class CCharacterRect
{
public:
	CCharacterRect(int ImageWidth, int ImageHeight, int CellWidth, int CellHeight, int Offset);
	~CCharacterRect();

	RECT GetRect(char character);
private:
	DirectX::XMINT2		m_ImageSize;
	DirectX::XMINT2		m_CellSize;

	int					m_RowWidth;
	int					m_Offset;
};

#endif;