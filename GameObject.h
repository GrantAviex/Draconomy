
//GameObject.h
//Created May 2015

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <list>
#include <DirectXMath.h>
#include "EventSystem.h"
using namespace DirectX;
using namespace std;

class CComponent;
class CObjectManager;


class CGameObject
{
public:
	CGameObject(unsigned int id, CGameObject* cloneFrom, CObjectManager* _objMan, CEventSystem* eventSys, bool server);
	~CGameObject();

	//Hierarchy
	CGameObject* GetParent() { return m_Parent; }
	void SetParent(CGameObject* _parent) { m_Parent = _parent; }
	list<CGameObject*> GetChildren() { return m_Children; }
	void AddChild(CGameObject* _child);
	void AddComponent(CComponent* _component);
	CComponent* GetComponent(string componentName);
	CComponent* GetComponentInChildren(string componentName);
	list<CComponent*> GetAllComponents() { return m_Components; }
	CObjectManager* GetObjMan() { return m_objMan; }
	void SetActive(bool _active);
	bool GetActive() { return m_bActive; }
	bool GetRootActive();
	void AddToChangeList();
	CGameObject* GetFollower() { return m_Following; }
	void SetFollower(CGameObject* _follower);
	//Transform
	void CopyTransform(CGameObject* prefab);
	XMFLOAT4X4 GetMatrix() { return m_TransformMatrix; }
	XMMATRIX GetWorldMatrix();
	XMFLOAT3 GetAxis(int i);
	XMFLOAT3 GetPosition();
	void SetPosition(XMFLOAT3 v);
	void SetPosition(float x, float y, float z);
	void SetMatrix(XMMATRIX* mat);
	void Translate(XMFLOAT3 v);
	void Rotate(float x, float y, float z);
	void GlobalRotate(float x, float y, float z);
	void LookAt(XMVECTOR target, XMVECTOR worldUp);

	//======
	// Scale
	//=======
	void SetLocalScale(float X, float Y, float Z, bool UpdateChildren = true);
	void SetWorldScale(float X, float Y, float Z);

	XMMATRIX GetWorldScaleMatrix();
	XMFLOAT3 GetLocalScale() { return m_LocalScale; }
	XMFLOAT3 GetWorldScale() { return m_WorldScale; }

	void UpdateWorldScale();

	//Utility
	void ReceiveDirectEvent(EventID _eventID, TEventData* _tEventData);
	void Update(float _dt);
	void SetName(string _name) { name = _name; }
	int GetID() { return m_uID; }
	string GetName() { return name; }
	bool GetGrounded(){ return m_bGrounded; }
	void SetGrounded(bool _grounded){ m_bGrounded = _grounded; }
	CEventSystem* GetEventSystem() { return m_pEventSystem; }
	void SetEventSystem(CEventSystem* _pEventSystem) { m_pEventSystem = _pEventSystem; }
	void IsAnimatedObject(bool animated){ m_bAnimated = animated; }
	bool GetAnimated() { return m_bAnimated; }
private:
	//Hierarchy
	CGameObject* m_Parent;
	CGameObject* m_Following;

	typedef  list< CGameObject* > ChildrenList;
	ChildrenList m_Children;
	list<CComponent*> m_Components;
	CObjectManager* m_objMan;
	bool m_bActive;
	int subscribeCount = 0;
	//Transform
	XMFLOAT4X4 m_TransformMatrix;
	bool	   iChanged;
	//Utility
	unsigned int m_uID;
	bool m_bGrounded;
	bool m_bAnimated;
	string name;
	CEventSystem* m_pEventSystem;

	DirectX::XMFLOAT3	m_LocalScale = { 1, 1, 1 };
	DirectX::XMFLOAT3	m_WorldScale = { 1, 1, 1 };
	DirectX::XMFLOAT3	m_Position = { 0, 0, 0 };
	DirectX::XMFLOAT3	m_Rotation = { 0, 0, 0 };
};

struct TChangedObject
{
	CGameObject* m_pGameObject;
	int m_iListIndex;
};

struct TNewObject
{
	string m_prefabName;
	XMFLOAT4X4 m_transform;
	int m_iListIndex;
};

#endif
