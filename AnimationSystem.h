#pragma once

#include <vector>

class CAnimation;

class CAnimationSystem
{
private:
	CAnimationSystem();
	CAnimationSystem(const CAnimationSystem& a);
	const CAnimationSystem& operator=(const CAnimationSystem& a);
public:
	~CAnimationSystem();
	static CAnimationSystem* GetInstance();

	void Add(CAnimation* animation);
	void Remove(CAnimation* animation);
	void Remove(std::string name);
	void RemoveAll();

	CAnimation* GetAnimation(std::string name);
	CAnimation* GetAnimation(size_t index);

private:

	// Shallow pointers
	std::vector< CAnimation* > m_vAnimations;
};

