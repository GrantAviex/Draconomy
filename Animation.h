#pragma once
#include <vector>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;
class CAnimation
{
public:

	struct TKey
	{
		float m_fTime;
		XMFLOAT4X4 m_mNode;
	};
	struct TChannel
	{
		std::vector<TKey> m_vKeys;
	};

	CAnimation();
	~CAnimation();

	const std::string& GetName() const;
	float GetDuration() const;
	const std::vector< TChannel >& GetChannels() const;


	std::string m_sName;
	std::vector< TChannel > m_vChannels;

};

