#include "AnimationSystem.h"
#include "Animation.h"

CAnimationSystem::CAnimationSystem()
{
}


CAnimationSystem::~CAnimationSystem()
{
}

CAnimationSystem* CAnimationSystem::GetInstance()
{
	static CAnimationSystem instance;
	return &instance;
}

void CAnimationSystem::Add(CAnimation* animation)
{
	if (animation != 0)
	{
		m_vAnimations.push_back(animation);
	}
}

void CAnimationSystem::Remove(CAnimation* animation)
{
	size_t i, size;

	size = m_vAnimations.size();
	for (i = 0; i < size; ++i)
	{
		if (animation == m_vAnimations[i])
		{
			m_vAnimations.erase(m_vAnimations.begin() + i);
			break;
		}
	}
}

void CAnimationSystem::Remove(std::string name)
{
	size_t i, size;

	size = m_vAnimations.size();
	for (i = 0; i < size; ++i)
	{
		if (name == m_vAnimations[i]->GetName())
		{
			m_vAnimations.erase(m_vAnimations.begin() + i);
			break;
		}
	}
}


void CAnimationSystem::RemoveAll()
{
	m_vAnimations.clear();
}

CAnimation* CAnimationSystem::GetAnimation(std::string name)
{
	size_t i, size;

	size = m_vAnimations.size();
	for (i = 0; i < size; ++i)
	{
		if (name == m_vAnimations[i]->GetName())
		{
			return m_vAnimations[i];
		}
	}

	return 0;
}

CAnimation* CAnimationSystem::GetAnimation(size_t index)
{
	if (index >= m_vAnimations.size())
	{
		return 0;
	}
	else
	{
		return m_vAnimations[index];
	}
}