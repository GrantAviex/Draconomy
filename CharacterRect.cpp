#include "CharacterRect.h"

CCharacterRect::CCharacterRect(int ImageWidth, int ImageHeight, int CellWidth, int CellHeight, int Offset)
{
	m_ImageSize.x = ImageWidth;
	m_ImageSize.y = ImageHeight;
	m_CellSize.x = CellWidth;
	m_CellSize.y = CellHeight;
	m_Offset = Offset;

	m_RowWidth = ImageWidth / CellWidth;
}

CCharacterRect::~CCharacterRect()
{

}

RECT CCharacterRect::GetRect(char character)
{
	RECT rect;

	int cell = (character - m_Offset);
	int currentRow = (cell / (m_RowWidth));
	int currentColum = (cell % m_RowWidth);

	rect.left = currentColum * m_CellSize.x;
	rect.right = rect.left + m_CellSize.x;

	rect.top = currentRow * m_CellSize.y;
	rect.bottom = rect.top + m_CellSize.y;

	return rect;
}