#ifndef ANIMATEDBILLBOARD_H
#define ANIMATEDBILLBOARD_H

#include "Component.h"
#include <string>
#include <DirectXMath.h>

class CRenderBillboard;
class CAnimatedBillboard : public CComponent
{
public:
	CAnimatedBillboard(CGameObject* Owner);
	~CAnimatedBillboard();

	virtual CAnimatedBillboard* clone() const;
	void OnEnable() override;
	void OnDisable() override;
	void Update(float Delta) override;
	void ReceiveEvent(EventID id, TEventData const* data) override;
	void SetOffset(float Offset) { m_Offset = Offset; }

	void SetFileName(std::wstring FileName)		{ m_FileName = FileName;}
	void SetWidth(int Width)					{ m_Width = Width; }
	void SetHeight(int Height)					{ m_Height = Height; }
	void SetCellWidth(int Width)				{ m_CellWidth = Width; }
	void SetCellHeight(int Height)				{ m_CellHeight = Height; }
	void SetCellCount(int Count)				{ m_CellCount = Count; }
	void SetScale(float X, float Y)				{ m_ScaleX = X; m_ScaleY = Y; }
	void SetDuration(float Duration)			{ m_Duration = Duration; }
	void SetRelativeLocation(float X, float Y, float Z);
	void SetUseWorld(float World);
	void SetRotation(float Rotation);

private:

	std::wstring						m_FileName;
	float								m_Offset;
	bool								m_bInit;

	int									m_Width;
	int									m_Height;

	float								m_Duration;
	int									m_CellCount;
	int									m_CellWidth;
	int									m_CellHeight;

	float								m_ScaleX;
	float								m_ScaleY;

	CRenderBillboard*					m_Billboard;

	DirectX::XMFLOAT3					m_RelativePosition;
	float								m_UseWorld;
	float								m_Rotation;
	
};


#endif