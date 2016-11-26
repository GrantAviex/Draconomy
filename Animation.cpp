#include "Animation.h"


CAnimation::CAnimation()
{
}

CAnimation::~CAnimation()
{
}

const std::string& CAnimation::GetName() const
{
	return m_sName;
}

float CAnimation::GetDuration() const
{
	int i = m_vChannels.size() - 1;
	for (; i >= 0; i--)
	{
		if (m_vChannels[i].m_vKeys.size() > 0)
		{
			return m_vChannels[i].m_vKeys.back().m_fTime;
		}
	}
	return 0;
}

const std::vector< CAnimation::TChannel >& CAnimation::GetChannels() const
{
	return m_vChannels;
}

