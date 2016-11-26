#include "CollisionManager.h"
#include "SphereCollider.h"
#include "AABBCollider.h"
#include "CapsuleCollider.h"
#include "Manager.h"
#include "XTime.h"
#include "Renderer.h"
#include "SphereTrigger.h"
#include "BulletController.h"
#include "VortexController.h"
#include "UnitStats.h"
#include "PlayerMovementController.h"
#include "Server.h"
#include "EventSystem.h"
#include "InputManager.h"
#include "GameObject.h"
#include "AssetManager.h"
#include "PlayerBuildController.h"
#include "ManaPickup.h"
#include "Explosion.h"
#include "EnemyController.h"
#include "Activatable.h"
#include "UpgradePickup.h"

CCollisionManager::CCollisionManager()
{

}
void CCollisionManager::Initialize()
{
	m_GridSystem = new CGridSystem;
	m_GridSystem->LoadGrid("Level2Collision");

	m_PathGridSystem = new CGridSystem;
	m_PathGridSystem->LoadGrid("PathMesh");
}

CCollisionManager::~CCollisionManager()
{
	delete m_GridSystem;
}
vector<TTriangle*> CCollisionManager::GetMeshTri(string _str)
{
	vector<CMesh> meshes = *(CManager::GetInstance()->GetAssestManager()->GetMeshes(_str));
	//return &(*meshes)[0];
	vector<TTriangle*> m_vcTriangles;

	std::vector<XMFLOAT3> vertpositions;
	std::vector<XMFLOAT3> normals;
	std::vector<int> indices;

	size_t size = meshes[0].GetVerts().size();
	vertpositions.resize(size);
	normals.resize(size);
	for (size_t i = 0; i < size; i++)	{
		vertpositions[i] = meshes[0].GetVerts()[i].vertInfo.pos;
		normals[i] = meshes[0].GetVerts()[i].vertInfo.nrm;
	}
	size = meshes[0].GetIndices().size();
	indices.resize(size);
	for (size_t i = 0; i < size; i++)
		indices[i] = meshes[0].GetIndices()[i];


	//Load triangles into grid from asset manager.
	for (size_t i = 0; i < size; i += 3) //if issue with winding, swap 2 and the 0
	{
		TTriangle * temptri = new TTriangle;
		temptri->verts[0] = vertpositions[indices[i]];
		temptri->verts[1] = vertpositions[indices[i + 1]];
		temptri->verts[2] = vertpositions[indices[i + 2]];
		temptri->normal.x = (normals[indices[i]].x + normals[indices[i + 1]].x + normals[indices[i + 2]].x) / 3.0f; //does this work?
		temptri->normal.y = (normals[indices[i]].y + normals[indices[i + 1]].y + normals[indices[i + 2]].y) / 3.0f;
		temptri->normal.z = (normals[indices[i]].z + normals[indices[i + 1]].z + normals[indices[i + 2]].z) / 3.0f;
		m_vcTriangles.push_back(temptri);
	}

	return m_vcTriangles;

}

bool CCollisionManager::CandyCollision(ICollider*_candy, ICollider* _col)
{
	CSphereTrigger* cTrigger = (CSphereTrigger*)(_candy);
	if (_col->GetType() == ColliderType::eSphere)
	{
		CGameObject* tempObj = ((CSphereCollider*)(_col))->GetOwner();
		bool col = cTrigger->CheckCollision(_col);
		if (col)
		{
			TEventData dummyData;
			dummyData.i = cTrigger->GetOwner()->GetID();
			tempObj->ReceiveDirectEvent(EventID::CandyPickUp, &dummyData);
			//cTrigger->GetOwner()->SetActive(false);

		}
		return col;
	}
	else if (_col->GetType() == ColliderType::eCapsule)
	{
		CGameObject* tempObj = ((CCapsuleCollider*)(_col))->GetOwner();
		if (((CCapsuleCollider*)(_col))->eCapsuleType == CapsuleType::eCapActivatable)
		{
			return false;
		}
		bool col = cTrigger->CheckCollision(_col);
		if (col)
		{
			TEventData dummyData;
			dummyData.i = cTrigger->GetOwner()->GetID();
			tempObj->ReceiveDirectEvent(EventID::CandyPickUp, &dummyData);
			//cTrigger->GetOwner()->SetActive(false);

		}
		return col;
	}
	return false;
}
bool CCollisionManager::RayToPath(XMFLOAT3 _position)
{
	XMFLOAT3 tempPos = _position;
	tempPos.y += 500;
	
	XMFLOAT3 tempEnd = tempPos;
	tempEnd.y -= 1000;

	unsigned int triCount;
	int triIndex;
	//GET TRIANGLES FROM BVH
	vector<TTriangle*>* tTri;
	tTri = m_PathGridSystem->GetAllTriangles();
	triCount = tTri->size();
	TRayCastHit tempRayHit;
	return LineSegmentToTriangle(tempRayHit.vHitPt, triIndex, *tTri, triCount, tempPos, tempEnd);
	
}

bool CCollisionManager::UpgradeCollision(ICollider*_mana, ICollider* _col)
{
	CSphereTrigger* cTrigger = (CSphereTrigger*)(_mana);
	bool col = false;
	if (_col->GetType() == ColliderType::eCapsule)
	{
		CCapsuleCollider* capCol = ((CCapsuleCollider*)_col);

		CGameObject* tempObj = capCol->GetOwner();
		if (capCol->eCapsuleType == CapsuleType::eCapPlayer)
		{
			col = cTrigger->CheckCollision(_col);
			if (col)
			{
				//Do Stuff Later
				CUpgradePickup* dragonSoul = (CUpgradePickup*)cTrigger->GetOwner()->GetComponent("UpgradePickup");
				if (dragonSoul->GetValue() == tempObj->GetID())
				{
					CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID::UpdateDragonSouls, TEventData(), tempObj->GetID());
				}
				else
				{
					col = false;
				}

			}
		}

	}
	return col;
}
bool CCollisionManager::ManaCollision(ICollider*_mana, ICollider* _col)
{
	CSphereTrigger* cTrigger = (CSphereTrigger*)(_mana);
	bool col = false;
	if (_col->GetType() == ColliderType::eCapsule)
	{
		CCapsuleCollider* capCol = ((CCapsuleCollider*)_col);

		CGameObject* tempObj = capCol->GetOwner();
		if (capCol->eCapsuleType == CapsuleType::eCapPlayer)
		{
			col = cTrigger->CheckCollision(_col);
			if (col)
			{
				CPlayerBuildController* pBuildControl = ((CPlayerBuildController*)tempObj->GetComponent("PlayerBuildController"));
				CManaPickup* pMana = ((CManaPickup*)cTrigger->GetOwner()->GetComponent("ManaPickup"));
				pBuildControl->AddMana((int)pMana->GetValue());
				if (pMana->GetDropped())
				{
					pBuildControl->AddManaCollected((int)pMana->GetValue());
				}
				pBuildControl->SendUpdateMana();
				TEventData dumbStuff;
				dumbStuff.s = "ManaPickUp";
				//			CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID, data, tempMoveCont->GetOwner()->GetID());


			}
		}

	}
	return col;
}
bool CCollisionManager::BarrelCollision(ICollider* _barrel, ICollider* _col)
{
	CSphereTrigger* cTrigger = (CSphereTrigger*)(_barrel);


	if (_col->GetType() == ColliderType::eSphere)
	{
		CSphereCollider* sphereCol = ((CSphereCollider*)_col);

		CGameObject* tempObj = sphereCol->GetOwner();
		if (sphereCol->eSphereType == SphereType::ePlayer)
		{

			CPlayerMovementController* tempMoveCont = ((CPlayerMovementController*)tempObj->GetComponent("PlayerMovementController"));

			if (tempMoveCont->nCandyCarried > 0)
			{
				bool col = cTrigger->CheckCollision(_col);
				if (col)
				{




					TEventData data;
					data.i = tempMoveCont->nCandyCarried;
					data.s = "CandyReturn";
					CManager::GetInstance()->GetServer()->GetEventSystem()->SendEventImmediate(EventID::CandyReturned, &data);
					tempMoveCont->AddCandyReturned(tempMoveCont->nCandyCarried);
					tempMoveCont->nCandyCarried = 0;

					data.i = cTrigger->GetOwner()->GetID();
					CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetEmitterActive, data);

					data.i = 0;
					XMFLOAT4X4 posMatrix;
					XMStoreFloat4x4(&posMatrix, tempMoveCont->GetOwner()->GetWorldMatrix());
					memcpy(data.m, posMatrix.m, sizeof(float) * 16);
					CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, data);
					CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID::UpdateCandyOnPlayer, data, tempMoveCont->GetOwner()->GetID());
					return true;
				}
				else
				{
					return false;
				}
			}


			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

	}
	else if (_col->GetType() == ColliderType::eCapsule)
	{
		CCapsuleCollider* capCol = ((CCapsuleCollider*)_col);

		CGameObject* tempObj = capCol->GetOwner();
		if (capCol->eCapsuleType == CapsuleType::eCapPlayer)
		{

			CPlayerMovementController* tempMoveCont = ((CPlayerMovementController*)tempObj->GetComponent("PlayerMovementController"));

			if (tempMoveCont->nCandyCarried > 0)
			{
				bool col = cTrigger->CheckCollision(_col);
				if (col)
				{




					TEventData data;
					data.i = tempMoveCont->nCandyCarried;
					data.s = "CandyReturn";
					CManager::GetInstance()->GetServer()->GetEventSystem()->SendEventImmediate(EventID::CandyReturned, &data);
					tempMoveCont->AddCandyReturned(tempMoveCont->nCandyCarried);
					tempMoveCont->nCandyCarried = 0;

					data.i = cTrigger->GetOwner()->GetID();
					CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetEmitterActive, data);

					data.i = 0;
					XMFLOAT4X4 posMatrix;
					XMStoreFloat4x4(&posMatrix, tempMoveCont->GetOwner()->GetWorldMatrix());
					memcpy(data.m, posMatrix.m, sizeof(float) * 16);
					CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, data);
					CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID::UpdateCandyOnPlayer, data, tempMoveCont->GetOwner()->GetID());
					return true;
				}
				else
				{
					return false;
				}
			}


			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

	}
	else
	{
		return false;
	}
}
bool CCollisionManager::WallTriCollision(ICollider* _wall, ICollider* _col, int wallType)
{
	std::vector<TTriangle*> wallTris;
	if (wallType == 0)
	{
		wallTris = GetMeshTri("ColliderCube");
	} 
	else if (wallType == 1)
	{
		wallTris = GetMeshTri("TreeWall");
	}
	XMFLOAT3 disp = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 realDisp = XMFLOAT3{ 0.0f, 0.0f, 0.0f };

	if (_col->GetType() != ColliderType::eCapsule)
	{
		return false;
	}
	CCapsuleCollider* capCol = ((CCapsuleCollider*)_col);
	
	TCapsule tCap;
	capCol->SetUpSpheres();
	tCap.botPos = capCol->m_BotSphere->vCenter;
	tCap.topPos = capCol->m_TopSphere->vCenter;
	tCap.fRadius = capCol->m_fRadius;

	TSphere botSphere;
	botSphere.fRadius = tCap.fRadius;
	botSphere.vCenter = tCap.botPos;
	botSphere.vCenter.y += botSphere.fRadius;
	XMMATRIX rotMatrix = ((CSphereTrigger*)_wall)->GetOwner()->GetWorldMatrix();

	for (size_t i = 0; i < wallTris.size(); i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			XMVECTOR triVec = XMLoadFloat3(&wallTris[i]->verts[j]);
			XMVECTOR rotatedTriVec = XMVector3TransformCoord(triVec, rotMatrix);
			XMStoreFloat3(&wallTris[i]->verts[j], rotatedTriVec);

		}
		XMVECTOR triNorm = XMLoadFloat3(&wallTris[i]->normal);
		XMVECTOR rotatedTriNorm = XMVector3TransformNormal(triNorm, rotMatrix);

		XMStoreFloat3(&wallTris[i]->normal, rotatedTriNorm);

		if (SphereToTriangleRes(botSphere, *wallTris[i], disp))
		{
			if (abs(disp.x) > abs(realDisp.x))
			{
				realDisp.x = disp.x;
			}
			if (abs(disp.y) > abs(realDisp.y))
			{
				realDisp.y = disp.y;
			}
			if (abs(disp.z) > abs(realDisp.z))
			{
				realDisp.z = disp.z;
			}
		}
	}
	if (realDisp.x != 0 || realDisp.y != 0 || realDisp.z != 0)
	{
		botSphere.vCenter.x += realDisp.x;
		botSphere.vCenter.y += realDisp.y;
		botSphere.vCenter.z += realDisp.z;
		//colSphere.vCenter.y -= 80;;
		botSphere.vCenter.y -= (botSphere.fRadius - 1);
		((CCapsuleCollider*)_col)->GetOwner()->SetPosition(botSphere.vCenter);
		return true;
	}
	return false;

}
bool CCollisionManager::WallSphereCollision(ICollider* _wall, ICollider* _col)
{
	CSphereTrigger* cTrigger = (CSphereTrigger*)(_wall);

	if (_col->GetType() == ColliderType::eSphere)
	{
		if (((CSphereCollider*)(_col))->eSphereType == SphereType::eStructure)
			return false;
		CGameObject* tempObj = ((CSphereCollider*)(_col))->GetOwner();
		bool col = cTrigger->CheckCollision(_col);
		if (col)
		{
			
			return true;
		}
	}
	else if (_col->GetType() == ColliderType::eCapsule)
	{
		CGameObject* tempObj = ((CCapsuleCollider*)(_col))->GetOwner();
		bool col = cTrigger->CheckCollision(_col);
		if (col)
		{
		
			return true;
		}
	}


	return false;
}
bool CCollisionManager::BulletToWallCollision(ICollider* _bullet, ICollider* _col)
{
	CSphereTrigger* bullTrigger = (CSphereTrigger*)(_bullet);
	CSphereTrigger* wallTrigger = (CSphereTrigger*)(_col);

	TSphere bulSphere, wallSphere;
	bulSphere.fRadius = bullTrigger->m_Radius;
	bulSphere.vCenter = bullTrigger->GetOwner()->GetPosition();

	wallSphere.fRadius = wallTrigger->m_Radius;
	wallSphere.vCenter = wallTrigger->GetOwner()->GetPosition();

	if (SphereToSphere(bulSphere, wallSphere))
	{
		CBulletController* tempBulCont = ((CBulletController*)bullTrigger->GetOwner()->GetComponent("BulletController"));
		int dmg = tempBulCont->GetDamage();
		CUnitStats* pUniStats = ((CUnitStats*)wallTrigger->GetOwner()->GetComponent("UnitStats"));
		pUniStats->TakeDamage(-(float)dmg);

		return true;

	}
		return false;
}

bool CCollisionManager::BulletCollision(ICollider* _bullet, ICollider* _col)
{


	CSphereTrigger* cTrigger = (CSphereTrigger*)(_bullet);

	if (_col->GetType() == ColliderType::eMesh)
	{
		CGameObject* tempObj = ((CMeshCollider*)(_col))->GetOwner();
		CGameObject* bulObj = cTrigger->GetOwner();
		CBulletController* bulCont = ((CBulletController*)bulObj->GetComponent("BulletController"));
		if (bulCont->GetVortexBool() == FALSE)
			return false;
		bool col = false;
		if (bulCont->GetWhoShotMe()->GetComponent("Structure") != nullptr)
		{
			col = cTrigger->CheckWallCollision(_col);
		}
		else
			col = cTrigger->CheckCollision(_col);
		if (col)
		{
			//	printf("GROUNDEXPLODE!!\n");

			cTrigger->GetOwner()->SetActive(false);

			return true;

		}
		return false;
	}
	else if (_col->GetType() == ColliderType::eCapsule)
	{
		CBulletController* bulletCont = nullptr;
		bulletCont = ((CBulletController*)cTrigger->GetOwner()->GetComponent("BulletController"));
		CGameObject* pShooter = bulletCont->GetWhoShotMe();
		CEnemyController* pEnController = ((CEnemyController*)pShooter->GetComponent("EnemyController"));
		CActivatable* pAct = (CActivatable*)(((CCapsuleCollider*)(_col))->GetOwner()->GetComponent("Activatable"));

		if (pEnController == nullptr)
		{
			if (((CCapsuleCollider*)(_col))->eCapsuleType == CapsuleType::eCapStructure)
			{
				return false;
			}
			CStructure* pStruct = ((CStructure*)pShooter->GetComponent("Structure"));
			if (pStruct != nullptr && pAct != nullptr)
			{
				return false; // Structure has shot an activateable(Do not Activate it and let the bullet pass through)
			}
		}
		else
		{
			if (pAct)
			{
				return false;
			}
			if (((CCapsuleCollider*)(_col))->eCapsuleType == CapsuleType::eCapEnemy)
			{
				return false; //Enemy has shot an enemy (LET IT PASS THROUGH)
			}
		}
		CGameObject* tempObj = ((CCapsuleCollider*)(_col))->GetOwner();
		
		if (tempObj == bulletCont->GetWhoShotMe())
			return false;
		bool col = cTrigger->CheckCollision(_col);

		if (col)
		{
			if (((CCapsuleCollider*)(_col))->eCapsuleType == CapsuleType::eCapActivatable)
			{
				CActivatable* act = (CActivatable*)(((CCapsuleCollider*)(_col))->GetOwner()->GetComponent("Activatable"));
				act->Activate();
				return true;
			}
			//	printf("TEDDYSTRUCTION!!\n");
			float dmg = (float)(bulletCont->GetDamage());
			float damgMult = 1.0f;
			damgMult = bulletCont->GetDamageMult();
			if (damgMult != 1.0f)
			{
				dmg *= bulletCont->GetDamageMult();
			}
			if (bulletCont->GetBouncing() == TRUE)
			{
				dmg *= 0.75f;
			}
		
			CExplosion* pExplode = nullptr;
			pExplode = ((CExplosion*)bulletCont->GetOwner()->GetComponent("Explosion"));
			int explodingLevel = 0;
			explodingLevel = bulletCont->GetExploding();
			if (pExplode != nullptr &&  explodingLevel > 0)
			{
				float xploRadius = pExplode->GetExplosionRadius();
				if (explodingLevel == 2)
				{
					xploRadius *= 2.0f;
					dmg *= 0.9f;

				}
				else
				{
					dmg *= 0.8f;

				}
				vector<CGameObject*> explosionVec = OverlapSphere(tempObj->GetPosition(), xploRadius, CapsuleType::eCapEnemy);
				for (CGameObject* explodeOBj : explosionVec)
				{
					CUnitStats* someStats = ((CUnitStats*)explodeOBj->GetComponent("UnitStats"));
					if (someStats != nullptr)
					{

						someStats->TakeDamage((float)-dmg);
						if (someStats->GetCurrentHealth() <= 0)
							bulletCont->KilledEnemy();
						else
						{
							float dmgMult = bulletCont->GetWknDmg();
							float defMult = bulletCont->GetWknAmr();
							if (dmgMult != 1.0f)
							{
								((CEnemyController*)tempObj->GetComponent("EnemyController"))->m_WknDmgMult = dmgMult;
							}
							if (defMult != 1.0f)
							{
								someStats->SetDefMult(defMult);
							}
							int statusInt = 0;
							if (bulletCont->GetBurning() == TRUE)
							{
								someStats->SetBurning(2.0f);
								statusInt++;
							}
							else if (bulletCont->GetBlazing() == TRUE)
							{
								someStats->SetBurning(2.0f);
								someStats->IncreaseBlaze();
								
								statusInt++;
							}
							float slowAmt = bulletCont->GetSlow();
							if (slowAmt > 0.0f)
							{
						//		statusInt += 2;
								// EnemyController SetSlowFunction(Pass slowAmt)
								((CEnemyController*)tempObj->GetComponent("EnemyController"))->SetSlowedStatus(slowAmt);
							}
							if (statusInt != 0)
							{
								TEventData statData;
								statData.i = statusInt;
								statData.m[0] = (float)tempObj->GetID();
								statData.m[1] = (float)(someStats->GetBlaze());
								CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetStatusEffect, statData);
							}
						}
					}
				}

			}
			else
			{
				CUnitStats* someStats = ((CUnitStats*)tempObj->GetComponent("UnitStats"));
				if (someStats != nullptr)
				{

					someStats->TakeDamage((float)-dmg);
					if (tempObj->GetComponent("PlayerGunController") != nullptr)
					{
						TEventData data;
						data.i = (int)someStats->GetCurrentHealth();
						CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID::UpdateHealth, data, tempObj->GetID());
					}
					if (someStats->GetCurrentHealth() <= 0)
						bulletCont->KilledEnemy();
					else
					{
						float dmgMult = bulletCont->GetWknDmg();
						float defMult = bulletCont->GetWknAmr();
						if (dmgMult != 1.0f)
						{
							((CEnemyController*)tempObj->GetComponent("EnemyController"))->m_WknDmgMult = dmgMult;
						}
						if (defMult != 1.0f)
						{
							someStats->SetDefMult(defMult);
						}
						int statusInt = 0;
						if (bulletCont->GetBurning() == TRUE)
						{
							someStats->SetBurning(2.0f);
							statusInt++;
						}
						else if (bulletCont->GetBlazing() == TRUE)
						{
							someStats->SetBurning(2.0f);
							someStats->IncreaseBlaze();
							statusInt++;
						}
						float slowAmt = bulletCont->GetSlow();
						if (slowAmt > 0.0f)
						{
						//	statusInt += 2;
							// EnemyController SetSlowFunction(Pass slowAmt)
							((CEnemyController*)tempObj->GetComponent("EnemyController"))->SetSlowedStatus(slowAmt);
						}
						if (statusInt != 0)
						{
							TEventData statData;
							statData.i = statusInt;
							statData.m[0] = (float)tempObj->GetID();
							statData.m[1] = (float)(someStats->GetBlaze());

							CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetStatusEffect, statData);
						}
					}
				}
			}
			return true;

		}
	}
	else if (_col->GetType() == ColliderType::eSphere)
	{
		CBulletController* bulletCont = nullptr;
		bulletCont = ((CBulletController*)cTrigger->GetOwner()->GetComponent("BulletController"));
		if (bulletCont->GetWhoShotMe()->GetComponent("EnemyController") == nullptr)
		{
			if (((CSphereCollider*)(_col))->eSphereType == SphereType::eStructure)
				return false;
		}
		CGameObject* tempObj = ((CSphereCollider*)(_col))->GetOwner();
		
		if (tempObj == bulletCont->GetWhoShotMe())
			return false;
		bool col = cTrigger->CheckCollision(_col);
		if (col)
		{
			//	printf("TEDDYSTRUCTION!!\n");
			int dmg = bulletCont->GetDamage();
			float damgMult = 1.0f;
			damgMult = bulletCont->GetDamageMult();
			if (damgMult != 1.0f)
			{
				dmg = (int)(dmg * bulletCont->GetDamageMult());
			}
			if (bulletCont->GetBouncing() == TRUE)
			{
				dmg = (int)(dmg *  0.5f);
			}
			CExplosion* pExplode = nullptr;
			pExplode = ((CExplosion*)bulletCont->GetOwner()->GetComponent("Explosion"));
			int explodingLevel = 0;
			explodingLevel = bulletCont->GetExploding();
			if (pExplode != nullptr &&  explodingLevel > 0)
			{
				float xploRadius = pExplode->GetExplosionRadius();
				if (explodingLevel == 2)
				{
					xploRadius *= 2.5f;
				}
				vector<CGameObject*> explosionVec = OverlapSphere(tempObj->GetPosition(), xploRadius, SphereType::eEnemy);
				for (CGameObject* explodeOBj : explosionVec)
				{
					CUnitStats* someStats = ((CUnitStats*)explodeOBj->GetComponent("UnitStats"));
					if (someStats != nullptr)
					{

						someStats->TakeDamage((float)-dmg);
						if (someStats->GetCurrentHealth() <= 0)
							bulletCont->KilledEnemy();
						else
						{
							float dmgMult = bulletCont->GetWknDmg();
							float defMult = bulletCont->GetWknAmr();
							if (dmgMult != 1.0f)
							{
								((CEnemyController*)tempObj->GetComponent("EnemyController"))->m_WknDmgMult = dmgMult;
							}
							if (defMult != 1.0f)
							{
								someStats->SetDefMult(defMult);
							}
							if (bulletCont->GetBurning() == TRUE)
							{
								someStats->SetBurning(2.0f);
							}
							float slowAmt = bulletCont->GetSlow();
							if (slowAmt > 0.0f)
							{
								// EnemyController SetSlowFunction(Pass slowAmt)
								((CEnemyController*)tempObj->GetComponent("EnemyController"))->SetSlowedStatus(slowAmt);
							}
						}
					}
				}

			}
			else
			{
				CUnitStats* someStats = ((CUnitStats*)tempObj->GetComponent("UnitStats"));
				if (someStats != nullptr)
				{

					someStats->TakeDamage((float)-dmg);
					if (tempObj->GetComponent("PlayerGunController") != nullptr)
					{
						TEventData data;
						data.i = (int)someStats->GetCurrentHealth();
						CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID::UpdateHealth, data, tempObj->GetID());
					}
					if (someStats->GetCurrentHealth() <= 0)
						bulletCont->KilledEnemy();
					else
					{
						float dmgMult = bulletCont->GetWknDmg();
						float defMult = bulletCont->GetWknAmr();
						if (dmgMult != 1.0f)
						{
							((CEnemyController*)tempObj->GetComponent("EnemyController"))->m_WknDmgMult = dmgMult;
						}
						if (defMult != 1.0f)
						{
							someStats->SetDefMult(defMult);
						}
						if (bulletCont->GetBurning() == TRUE)
						{
							someStats->SetBurning(2.0f);
						}
						float slowAmt = bulletCont->GetSlow();
						if (slowAmt > 0.0f)
						{
							// EnemyController SetSlowFunction(Pass slowAmt)
							((CEnemyController*)tempObj->GetComponent("EnemyController"))->SetSlowedStatus(slowAmt);
						}
					}
				}
			}
			/*	TEventData dragonData;
				dragonData.i = 27;
				XMMATRIX dragMat = XMLoadFloat4x4(&cTrigger->GetOwner()->GetMatrix()) * 300;
				XMFLOAT4X4 tempMat;
				XMStoreFloat4x4(&tempMat, dragMat);
				memcpy(dragonData.m, tempMat.m, sizeof(float) * 16);
				CManager::GetInstance()->GetServer()->SendEventMessage(DrawDebugSphere, dragonData);*/

			//cTrigger->GetOwner()->SetActive(false);

			return true;

		}
	}
	return false;
}
void CCollisionManager::HandleCollisions()
{

	//bullet to walltrigger stuff
	for (size_t i = 0; i < triggerObjects.size(); i++)
	{
		CGameObject* triggObj = triggerObjects[i]->GetOwner();
		if (triggObj->GetActive()) //IF TRIGGER IS ACTIVE CHECK IT
		{
			if (triggerObjects[i]->eTriggerType == TriggerType::eBullet)
			{
				CBulletController* tempBulPtr = ((CBulletController*)triggObj->GetComponent("BulletController"));
				CGameObject* pShooter = tempBulPtr->GetWhoShotMe();

				if (pShooter->GetComponent("EnemyController") != nullptr)
				{
					for (size_t j = 0; j < triggerObjects.size(); j++)
					{
						CGameObject* triggObj2 = triggerObjects[j]->GetOwner();
						if (triggObj2->GetActive()) //IF TRIGGER IS ACTIVE CHECK IT
						{
							if (triggerObjects[j]->eTriggerType == TriggerType::eWall || triggerObjects[j]->eTriggerType == TriggerType::eTreeWall)
							{
								if (BulletToWallCollision(triggerObjects[i], triggerObjects[j]))
								{
									if (tempBulPtr->GetPiercing() == false)
									{
										triggObj->SetActive(false);
									}
								}
							}
						}
					}
				}
			}
		}
	}
	//GROUNDED TRIGGER STUFF vvvvvvv
	for (size_t i = 0; i < 1; i++)
	{


		for (size_t i = 0; i < triggerObjects.size(); i++)
		{
			CGameObject* triggObj = triggerObjects[i]->GetOwner();
			if (triggObj->GetActive()) //IF TRIGGER IS ACTIVE CHECK IT
			{
				for (size_t j = 0; j < gameObjects.size(); j++)
				{

					CGameObject* gameObj;
					if (gameObjects[j]->GetType() == ColliderType::eSphere)
					{
						gameObj = ((CSphereCollider*)gameObjects[j])->GetOwner();
					}
					else if (gameObjects[j]->GetType() == ColliderType::eCapsule)
					{
						gameObj = ((CCapsuleCollider*)gameObjects[j])->GetOwner();
					}
					else
					{
						gameObj = ((CMeshCollider*)gameObjects[j])->GetOwner();
					}
					if (gameObj->GetActive()) // IF OBJECT IS ACTIVE CHECK IT
					{
						if (triggerObjects[i]->eTriggerType == TriggerType::eBarrel)
						{
							if (BarrelCollision(triggerObjects[i], gameObjects[j]))
							{
								break;
							}
							else
							{
								continue;
							}
						}
						if (triggerObjects[i]->eTriggerType == TriggerType::eCandy)
						{
							if (CandyCollision(triggerObjects[i], gameObjects[j]))
							{
								break;
							}
							else
							{
								continue;
							}


						}
						if (triggerObjects[i]->eTriggerType == TriggerType::eManaPickup)
						{
							if (ManaCollision(triggerObjects[i], gameObjects[j]))
							{
								triggObj->SetActive(false);
								break;
							}
							else
							{
								continue;
							}

						}
						if (triggerObjects[i]->eTriggerType == TriggerType::eUpgradePickup)
						{
							if (UpgradeCollision(triggerObjects[i], gameObjects[j]))
							{
								triggObj->SetActive(false);
								break;
							}
							else
							{
								continue;
							}

						}
						if (triggerObjects[i]->eTriggerType == TriggerType::eWall)
						{
							if (WallSphereCollision(triggerObjects[i], gameObjects[j]))
							{
								WallTriCollision(triggerObjects[i], gameObjects[j], 0);
								//	triggObj->SetGrounded(false);
							}
						}
						if (triggerObjects[i]->eTriggerType == TriggerType::eTreeWall)
						{
							if (WallSphereCollision(triggerObjects[i], gameObjects[j]))
							{
								WallTriCollision(triggerObjects[i], gameObjects[j], 1);
								//	triggObj->SetGrounded(false);
							}
						}
						if (triggerObjects[i]->eTriggerType == TriggerType::eBullet)
						{
							if (BulletCollision(triggerObjects[i], gameObjects[j]))
							{
								//Bullet collided, KILL IT
								CBulletController* pBulCont = ((CBulletController*)triggerObjects[i]->GetOwner()->GetComponent("BulletController"));

								pBulCont->Bounce();

								//BOUNCE SETS THE BULLET INACTIVE IF IT CAN'T BOUNCE!

								break;
							}
						}
						if (triggerObjects[i]->eTriggerType == TriggerType::eFlame)
						{
							if (BulletCollision(triggerObjects[i], gameObjects[j]))
							{
								//Flames hit
								break;
							}
						}


						if (gameObjects[j]->GetType() == ColliderType::eMesh)
						{
							if (triggerObjects[i]->eTriggerType == TriggerType::eGround)
							{
								if (triggerObjects[i]->CheckCollision(gameObjects[j]))
								{
									triggObj->SetGrounded(true);
									break;
								}
								else
								{
									triggObj->SetGrounded(false);
									break;
								}
							}
						}
					}
				}
			}

		}
	}
	//GROUNDED TRIGGER STUFF ^^^^^^^
	int nColChecks = 1;
	while (nColChecks > 0)
	{
		for (size_t i = 0; i < gameObjects.size(); i++)
		{
			CGameObject* tempObj;
			if (gameObjects[i]->GetType() == ColliderType::eMesh)
			{
				tempObj = ((CMeshCollider*)gameObjects[i])->GetOwner();

			}
			else if (gameObjects[i]->GetType() == ColliderType::eSphere)
			{
				tempObj = ((CSphereCollider*)gameObjects[i])->GetOwner();
			}
			else if (gameObjects[i]->GetType() == ColliderType::eAABB)
			{
				tempObj = ((CAABBCollider*)gameObjects[i])->GetOwner();
			}
			else if (gameObjects[i]->GetType() == ColliderType::eCapsule)
			{
				tempObj = ((CCapsuleCollider*)gameObjects[i])->GetOwner();
			}
			if (tempObj->GetActive()) // IF OBJECT IS ACTIVE CHECK IT
			{
				for (size_t j = i + 1; j < gameObjects.size(); j++)
				{
					CGameObject* tempOtherObj;
					if (gameObjects[j]->GetType() == ColliderType::eMesh)
					{
						tempOtherObj = ((CMeshCollider*)gameObjects[j])->GetOwner();

					}
					else if (gameObjects[j]->GetType() == ColliderType::eSphere)
					{
						tempOtherObj = ((CSphereCollider*)gameObjects[j])->GetOwner();
						if (gameObjects[i]->GetType() == ColliderType::eSphere)
						{
							if (((CSphereCollider*)gameObjects[i])->eSphereType == SphereType::eEnemy && ((CSphereCollider*)gameObjects[j])->eSphereType == SphereType::eEnemy)
								continue;
						}
					}
					else if (gameObjects[j]->GetType() == ColliderType::eCapsule)
					{
						tempOtherObj = ((CCapsuleCollider*)gameObjects[j])->GetOwner();
						((CCapsuleCollider*)gameObjects[j])->SetUpSpheres();
					}
					if (tempOtherObj->GetActive()) // IF OBJECT IS ACTIVE CHECK IT
					{
						if (gameObjects[i]->CheckCollision(gameObjects[j]))
						{
							if (gameObjects[i]->GetType() != ColliderType::eMesh)
							{
								pair<ICollider*, ICollider*> col;
								col = make_pair(gameObjects[i], gameObjects[j]);
								colObjects.push_back(col);
							}

						}
					}
				}
			}
			//Handle Collision objects.
			for (size_t col = 0; col < colObjects.size(); col++)
			{
				colObjects[col].first->HandleCollision(colObjects[col].second);
			}
			colObjects.clear();
		}
		nColChecks--;
	}
}
void CCollisionManager::cross_Product(XMFLOAT3 &_a, XMFLOAT3 &_b, XMFLOAT3 &_c)
{

}
float CCollisionManager::dot_Product(XMFLOAT3 _a, XMFLOAT3 _b)
{
	return _a.x*_b.x + _a.y*_b.y + _a.z*_b.z;
}
bool CCollisionManager::AABBtoAABB(const CAABBCollider& lhs, const CAABBCollider& rhs)
{
	XMFLOAT3 maxInWLHS{ lhs.m_center.x + lhs.m_max.x, lhs.m_center.y + lhs.m_max.y, lhs.m_center.z + lhs.m_max.z };
	XMFLOAT3 minInWLHS{ lhs.m_center.x - lhs.m_min.x, lhs.m_center.y - lhs.m_min.y, lhs.m_center.z - lhs.m_min.z };

	XMFLOAT3 maxInWRHS{ rhs.m_center.x + rhs.m_max.x, rhs.m_center.y + rhs.m_max.y, rhs.m_center.z + rhs.m_max.z };
	XMFLOAT3 minInWRHS{ rhs.m_center.x - rhs.m_min.x, rhs.m_center.y - rhs.m_min.y, rhs.m_center.z - rhs.m_min.z };

	if (maxInWLHS.x < minInWRHS.x || minInWLHS.x > maxInWRHS.x)
		return false;
	if (maxInWLHS.y < minInWRHS.y || minInWLHS.y > maxInWRHS.y)
		return false;
	if (maxInWLHS.z < minInWRHS.z || minInWLHS.z > maxInWRHS.z)
		return false;
	return true;
}

bool CCollisionManager::SphereToAABB(const TSphere& _lhs, const CAABBCollider& rhs, float& _distance, XMVECTOR& _dir)
{
	XMFLOAT3 closestPt;

	XMFLOAT3 maxInWorld{ rhs.m_center.x + rhs.m_max.x, rhs.m_center.y + rhs.m_max.y, rhs.m_center.z + rhs.m_max.z };
	XMFLOAT3 minInWorld{ rhs.m_center.x + rhs.m_min.x, rhs.m_center.y + rhs.m_min.y, rhs.m_center.z + rhs.m_min.z };
	TSphere lhs = _lhs;
	lhs.vCenter.y += 100;
	//X
	if (lhs.vCenter.x > maxInWorld.x)
		closestPt.x = maxInWorld.x;
	else if (lhs.vCenter.x < minInWorld.x)
		closestPt.x = minInWorld.x;
	else
		closestPt.x = lhs.vCenter.x;
	//Y
	if (lhs.vCenter.y > maxInWorld.y)
		closestPt.y = maxInWorld.y;
	else if (lhs.vCenter.y < minInWorld.y)
		closestPt.y = minInWorld.y;
	else
		closestPt.y = lhs.vCenter.y;
	//Z
	if (lhs.vCenter.z > maxInWorld.z)
		closestPt.z = maxInWorld.z;
	else if (lhs.vCenter.z < minInWorld.z)
		closestPt.z = minInWorld.z;
	else
		closestPt.z = lhs.vCenter.z;


	float radSqrd = lhs.fRadius;
	radSqrd *= radSqrd;
	_distance = abs(abs(lhs.vCenter.x - closestPt.x) + abs(lhs.vCenter.y - closestPt.y) + abs(lhs.vCenter.z - closestPt.z));
	float distanceSqrd = _distance * _distance;

	if (distanceSqrd < radSqrd)
	{

		_distance = lhs.fRadius - _distance;
		_dir = ((XMLoadFloat3(&lhs.vCenter)) - (XMLoadFloat3(&closestPt)));
		return true;

	}
	return false;

}
bool CCollisionManager::SphereToAABB(const TSphere& lhs, const CAABBCollider& rhs)
{
	XMFLOAT3 closestPt;
	XMFLOAT3 maxInWorld{ rhs.m_center.x + rhs.m_max.x, rhs.m_center.y + rhs.m_max.y, rhs.m_center.z + rhs.m_max.z };
	XMFLOAT3 minInWorld{ rhs.m_center.x - rhs.m_min.x, rhs.m_center.y - rhs.m_min.y, rhs.m_center.z - rhs.m_min.z };

	//X
	if (lhs.vCenter.x > maxInWorld.x)
		closestPt.x = maxInWorld.x;
	else if (lhs.vCenter.x < minInWorld.x)
		closestPt.x = minInWorld.x;
	else
		closestPt.x = lhs.vCenter.x;
	//Y
	if (lhs.vCenter.y > maxInWorld.y)
		closestPt.y = maxInWorld.y;
	else if (lhs.vCenter.y < minInWorld.y)
		closestPt.y = minInWorld.y;
	else
		closestPt.y = lhs.vCenter.y;
	//Z
	if (lhs.vCenter.z > maxInWorld.z)
		closestPt.z = maxInWorld.z;
	else if (lhs.vCenter.z < minInWorld.z)
		closestPt.z = minInWorld.z;
	else
		closestPt.z = lhs.vCenter.z;


	float radSqrd = lhs.fRadius;
	radSqrd *= radSqrd;
	float distanceSqrd = (closestPt.x - lhs.vCenter.x) * (closestPt.x - lhs.vCenter.x) +
		(closestPt.y - lhs.vCenter.y) * (closestPt.y - lhs.vCenter.y) +
		(closestPt.z - lhs.vCenter.z) * (closestPt.z - lhs.vCenter.z);

	if (distanceSqrd < radSqrd)
		return true;
	return false;

}
// SphereToSphere
//
// Returns true if the Spheres collide. False if not.
bool CCollisionManager::SphereToSphere(const TSphere& lhs, const TSphere& rhs)
{

	float radSqrd = lhs.fRadius + rhs.fRadius;
	radSqrd *= radSqrd;
	float distanceSqrd = (rhs.vCenter.x - lhs.vCenter.x) * (rhs.vCenter.x - lhs.vCenter.x) +
		(rhs.vCenter.y - lhs.vCenter.y) * (rhs.vCenter.y - lhs.vCenter.y) +
		(rhs.vCenter.z - lhs.vCenter.z) * (rhs.vCenter.z - lhs.vCenter.z);

	if (distanceSqrd < radSqrd)
		return true;
	return false;

}
bool CCollisionManager::SphereToSphereSingleRes(TSphere& lhs, TSphere& rhs)
{

	float distance = abs(rhs.vCenter.x - lhs.vCenter.x) +
		abs(rhs.vCenter.y - lhs.vCenter.y) +
		abs(rhs.vCenter.z - lhs.vCenter.z);
	float radSqrd = lhs.fRadius + rhs.fRadius;
	//radSqrd *= radSqrd;
	if (distance > radSqrd)
	{
		return false;
	}

	XMVECTOR lSphereCent = XMLoadFloat3(&lhs.vCenter), rSphereCent = XMLoadFloat3(&rhs.vCenter);
	XMVECTOR toSphere = rSphereCent - lSphereCent;
	toSphere = XMVector3Normalize(toSphere);
	lSphereCent += (-toSphere * (radSqrd - distance));

	XMStoreFloat3(&lhs.vCenter, lSphereCent);
	return true;
}
void CCollisionManager::SphereToSphereRes(TSphere& lhs, TSphere& rhs, CGameObject* lVel, CGameObject* rVel)
{

	float distance = (rhs.vCenter.x - lhs.vCenter.x) +
		(rhs.vCenter.y - lhs.vCenter.y) +
		(rhs.vCenter.z - lhs.vCenter.z);
	float dt = (float)(CManager::GetInstance()->GetDelta());

	XMVECTOR lSphereCent = XMLoadFloat3(&lhs.vCenter), rSphereCent = XMLoadFloat3(&rhs.vCenter);
	XMVECTOR toSphere = rSphereCent - lSphereCent;
	toSphere = XMVector3Normalize(toSphere);
	lSphereCent += (-toSphere * distance  * .3f * dt);
	rSphereCent += (toSphere * distance  * .3f * dt);

	XMStoreFloat3(&lhs.vCenter, lSphereCent);
	XMStoreFloat3(&rhs.vCenter, rSphereCent);

}
bool CCollisionManager::CapsuleToSphere(const TCapsule& capsule,  TSphere& sphere)
{
	XMVECTOR segVec = XMLoadFloat3(&capsule.topPos) - XMLoadFloat3(&capsule.botPos);
	XMVECTOR vec2Circ = XMLoadFloat3(&sphere.vCenter) - XMLoadFloat3(&capsule.botPos);
	float distance = (XMVector3Dot(vec2Circ, segVec).m128_f32[0] / XMVector3Dot(segVec, segVec).m128_f32[0]);

	if (distance < 0)
		distance = 0;
	if (distance > 1)
		distance = 1;
	//distance *= distance;
	XMVECTOR clstPt = XMLoadFloat3(&capsule.botPos) + (segVec * distance);

	TSphere tempSphere;
	tempSphere.fRadius = capsule.fRadius;
	XMStoreFloat3(&tempSphere.vCenter, clstPt);


	return SphereToSphereSingleRes(sphere, tempSphere);

}
bool CCollisionManager::LineSegmentToTriangle(XMFLOAT3 &vOut, int &uiTriIndex, vector<TTriangle *>pTris, unsigned int uiTriCount, const XMFLOAT3 &vStart, const XMFLOAT3 &vEnd)
{

	vOut = vEnd;
	uiTriIndex = -1;

	//Find the NEAREST interesecting triangle
	for (size_t i = 0; i < uiTriCount; i++)
	{

		XMVECTOR fOut = XMLoadFloat3(&vOut), fStart = XMLoadFloat3(&vStart), fEnd = XMLoadFloat3(&vEnd);
		XMVECTOR fVerts0 = XMLoadFloat3(&pTris[i]->verts[0]), fVerts1 = XMLoadFloat3(&pTris[i]->verts[1]), fVerts2 = XMLoadFloat3(&pTris[i]->verts[2]);
		XMVECTOR fNorm = XMLoadFloat3(&pTris[i]->normal);

		float D0 = XMVector3Dot(fStart, fNorm).m128_f32[0];
		float D1 = XMVector3Dot(fVerts1, fNorm).m128_f32[0];
		float D2 = D0 - D1;
		if ((D2) < 0 && (XMVector3Dot(fOut, fNorm).m128_f32[0] - D1) < 0)
			continue;
		if ((D2) > 0 && (XMVector3Dot(fOut, fNorm).m128_f32[0] - D1) > 0)
			continue;
		XMVECTOR L = fOut - fStart;
		float D3 = XMVector3Dot(fNorm, L).m128_f32[0];
		float DF = -(D2 / D3);
		XMVECTOR CP = fStart + L * DF;
		//2 - 0
		XMVECTOR normal0;
		normal0 = XMVector3Cross((fVerts2 - fVerts0), fNorm);
		if (XMVector3Dot(normal0, (CP - fVerts0)).m128_f32[0] > 0)
			continue;
		XMVECTOR normal1;
		//1 - 2
		normal1 = XMVector3Cross((fVerts1 - fVerts2), fNorm);
		if (XMVector3Dot(normal1, (CP - fVerts2)).m128_f32[0] > 0)
			continue;
		XMVECTOR normal2;
		//0 - 1
		normal2 = XMVector3Cross((fVerts0 - fVerts1), fNorm);
		if (XMVector3Dot(normal2, (CP - fVerts1)).m128_f32[0] > 0)
			continue;

		XMStoreFloat3(&vOut, CP);
		uiTriIndex = i;
	}
	if (uiTriIndex != -1)
		return true;
	else
		return false;

}
bool CCollisionManager::RayToCylinder(XMVECTOR& _rayPos, XMVECTOR& _rayDir, XMVECTOR& _botSpherePos, XMVECTOR& _topSpherePos, float _radius)
{
	XMVECTOR D = _topSpherePos - _botSpherePos;
	XMVECTOR m = _rayPos - _botSpherePos;
	float dd = XMVector3Dot(D, D).m128_f32[0]; // dot product of d with d (squared magnitude of d)
	float nd = XMVector3Dot(_rayDir, D).m128_f32[0]; // dot product of ray normal (n) with d
	float mn = XMVector3Dot(m, _rayDir).m128_f32[0];
	float md = XMVector3Dot(m, D).m128_f32[0];
	float mm = XMVector3Dot(m, m).m128_f32[0];

	float a = dd - nd * nd;
	float b = dd*mn - nd*md;
	float c = dd*(mm - _radius*_radius) - md*md;


	if (abs(a) < FLT_EPSILON)
	{
		return false;
	}

	if (c > 0.0f && b > 0.0f) //ray is outside circle and points away.
		return false;

	float discr = b*b - a*c;
	if (discr < 0.0f)
	{
		return false;
	}
	float t = (-b - sqrt(discr)) / a;
	if (t < 0.0f)
	{
		t = 0.0f;
	}

	XMVECTOR colPt = _rayPos + (_rayDir *t);

	if (XMVector3Dot(colPt - _botSpherePos, D).m128_f32[0] < 0)
		return false;
	if (XMVector3Dot(colPt - _topSpherePos, D).m128_f32[0] > 0)
		return false;

	return true;
}
bool CCollisionManager::RayToCapsule(XMVECTOR& _rayPos, XMVECTOR& _rayDir, XMVECTOR& _botSpherePos, XMVECTOR& _topSpherePos, float _radius, float& _time)
{
	//D = _topSpherePos - _botSpherePos
	_time = FLT_MAX;
	bool bReturn = false;
	if (RayToCylinder(_rayPos, _rayDir, _botSpherePos, _topSpherePos, _radius))
		return true;
	else if (RayToSphere(_rayPos, _rayDir, _botSpherePos, _radius) || RayToSphere(_rayPos, _rayDir, _topSpherePos, _radius))
		return true;
	// TODO: Complete this function

	return bReturn;

}
bool CCollisionManager::RayToSphere(XMVECTOR _rayPos, XMVECTOR _rayDir, XMVECTOR _spherePos, float _radius)
{

	XMVECTOR m = _rayPos - _spherePos;
	float b = XMVector3Dot(m, _rayDir).m128_f32[0];
	float c = (XMVector3Dot(m, m).m128_f32[0]) - (_radius * _radius); // Squared distance from start of ray to sphere surface
	if (c > 0.0f && b > 0.0f)
		return false; // Ray starts outside and points away
	float discr = b*b - c;
	if (discr < 0.0f)
		return false; // Negative discriminant means ray missedt = -b – Sqrt( discr );
	//if (t < 0.0f) 
	//	t = 0.0f; // Ray starts inside sphere, clamp to 0q = p + t * d;
	return true;

}
//bool CCollisionManager::RayToAabb(XMVECTOR _rayPos, XMVECTOR _rayDir, XMVECTOR _boxCenter, float _boxWidth)
//{
//
//}
std::vector<CGameObject*> CCollisionManager::RayCastEnemies(const XMVECTOR _start, XMVECTOR _dir)
{
	std::vector<CGameObject*> objsHit;
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->GetType() == ColliderType::eSphere)
		{
			CSphereCollider* sphrCol = ((CSphereCollider*)gameObjects[i]);
			if (sphrCol->eSphereType != SphereType::eEnemy)
				continue;
			if (sphrCol->GetOwner()->GetActive() == false)
				continue;
			bool col = RayToSphere(_start, _dir, XMLoadFloat3(&sphrCol->GetOwner()->GetPosition()), sphrCol->m_Radius);
			if (col)
			{
				objsHit.push_back(sphrCol->GetOwner()); //Object was hit, push it into vector.
			}
		}
		else if (gameObjects[i]->GetType() == ColliderType::eCapsule)
		{
			CCapsuleCollider* capCol = ((CCapsuleCollider*)gameObjects[i]);
			if (capCol->eCapsuleType != CapsuleType::eCapEnemy)
				continue;
			if (capCol->GetOwner()->GetActive() == false)
				continue;
			float fTime = 0.0f;
			XMVECTOR daStart = _start;
			capCol->SetUpSpheres();
			bool col = RayToCapsule(daStart, _dir, XMLoadFloat3(&capCol->m_BotSphere->vCenter), XMLoadFloat3(&capCol->m_TopSphere->vCenter), capCol->m_fRadius, fTime);
			if (col)
			{
				objsHit.push_back(capCol->GetOwner()); //Object was hit, push it into vector.
			}
		}
	}
	return objsHit;
}

std::vector<CGameObject*> CCollisionManager::RayCastActs(const XMVECTOR _start, XMVECTOR _dir)
{
	std::vector<CGameObject*> objsHit;
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->GetType() == ColliderType::eCapsule)
		{
			CCapsuleCollider* capCol = ((CCapsuleCollider*)gameObjects[i]);
			if (capCol->eCapsuleType != CapsuleType::eCapActivatable)
				continue;
			if (capCol->GetOwner()->GetActive() == false)
				continue;
			float fTime = 0.0f;
			XMVECTOR daStart = _start;
			capCol->SetUpSpheres();
			bool col = RayToCapsule(daStart, _dir, XMLoadFloat3(&capCol->m_BotSphere->vCenter), XMLoadFloat3(&capCol->m_TopSphere->vCenter), capCol->m_fRadius, fTime);
			if (col)
			{
				objsHit.push_back(capCol->GetOwner()); //Object was hit, push it into vector.
			}
		}
	}
	return objsHit;
}

std::vector<CGameObject*> CCollisionManager::RayCastStructs(const XMVECTOR _start, XMVECTOR _dir)
{
	std::vector<CGameObject*> objsHit;
	for (size_t i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->GetType() == ColliderType::eSphere)
		{
			CSphereCollider* sphrCol = ((CSphereCollider*)gameObjects[i]);
			if (sphrCol != nullptr) // TURRETS <----
			{
				if (sphrCol->eSphereType != SphereType::eStructure)
					continue;
				if (sphrCol->GetOwner()->GetActive() == false)
					continue;
				bool col = RayToSphere(_start, _dir, XMLoadFloat3(&sphrCol->GetOwner()->GetPosition()), sphrCol->m_Radius);
				if (col)
				{
					objsHit.push_back(sphrCol->GetOwner()); //Object was hit, push it into vector.
				}
			}

		}
	}
	for (size_t i = 0; i < triggerObjects.size(); i++)
	{
		//WALLS 
		CSphereTrigger* sphrTrig = ((CSphereTrigger*)triggerObjects[i]);
		if (sphrTrig != nullptr)
		{
			if (sphrTrig->eTriggerType != TriggerType::eWall)
				continue;
			if (sphrTrig->GetOwner()->GetActive() == false)
				continue;
			bool col = RayToSphere(_start, _dir, XMLoadFloat3(&sphrTrig->GetOwner()->GetPosition()), sphrTrig->m_Radius);
			if (col)
			{
				objsHit.push_back(sphrTrig->GetOwner()); //Object was hit, push it into vector.
			}
		}
	}
	return objsHit;
}
bool CCollisionManager::RayCastToTri(const XMVECTOR _start, XMVECTOR _dir, float _distance, TRayCastHit &_rayHit)
{


	XMVECTOR vEnd = _start + (_dir * _distance);
	XMFLOAT3 tempEnd;
	XMStoreFloat3(&tempEnd, vEnd);
	unsigned int triCount;
	int triIndex;
	//GET TRIANGLES FROM BVH
	XMFLOAT3 tempStart;
	XMStoreFloat3(&tempStart, _start);
	vector<TTriangle*>* tTri;
	tTri = m_GridSystem->GetAllTriangles();
	triCount = tTri->size();
	bool hit = LineSegmentToTriangle(_rayHit.vHitPt, triIndex, *tTri, triCount, tempStart, tempEnd);
	if (hit)
	{
		_rayHit.vNorm = (*tTri)[triIndex]->normal;
		XMStoreFloat3(&_rayHit.vDir, _dir);
		XMVECTOR tempHitPt = XMLoadFloat3(&_rayHit.vHitPt);
		XMVECTOR toHit = tempHitPt - _start;
		_rayHit.fDistance = XMVector3Length(toHit).m128_f32[0];
		toHit = XMVector3Normalize(toHit);

		XMStoreFloat3(&_rayHit.vDir, toHit);
		return true;
	}
	return false;
}
bool CCollisionManager::SphereToTriangle(const TSphere& sphere, const TTriangle& tri)
{

	XMVECTOR vCent = XMLoadFloat3(&sphere.vCenter);
	XMVECTOR fVerts0 = XMLoadFloat3(&tri.verts[0]), fVerts1 = XMLoadFloat3(&tri.verts[1]), fVerts2 = XMLoadFloat3(&tri.verts[2]);
	XMVECTOR fNorm = XMLoadFloat3(&tri.normal);



	float offset = XMVector3Dot((vCent - fVerts0), fNorm).m128_f32[0];
	//	XMVECTOR offsetV = fNorm * offset;
	XMVECTOR scaledNorm = fNorm * offset;
	XMVECTOR projectPt = vCent - scaledNorm;


	if (XMVector3Length(projectPt - vCent).m128_f32[0] > (sphere.fRadius * 2))
		return false;
	XMVECTOR closestPt;
	XMVECTOR edge[3];
	XMVECTOR eNorm[3];
	float ePlanesOf[3];
	XMVECTOR ePlanesNorm[3];
	int check[3];


	for (size_t i = 0; i < 3; i++)
	{
		int curr = i;
		int next = curr - 1;
		if (next == -1)
			next = 2;
		edge[i] = XMLoadFloat3(&tri.verts[next]) - XMLoadFloat3(&tri.verts[curr]);
		eNorm[i] = XMVector3Cross(edge[i], fNorm);
		ePlanesNorm[i] = eNorm[i];
		ePlanesOf[i] = XMVector3Dot(XMLoadFloat3(&tri.verts[curr]), eNorm[i]).m128_f32[0];
		if (XMVector3Dot(ePlanesNorm[i], projectPt).m128_f32[0] >= ePlanesOf[i])
			check[i] = 1;
		else
			check[i] = 2;
	}

	if (check[0] == check[1] && check[1] == check[2] && check[2] == 2)
	{
		closestPt = projectPt;
	}
	else
	{
		float dist[3];
		XMVECTOR edgeC[3];
		for (size_t i = 0; i < 3; i++)
		{
			int curr = i;
			int next = curr - 1;
			if (next == -1)
				next = 2;
			edgeC[i] = (XMLoadFloat3(&tri.verts[curr]) + (edge[curr] * .5f));
			dist[i] = XMVector3Length(edgeC[i]).m128_f32[0];
		}
		int closestE = 0;
		if (dist[0] > dist[1])
		{
			closestE = 1;
		}
		if (dist[closestE] > dist[2])
		{
			closestE = 2;
		}
		closestPt = edgeC[closestE];
	}

	float distance = XMVector3Length(vCent - closestPt).m128_f32[0];
	if (distance < sphere.fRadius)
	{
		return true;
	}
	return false;
}
vector<CGameObject*> CCollisionManager::OverlapCheckForAABB(TSphere _Sphere)
{
	vector<CGameObject*> vcInSphere;
	for (CAABBCollider* col : aabbCColliders)
	{
		if (col->GetOwner()->GetActive() == false)
			continue;

		//Checks gameobjects sphere collider to see if it's in the overlap Sphere
		if (SphereToAABB(_Sphere, *col))
		{
			vcInSphere.push_back(col->GetOwner());
		}

	}
	return vcInSphere;
}
vector<CGameObject*> CCollisionManager::OverlapCheckForAABB(XMFLOAT3 _center, float _radius)
{
	vector<CGameObject*> vcInSphere;
	for (CAABBCollider* col : aabbCColliders)
	{
		if (col->GetOwner()->GetActive() == false)
			continue;

		TSphere overLapSphere;
		overLapSphere.fRadius = _radius;
		overLapSphere.vCenter = _center;
		//Checks gameobjects sphere collider to see if it's in the overlap Sphere
		if (SphereToAABB(overLapSphere, *col))
		{
			vcInSphere.push_back(col->GetOwner());
		}

	}
	return vcInSphere;
}
vector<CGameObject*> CCollisionManager::OverlapSphere(TSphere _Sphere, SphereType _type)
{
	vector<CGameObject*> vcInSphere;
	for (ICollider* col : gameObjects)
	{
		CSphereCollider* sphrCol;
		if (col->GetType() == ColliderType::eSphere)
		{
			sphrCol = ((CSphereCollider*)col);
		}
		else
			continue;
		if (sphrCol->GetOwner()->GetActive() == false)
			continue;
		if (sphrCol->eSphereType == _type)
		{

			TSphere colSphere;
			colSphere.fRadius = sphrCol->m_Radius;
			colSphere.vCenter = sphrCol->GetOwner()->GetPosition();
			//Checks gameobjects sphere collider to see if it's in the overlap Sphere
			if (SphereToSphere(_Sphere, colSphere))
			{
				vcInSphere.push_back(sphrCol->GetOwner());
			}
		}

	}
	return vcInSphere;
}
vector<CGameObject*> CCollisionManager::OverlapSphere(XMFLOAT3 _center, float _radius, TriggerType _type)
{
	vector<CGameObject*> vcInSphere;
	for (ICollider* col : triggerObjects)
	{
		CSphereTrigger* sphrTrig;
		if (col->GetType() == ColliderType::eSphere)
		{
			sphrTrig = ((CSphereTrigger*)col);
		}
		if (sphrTrig->GetOwner()->GetActive() == false)
			continue;
		bool treeWall = (sphrTrig->eTriggerType == eTreeWall && _type == eWall);
		if (sphrTrig->eTriggerType == _type || treeWall)
		{

			TSphere colSphere, mySphere;
			if (_type == TriggerType::eWall)
			{
				colSphere.fRadius = sphrTrig->m_Radius *0.5f;
			}
			else
			{
				colSphere.fRadius = sphrTrig->m_Radius;
			}
			colSphere.vCenter = sphrTrig->GetOwner()->GetPosition();


			mySphere.fRadius = _radius;
			mySphere.vCenter = _center;
			//Checks gameobjects sphere collider to see if it's in the overlap Sphere
			if (SphereToSphere(mySphere, colSphere))
			{
				vcInSphere.push_back(sphrTrig->GetOwner());
			}
		}


	}
	return vcInSphere;
}
vector<CGameObject*> CCollisionManager::OverlapSphere(XMFLOAT3 _center, float _radius, CapsuleType _type)
{
	vector<CGameObject*> vcInSphere;
	for (ICollider* col : gameObjects)
	{

		CCapsuleCollider* capCol;
		if (col->GetType() == ColliderType::eCapsule)
		{
			capCol = ((CCapsuleCollider*)col);
		}
		else
			continue;
		if (capCol->GetOwner()->GetActive() == false)
			continue;
		if (capCol->eCapsuleType == _type)
		{

			TSphere mySphere;
			TCapsule colCap;
			capCol->SetUpSpheres();
			colCap.fRadius = capCol->m_fRadius;
			colCap.botPos = capCol->m_BotSphere->vCenter;
			colCap.topPos = capCol->m_TopSphere->vCenter;

			mySphere.fRadius = _radius;
			mySphere.vCenter = _center;
			//Checks gameobjects sphere collider to see if it's in the overlap Sphere
			if (CapsuleToSphere(colCap, mySphere))
			{
				vcInSphere.push_back(capCol->GetOwner());
			}
		}

	}
	return vcInSphere;
}
vector<CGameObject*> CCollisionManager::OverlapSphere(XMFLOAT3 _center, float _radius, SphereType _type)
{


	vector<CGameObject*> vcInSphere;
	for (ICollider* col : gameObjects)
	{

		CSphereCollider* sphrCol;
		if (col->GetType() == ColliderType::eSphere)
		{
			sphrCol = ((CSphereCollider*)col);
		}
		else
			continue;
		if (sphrCol->GetOwner()->GetActive() == false)
			continue;
		if (sphrCol->eSphereType == _type)
		{

			TSphere colSphere, mySphere;
			colSphere.fRadius = sphrCol->m_Radius;
			colSphere.vCenter = sphrCol->GetOwner()->GetPosition();

			mySphere.fRadius = _radius;
			mySphere.vCenter = _center;
			//Checks gameobjects sphere collider to see if it's in the overlap Sphere
			if (SphereToSphere(mySphere, colSphere))
			{
				vcInSphere.push_back(sphrCol->GetOwner());
			}
		}

	}
	return vcInSphere;
}
bool CCollisionManager::SphereToTriangleRes(const TSphere& sphere, const TTriangle& tri, XMFLOAT3& displacement)
{
	XMVECTOR vDisp = XMLoadFloat3(&displacement);
	XMVECTOR vCent = XMLoadFloat3(&sphere.vCenter);
	XMVECTOR fVerts0 = XMLoadFloat3(&tri.verts[0]), fVerts1 = XMLoadFloat3(&tri.verts[1]), fVerts2 = XMLoadFloat3(&tri.verts[2]);
	XMVECTOR fNorm = XMLoadFloat3(&tri.normal);

	XMVECTOR closestPt;

	float offset = XMVector3Dot((vCent - fVerts0), fNorm).m128_f32[0];
	//	XMVECTOR offsetV = fNorm * offset;
	XMVECTOR scaledNorm = fNorm * offset;
	XMVECTOR projectPt = vCent - scaledNorm;


	if (XMVector3Length(projectPt - vCent).m128_f32[0] > (sphere.fRadius * 2))
		return false;
	XMVECTOR edge[3];
	XMVECTOR eNorm[3];
	float ePlanesOf[3];
	XMVECTOR ePlanesNorm[3];
	int check[3];


	for (size_t i = 0; i < 3; i++)
	{
		int curr = i;
		int next = curr - 1;
		if (next == -1)
			next = 2;
		edge[i] = XMLoadFloat3(&tri.verts[next]) - XMLoadFloat3(&tri.verts[curr]);
		eNorm[i] = XMVector3Cross(edge[i], fNorm);
		ePlanesNorm[i] = eNorm[i];
		ePlanesOf[i] = XMVector3Dot(XMLoadFloat3(&tri.verts[curr]), eNorm[i]).m128_f32[0];
		if (XMVector3Dot(ePlanesNorm[i], projectPt).m128_f32[0] >= ePlanesOf[i])
			check[i] = 1;
		else
			check[i] = 2;
	}

	if (check[0] == check[1] && check[1] == check[2] && check[2] == 2)
	{
		closestPt = projectPt;
	}
	else
	{
		float dist[3];
		XMVECTOR edgeC[3];
		for (size_t i = 0; i < 3; i++)
		{
			int curr = i;
			int next = curr - 1;
			if (next == -1)
				next = 2;
			edgeC[i] = (XMLoadFloat3(&tri.verts[curr]));// possible revert
			dist[i] = XMVector3Length(edgeC[i]).m128_f32[0];
		}
		int closestE = 0;
		if (dist[0] > dist[1])
		{
			closestE = 1;
		}
		if (dist[closestE] > dist[2])
		{
			closestE = 2;
		}
		closestPt = edgeC[closestE];
	}

	float distance = XMVector3Length(vCent - closestPt).m128_f32[0];
	if (distance <= sphere.fRadius)
	{
		XMVECTOR V = vCent - closestPt;
		float tempDist = XMVector3Length(V).m128_f32[0];
		XMVECTOR tempNorm = XMVector3Normalize(V);
		if (XMVector3Dot(V, tempNorm).m128_f32[0] < 0)
			return false;
		XMStoreFloat3(&displacement, (tempNorm * (sphere.fRadius - tempDist)));
		return true;
	}
	return false;
}