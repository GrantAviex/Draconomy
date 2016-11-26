
//Collider.h
//Created May 2015

#ifndef COLLIDER_H
#define COLLIDER_H
enum ColliderType
{
	eMesh = 0,
	eSphere,
	eCapsule,
	eAABB
};
class ICollider
{
public:
	virtual bool CheckCollision(ICollider* other) = 0;
	virtual void HandleCollision(ICollider* other) = 0;
	virtual ColliderType GetType() = 0;
protected:
	ICollider() = default;
	~ICollider() = default;
};

#endif