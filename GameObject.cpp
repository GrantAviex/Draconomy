
//GameObject.cpp
//Created May 2015

#include "GameObject.h"
#include "Component.h"
#include "ObjectManager.h"
#ifndef IARESERVER
#include"Wwise_IDs.h"
#include "AudioSystemWwise.h"
#endif
#ifdef IARESERVER
#include "Manager.h"
#include "Server.h"
#include "Events.h"
#endif
CGameObject::CGameObject(unsigned int id, CGameObject* cloneFrom, CObjectManager* _objMan, CEventSystem* eventSys, bool server)
{
	m_objMan = _objMan;
	m_uID = id;
	m_Parent = nullptr;
	m_Following = nullptr;
	m_bActive = false;
	m_pEventSystem = eventSys;

	if (cloneFrom != nullptr)
	{
		name = cloneFrom->GetName();
		m_TransformMatrix = cloneFrom->m_TransformMatrix;
		XMFLOAT3 scl = cloneFrom->GetLocalScale();
		SetLocalScale(scl.x, scl.y,scl.z);
#ifndef IARESERVER
		if (strcmp(name.c_str(), "Camera") == 0)
		{
			CGameObject* root = this;
			while (root->GetParent())
			{
				root = root->GetParent();
			}
			
		//	AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_MX_MUSICLOOP_05,XMFLOAT3{0,0,0});
		}
	//	AudioSystemWwise::Get()->RegisterEntity(this,nullptr);
#endif
		
		
		list<CComponent*>& cList = cloneFrom->GetAllComponents();
		for (auto i = cList.begin(); i != cList.end(); i++)
		{
			CComponent* nCom = (*i)->clone();
			nCom->SetOwner(this);
			
			AddComponent(nCom);
		}
		list<CGameObject*>& chList = cloneFrom->GetChildren();
		if (server)
		{
			for (auto i = chList.begin(); i != chList.end(); i++)
			{
				unsigned int _id = m_objMan->GetNewUniqueID();
				CGameObject* nChild = new CGameObject(_id, (*i), m_objMan, eventSys,server);
				AddChild(nChild);
				nChild->SetParent(this);
				_objMan->AddObject(_id, nChild);
			}
		}
		else
		{
			int chCount = 1;
			for (auto i = chList.begin(); i != chList.end(); i++)
			{
				unsigned int _id = id + chCount;
				CGameObject* nChild = new CGameObject(_id, (*i), m_objMan, eventSys, server);
				AddChild(nChild);
				nChild->SetParent(this);
				_objMan->AddObject(_id, nChild);
				chCount++;
			}

		}
	}
	iChanged = true;
}
void CGameObject::ReceiveDirectEvent(EventID _eventID, TEventData* _tEventData)
{
	auto begin = m_Components.begin();
	auto end = m_Components.end();

	for (auto i = begin; i != end; ++i)
	{
		CComponent* current = *i;
		current->ReceiveEvent(_eventID, _tEventData);
	}

}
CGameObject::~CGameObject()
{
	//To do: Destroy components and children
	while (m_Components.size() > 0)
	{
		CComponent* comp = (*m_Components.begin());
		m_Components.pop_front();
		comp->Unsubscribe();
		delete comp;
		comp = nullptr;
	}
}

void CGameObject::CopyTransform(CGameObject* prefab)
{
	m_TransformMatrix = prefab->GetMatrix();
	list<CGameObject*>  list = GetChildren();
	auto myIter = list.begin();
	for (auto& i : prefab->GetChildren())
	{
		(*myIter)->CopyTransform(i);
	}
}

void CGameObject::AddChild(CGameObject* _child)
{
	m_Children.push_back(_child);
}

void CGameObject::AddComponent(CComponent* _component)
{
	m_Components.push_back(_component);
}

CComponent* CGameObject::GetComponent(string componentName)
{
	auto begin = m_Components.begin();
	auto end = m_Components.end();

	for (auto i = begin; i != end; ++i)
	{
		CComponent* current = *i;
		if (current->GetName() == componentName)
		{
			return current;
		}
	}

	return nullptr;
}

CComponent* CGameObject::GetComponentInChildren(string componentName)
{
	auto begin = m_Children.begin();
	auto end = m_Children.end();

	for (auto i = begin; i != end; ++i)
	{
		CGameObject* child = *i;
		CComponent* component = child->GetComponent(componentName);
		if (component != nullptr)
		{
			return component;
		}

		component = child->GetComponentInChildren(componentName);
		if (component != nullptr)
		{
			return component;
		}
	}

	return nullptr;
}

XMFLOAT3 CGameObject::GetAxis(int i)
{
	XMFLOAT3 v;
	v.x = m_TransformMatrix.m[i][0];
	v.y = m_TransformMatrix.m[i][1];
	v.z = m_TransformMatrix.m[i][2];
	return v;
}

XMFLOAT3 CGameObject::GetPosition()
{
	XMFLOAT3 v;
	v.x = m_TransformMatrix._41;
	v.y = m_TransformMatrix._42;
	v.z = m_TransformMatrix._43;
	return v;
}
void CGameObject::SetMatrix(XMMATRIX* mat)
{
	XMStoreFloat4x4(&m_TransformMatrix, *mat);
	iChanged = true;
}
void CGameObject::SetPosition(float x, float y, float z)
{
	m_TransformMatrix._41 = x;
	m_TransformMatrix._42 = y;
	m_TransformMatrix._43 = z;
	iChanged = true;
}

void CGameObject::SetPosition(XMFLOAT3 v)
{
	m_TransformMatrix._41 = v.x;
	m_TransformMatrix._42 = v.y;
	m_TransformMatrix._43 = v.z;
	iChanged = true;
}

void CGameObject::Translate(XMFLOAT3 v)
{
	auto translatedMatrix = XMLoadFloat4x4(&m_TransformMatrix) * XMMatrixTranslation(v.x, v.y, v.z);
	XMStoreFloat4x4(&m_TransformMatrix, translatedMatrix);
	iChanged = true;
}

void CGameObject::Rotate(float x, float y, float z)
{
	//m_Rotation.x = x;
	//m_Rotation.y = y;
	//m_Rotation.z = z;

	XMVECTOR rotation = XMLoadFloat3(&XMFLOAT3(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));


	auto rotatedMatrix = XMMatrixRotationRollPitchYawFromVector(rotation)  *XMLoadFloat4x4(&m_TransformMatrix);
	XMStoreFloat4x4(&m_TransformMatrix, rotatedMatrix);
	iChanged = true;
}

void CGameObject::GlobalRotate(float x, float y, float z)
{

	XMVECTOR rotation = XMLoadFloat3(&XMFLOAT3(XMConvertToRadians(x), XMConvertToRadians(y), XMConvertToRadians(z)));
	auto rotatedMatrix = XMLoadFloat4x4(&m_TransformMatrix) * XMMatrixRotationRollPitchYawFromVector(rotation);
	XMStoreFloat4x4(&m_TransformMatrix, rotatedMatrix);
	iChanged = true;
}

XMMATRIX CGameObject::GetWorldMatrix()
{
	auto worldMatrix = XMLoadFloat4x4(&m_TransformMatrix);

	if (m_Parent)
	{
		worldMatrix *= m_Parent->GetWorldMatrix();
	}
	
	return worldMatrix;
}
void CGameObject::AddToChangeList()
{
	m_objMan->ObjectChanged(this, m_uID);
	for (auto& child : m_Children)
	{
		if (child->GetActive())
		{
			child->AddToChangeList();
		}
	}

}
void CGameObject::Update(float _dt)
{
	for (auto i = m_Components.begin(); i != m_Components.end(); i++)
	{
		(*i)->Update(_dt);
	}
	if (m_Following)
	{
		SetMatrix(&m_Following->GetWorldMatrix());
	}
	if (iChanged)
	{
		AddToChangeList();
		iChanged = false;
	}
}
bool CGameObject::GetRootActive()
{
	if (m_Parent)
	{
		return m_Parent->GetRootActive();
	}
	else
	{
		return m_bActive;
	}
}
void CGameObject::SetActive(bool _active)
{
	if (m_bActive != _active)
	{
		m_objMan->ObjectChanged(this, m_uID);
		m_bActive = _active;
		for (auto& child : m_Children)
		{
			child->SetActive(_active);
		}
		//if (m_Following)
		//{
		//	//m_Following->SetActive(false);
		//}
		if (m_bActive)
		{
			subscribeCount++;
			for (auto& component : m_Components)
			{
				component->OnEnable();
				component->Subscribe();
			}
		}
		else
		{
			subscribeCount--;
			for (auto& component : m_Components)
			{
				component->OnDisable();
				component->Unsubscribe();
			}
		}
	}

}

void CGameObject::LookAt(XMVECTOR target, XMVECTOR worldUp)
{
	XMVECTOR position = XMLoadFloat3(&GetPosition());
	XMVECTOR forward = target - position;
	XMVECTOR right = XMVector3Cross(worldUp, forward);
	XMVECTOR up = XMVector3Cross(forward, right);

	XMMATRIX matrix = XMLoadFloat4x4(&GetMatrix());
	matrix.r[0] = XMVector3Normalize(right);
	matrix.r[1] = XMVector3Normalize(up);
	matrix.r[2] = XMVector3Normalize(forward);
	SetMatrix(&matrix);
	iChanged = true;
}

void CGameObject::SetLocalScale(float X, float Y, float Z, bool UpdateChildren)
{
	// sets local scale
	m_LocalScale.x = X;
	m_LocalScale.y = Y;
	m_LocalScale.z = Z;


	// calc new world scale
	m_WorldScale = { 1, 1, 1 };
	//if (m_Parent)
	//{
	//	m_WorldScale.x *= m_Parent->GetLocalScale().x;
	//	m_WorldScale.y *= m_Parent->GetLocalScale().y;
	//	m_WorldScale.z *= m_Parent->GetLocalScale().z;
	//}
	//
	m_WorldScale.x *= m_LocalScale.x;
	m_WorldScale.y *= m_LocalScale.y;
	m_WorldScale.z *= m_LocalScale.z;

	if (UpdateChildren)
	{
		ChildrenList::iterator itter = m_Children.begin();
		for (; itter != m_Children.end(); itter++)
		{
			(*itter)->UpdateWorldScale();
		}
	}
#ifndef IARESERVER
	if (m_Parent)
	{
		if (m_Parent->name == "EnemyGoblin")
		{
			int i = 9;
			i++;
		}
	}
#endif;

#ifdef IARESERVER
	TEventData d;
	d.i = m_uID;
	d.m[0] = X;
	d.m[1] = Y;
	d.m[2] = Z;
	CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetLocalScale, d);
#endif;
}

void CGameObject::UpdateWorldScale()
{
	// calc new world scale
	m_WorldScale = { 1, 1, 1 };
	if (m_Parent)
	{
		m_WorldScale.x *= m_Parent->GetLocalScale().x;
		m_WorldScale.y *= m_Parent->GetLocalScale().y;
		m_WorldScale.z *= m_Parent->GetLocalScale().z;
	}

	m_WorldScale.x *= m_LocalScale.x;
	m_WorldScale.y *= m_LocalScale.y;
	m_WorldScale.z *= m_LocalScale.z;

	ChildrenList::iterator itter = m_Children.begin();
	for (; itter != m_Children.end(); itter++)
	{
		(*itter)->UpdateWorldScale();
	}
}

void CGameObject::SetWorldScale(float X, float Y, float Z)
{
	m_WorldScale.x = X;
	m_WorldScale.y = Y;
	m_WorldScale.z = Z;

#ifdef IARESERVER
	TEventData d;
	d.i = m_uID;
	d.m[0] = X;
	d.m[1] = Y;
	d.m[2] = Z;
	CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetWorldScale, d);
#endif;
}

XMMATRIX CGameObject::GetWorldScaleMatrix()
{
	//XMFLOAT3 m_WorldScale = { 1, 1, 1 };
	//if (m_Parent)
	//{
	//	m_WorldScale.x *= m_Parent->GetLocalScale().x;
	//	m_WorldScale.y *= m_Parent->GetLocalScale().y;
	//	m_WorldScale.z *= m_Parent->GetLocalScale().z;
	//}
	//
	//m_WorldScale.x *= m_LocalScale.x;
	//m_WorldScale.y *= m_LocalScale.y;
	//m_WorldScale.z *= m_LocalScale.z;
	XMMATRIX scaleMatrix = XMMatrixScaling(m_WorldScale.x, m_WorldScale.y, m_WorldScale.z);
	return scaleMatrix;
}
void CGameObject::SetFollower(CGameObject*_follower)
{
	m_Following = _follower;
	m_Following->SetMatrix(&XMLoadFloat4x4(&m_TransformMatrix));
}