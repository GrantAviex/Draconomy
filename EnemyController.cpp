

//EnemyController.cpp
//Created May 2015

#include "EnemyController.h"
#ifdef IARESERVER
#include "Manager.h"
#include "XTime.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "CandyContainer.h"
#include "ObjectManager.h"
#include "SphereTrigger.h"
#include "Server.h"
#include "Structure.h"
#include "Events.h"
#include "BulletController.h"
#include "UnitStats.h"
#include "ManaPickup.h"
#include "ManagerplayState.h"
#include "MeshRenderer.h"
#include "TextBillboard.h"
#include "AnimationController.h"
#include "CoinIndicator.h"
#include "Wwise_IDs.h"
#endif

using DirectX::XMStoreFloat4x4;

CEnemyController::CEnemyController(CGameObject* _Owner)
	: CComponent(_Owner, "EnemyController")
{
	currentPath = 1;
	pathDirection = 1;
	m_nCandy = 0;
	m_Velocity = XMFLOAT3(0, 0, 0);
	//XMVECTOR point1 = XMLoadFloat3(&XMFLOAT3(-3000, 720, 3800));
	//AddPathPoint(point1);

	m_pCollision = CManager::GetInstance()->m_ColManager;

	m_fHostileSpeedupSpeed = 250.0f;
	m_fHostileSpeedupDuration = 25.0f;
	m_fHostileSpeedupTimer = 0.0f;

	m_iUnstuckCountLimit = 22;
	m_fUnstuckDuration = 0.25f;
	m_fUnstuckDistanceLimit = 150.0f;
	m_fXplodeTimer = 0.0f;
	m_f3UnstuckLastPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_iUnstuckCount = 0;
	m_fUnstuckTimer = 0.0f;
	m_bIsAssassin = false;
	m_bIsJavThrower = false;
	m_bIsBomber = false;
	m_bHyperWave = false;
	m_bShowCarnage = false;
	m_MoveSpeedMult = 1.0f;
}

CEnemyController::~CEnemyController()
{
}
void CEnemyController::SelfDestruct()
{
	//Start timer??
	//bSDTimer = true;
	//nSDTimer = 3.0f;
	//OR Just blow up??
	std::vector<CGameObject*> playerVc;
	std::vector<CGameObject*> structVc;
	float explosionRadius = 350.0f;

	playerVc = m_pCollision->OverlapSphere(m_Owner->GetPosition(), explosionRadius, CapsuleType::eCapPlayer);
	structVc = m_pCollision->OverlapSphere(m_Owner->GetPosition(), explosionRadius, SphereType::eStructure);

	for (CGameObject* col : playerVc)
	{
		((CUnitStats*)col->GetComponent("UnitStats"))->TakeDamage(-50.0f);
	}
	for (CGameObject* col : structVc)
	{
		((CUnitStats*)col->GetComponent("UnitStats"))->TakeDamage(-50.0f);
	}
	structVc = m_pCollision->OverlapSphere(m_Owner->GetPosition(), explosionRadius, TriggerType::eWall);
	for (CGameObject* col : structVc)
	{
		((CUnitStats*)col->GetComponent("UnitStats"))->TakeDamage(-120.0f);
	}



	TEventData exploData;
	XMFLOAT3 tempPos = m_Owner->GetPosition();
	exploData.m[0] = tempPos.x;
	exploData.m[1] = tempPos.y;
	exploData.m[2] = tempPos.z;

	CManager::GetInstance()->GetServer()->SendEventMessage(EventID::CreateExplosion, exploData);

	m_Owner->SetActive(false);
	TEventData data;
	data.i = 0;
	data.s = "Explosion";
	XMFLOAT4X4 poss;
	XMStoreFloat4x4(&poss, m_Owner->GetWorldMatrix());
	XMFLOAT4X4 posMatrix;
	XMStoreFloat4x4(&posMatrix, m_Owner->GetWorldMatrix());
	memcpy(data.m, posMatrix.m, sizeof(float) * 16);
	CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, data);

	TEventData data2;
	data2.i = 2.5f;
	CManager::GetInstance()->GetServer()->SendEventToAllPlayers(EventID::CameraShake, data2);
}
void CEnemyController::ReceiveEvent(EventID id, TEventData const* data)
{
	switch (id)
	{
	case EventID::CandyPickUp:
	{
		if (m_nCandy <= 0)
		{
			m_Owner->GetObjMan()->GetObjectByID(data->i)->SetActive(false);
			m_nCandy++;
			m_CurrentDestination = PathingGoal::EscapePoint;

			TEventData data;
			data.i = 0;
			data.s = "EnemiesTakeCandy";
			XMFLOAT4X4 poss;
			XMStoreFloat4x4(&poss, m_Owner->GetWorldMatrix());
			XMFLOAT4X4 posMatrix;
			XMStoreFloat4x4(&posMatrix, m_Owner->GetWorldMatrix());
			memcpy(data.m, posMatrix.m, sizeof(float) * 16);
			CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, data);
		}
		break;
	}
	}
}

void CEnemyController::OnEnable()
{
	m_fSlowMult = 1.0f;
	m_fSlowTick = 0.0f;
	m_fSlowTimer = 0.0f;

	currentPath = 1;
	pathDirection = 1;
	m_nCandy = 0;
	m_Velocity = XMFLOAT3(0, 0, 0);

	//PLACEHOLDER VALUES
	m_AttackScanRange = m_AttackRange;
	m_AttackScanRangeForBuildings = m_AttackRange + 60;
	m_SoftCollisionRange = 80.0f;
	m_fAttackTargetScanTimer = 0.0f;
	m_fAttackTargetScanPeriod = 0.2f;
	m_fSoftCollisionScanTimer = 0.0f;
	m_fSoftCollisionScanPeriod = 0.1f;
	m_fAttackTimer = 0.0f;
	m_CurrentAction = EnemyAction::Moving;
	m_CurrentDestination = CandyBarrel;
	m_bEscaped = false;


	m_fHostileSpeedupTimer = 0.0f;
	m_f3UnstuckLastPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_iUnstuckCount = 0;
	m_fUnstuckTimer = 0.0f;

	for (auto& child : m_Owner->GetChildren())
	{
		if (child->GetName() == "CandyCounter")
		{
			child->SetActive(false);
			m_Owner->GetObjMan()->ObjectChanged(child, child->GetID());
		}
	}

	m_WknDmgMult = 1.0f;

	
	m_fXplodeTimer = -1.0f;

	m_fDragonSlayerAnimTimer = 0.0f;
}

void CEnemyController::OnDisable()
{
	//m_vPath.clear();

	if (m_bEscaped)
	{
		CManager::GetInstance()->GetCandyContainer()->m_MaxCandy--;
		TEventData data;
		data.i = 0;
		CManager::GetInstance()->GetServer()->GetEventSystem()->SendEventImmediate(EventID::EnemyEscaped, &TEventData());

		if (CManager::GetInstance()->GetCandyContainer()->m_MaxCandy == 0)
		{
			data.s = "Lose";
			CManager::GetInstance()->GetServer()->SendEventMessage(EventID::Lose, data);

			data.i = 1;
			CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, data);

			CManagerplayState::GetInstance()->SendScoresToServer();
		}
	}
	else
	{
		//Send killed event
		CManager::GetInstance()->GetServer()->GetEventSystem()->SendEventImmediate(EventID::EnemyKilled, &TEventData());

		//Drop mana pickup
		float currentDropValue = m_ManaValue * CManagerplayState::GetInstance()->GetManaDropMult();
		string manaPrefab = "ManaPickup";
		if (currentDropValue == 0.0f)
		{
			manaPrefab = "UpgradePickup";

			auto position = m_Owner->GetMatrix();
			position.m[3][1] += 60.0f;
			auto eventSystem = m_Owner->GetEventSystem();
			auto objectManger = m_Owner->GetObjMan();
			vector<int> ids = CManager::GetInstance()->GetServer()->GetPlayerIDs();
			float x, z;
			for (auto& id : ids)
			{
				x = float(rand() % 100 - 50);
				z = float(rand() % 100 - 50);
				position.m[3][0] += x;
				position.m[3][2] += z;
				auto object = objectManger->CreateObject(manaPrefab, position, eventSystem);
				auto trigger = (CSphereTrigger*)object->GetComponent("SphereTrigger");
				trigger->eTriggerType = TriggerType::eUpgradePickup;
				auto upgrade = (CManaPickup*)object->GetComponent("UpgradePickup");
				upgrade->SetValue((float)id);
				TEventData tdata;
				tdata.i = object->GetID();
				CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID::SetBillboardVisibility, tdata, id);
			}
		}
		else
		{
			float scale = 1.2f + (currentDropValue * 0.03f);
			auto position = m_Owner->GetMatrix();
			position.m[3][1] += 60.0f;
			auto eventSystem = m_Owner->GetEventSystem();
			auto objectManger = m_Owner->GetObjMan();
			auto object = objectManger->CreateObject(manaPrefab, position, eventSystem);
			object->SetLocalScale(scale, scale, scale);
			auto trigger = (CSphereTrigger*)object->GetComponent("SphereTrigger");
			trigger->eTriggerType = TriggerType::eManaPickup;
			auto mana = (CManaPickup*)object->GetComponent("ManaPickup");
			mana->SetValue(currentDropValue);
		}


		//Drop candy pickup
		if (m_nCandy > 0)
		{
			auto position = m_Owner->GetMatrix();
			auto eventSystem = m_Owner->GetEventSystem();
			auto objectManger = m_Owner->GetObjMan();
			auto object = objectManger->CreateObject("Candy", position, eventSystem);
			CSphereTrigger* trigger = (CSphereTrigger*)object->GetComponent("SphereTrigger");
			trigger->eTriggerType = TriggerType::eCandy;
		}
		if (m_bShowCarnage)
		{
			string name = m_Owner->GetName();
			name += "Carnage";
			CGameObject* carnage = m_Owner->GetObjMan()->CreateObject(name, m_Owner->GetMatrix(), m_Owner->GetEventSystem());
		}
	}

	CManagerplayState::GetInstance()->EnemyDied();
	m_nCandy = 0;
	m_bHyperWave = false;
	m_Owner->SetPosition(0, 1000, -25000);
}
XMFLOAT3 CEnemyController::ProjectPlacement(int _steps)
{
	XMFLOAT3 tempPos = m_Owner->GetPosition();
	XMVECTOR tempVecPos = XMLoadFloat3(&m_Owner->GetPosition());
	CVectorField* vectorField = CManagerplayState::GetInstance()->GetVectorField();
	for (size_t i = 0; i < _steps; i++)
	{
		XMStoreFloat3(&tempPos, tempVecPos);
		XMFLOAT3 vector = vectorField->GetVector(m_CurrentDestination, tempPos);
		XMVECTOR tempVect = XMLoadFloat3(&vector);
		tempVecPos += tempVect * 40;
	}
	XMStoreFloat3(&tempPos, tempVecPos);
	return tempPos;
}
bool CEnemyController::MoveAlongPath()
{
	float dt = CManager::GetInstance()->GetDelta();

	XMMATRIX mat = m_Owner->GetWorldMatrix();
	XMFLOAT3 position;
	XMStoreFloat3(&position, mat.r[3]);
	auto oldPos = mat.r[3];
	CVectorField* vectorField = CManagerplayState::GetInstance()->GetVectorField();
	XMFLOAT3 vector = vectorField->GetVector(m_CurrentDestination, position);
	int stepsFromGoal = vectorField->GetDistance(m_CurrentDestination, position);

	//Sewer pushback
	if (pushbackTimer > 0)
	{
		XMFLOAT3 pushbackDirection = vectorField->GetVector(PathingGoal::CandyBarrel, position);
		XMVECTOR pushVec = XMLoadFloat3(&pushbackDirection) * -300 * dt;
		XMMATRIX pushbackbMat = XMMatrixTranslationFromVector(pushVec);
		m_Owner->SetMatrix(&(mat * pushbackbMat));
		pushbackTimer -= dt;
		m_fUnstuckTimer = 0.0f;
		return true;
	}

	XMVECTOR lookDir = XMLoadFloat3(&vector);
	//XMVECTOR moveDir = mat.r[2];
	XMVECTOR moveDir = XMLoadFloat3(&vector);
	float hostileSpeedupAmount = (m_fHostileSpeedupTimer / m_fHostileSpeedupDuration) * m_fHostileSpeedupSpeed;
	float adjustedMovespeed = ((m_MaxMoveSpeed*m_MoveSpeedMult) + hostileSpeedupAmount) * m_fSlowMult;
	if (m_bIsAssassin)
	{
		adjustedMovespeed = m_MaxMoveSpeed;
	}
	if (m_fXplodeTimer <= 0.0f && m_fXplodeTimer > -1.0f)
		adjustedMovespeed *= 2.8f;
	XMVECTOR moveVec = moveDir * adjustedMovespeed * dt;
	XMMATRIX translationMat = XMMatrixTranslationFromVector(moveVec);
	m_Owner->SetMatrix(&(mat * translationMat));

	XMFLOAT3 p;
	XMStoreFloat3(&p, oldPos);
	XMVECTOR lookPoint = XMLoadFloat3(&p) + (lookDir * 100.0f);

	LookAt(lookPoint);
	//m_Owner->LookAt(lookPoint, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	if (stepsFromGoal <= 3)
	{
		//Get candy from barrel
		if (m_nCandy == 0 && m_CurrentDestination == CandyBarrel)
		{
			auto candyBarrel = CManager::GetInstance()->GetCandyContainer();

			if (candyBarrel->m_CurrCandy > 0)
			{
				candyBarrel->m_CurrCandy--;
				m_nCandy += 1;
				m_CurrentDestination = PathingGoal::EscapePoint;

				TEventData data;
				data.i = 0;
				data.s = "EnemiesTakeCandy";
				XMFLOAT4X4 poss;
				XMStoreFloat4x4(&poss, m_Owner->GetWorldMatrix());
				XMFLOAT4X4 posMatrix;
				XMStoreFloat4x4(&posMatrix, m_Owner->GetWorldMatrix());
				memcpy(data.m, posMatrix.m, sizeof(float) * 16);
				CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, data);

				data.i = candyBarrel->GetOwner()->GetID();
				data.s = "CandyTaken";
				CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetEmitterActive, data);

				data.m[0] = 1.f;
				data.m[3] = 3;
				data.s = "    Gold Taken!";
				CManager::GetInstance()->GetServer()->SendEventMessage(EventID::DisplayHUDText, data);
			}
		}

		//Disappear at escape point
		else if (m_CurrentDestination == EscapePoint)
		{
			m_bEscaped = true;
			m_Owner->SetActive(false);
			return false;
		}
	}
	return true;
}
void CEnemyController::Cleave()
{
	std::vector<CGameObject*> playerVc;
	std::vector<CGameObject*> structVc;
	float explosionRadius = 350.0f;
	XMVECTOR cleaveVec = XMLoadFloat3(&m_Owner->GetPosition());
	XMVECTOR zAxis = XMLoadFloat3(&m_Owner->GetAxis(2));
	zAxis *= 250;
	cleaveVec += zAxis;
	XMFLOAT3 cleavePt;
	XMStoreFloat3(&cleavePt, cleaveVec);
	playerVc = m_pCollision->OverlapSphere(cleavePt, explosionRadius, CapsuleType::eCapPlayer);
	structVc = m_pCollision->OverlapSphere(cleavePt, explosionRadius, SphereType::eStructure);

	for (CGameObject* col : playerVc)
	{
		((CUnitStats*)col->GetComponent("UnitStats"))->TakeDamage(-m_AttackDamage);
	}
	for (CGameObject* col : structVc)
	{
		((CUnitStats*)col->GetComponent("UnitStats"))->TakeDamage(-m_AttackDamage * 1.5f);
	}
	structVc.clear();
	structVc = m_pCollision->OverlapSphere(m_Owner->GetPosition(), explosionRadius, TriggerType::eWall);
	for (CGameObject* col : structVc)
	{
		((CUnitStats*)col->GetComponent("UnitStats"))->TakeDamage(-m_AttackDamage * 2.0f);
	}

}
void CEnemyController::LookAt(XMVECTOR pos)
{
#ifdef IARESERVER
	XMVECTOR lookerPos = m_Owner->GetWorldMatrix().r[3];
	XMVECTOR lookerDir = pos - lookerPos;
	lookerDir = XMVector3Normalize(lookerDir);
	XMMATRIX lookerMatrix = m_Owner->GetWorldMatrix();
	float time = (float)(CManager::GetInstance()->GetDelta());

	float yRot = XMVector3Dot(lookerDir, lookerMatrix.r[0]).m128_f32[0];
	float TURNSPEED = 5.0f;
	m_Owner->Rotate(0, XMConvertToDegrees(yRot * TURNSPEED * time), 0);
#endif
}
void CEnemyController::SetSlowedStatus(float _slowAmt)
{
	if (_slowAmt < 0.4f)
	{
		int stunChance = rand() % 8;
		if (stunChance == 5)
		{
			m_fSlowMult = 0.0f;
			m_fSlowTimer = .6f;
			TEventData statData;
			statData.i = 2;
			statData.m[0] = (float)m_Owner->GetID();
			CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetStatusEffect, statData);
			return;
		}
	}
	if ((m_fSlowMult == 0.0f && m_fSlowTimer > 0) || (m_fSlowMult < _slowAmt && m_fSlowTimer > 0))
		return;
	m_fSlowMult = _slowAmt;
	m_fSlowTimer = 1.5f;
	//Make Slow Emitter ACTIVE
	//((CMeshRenderer*)m_Owner->GetComponent("MeshRenderer"))->SetColorModifier(.2f, 1.0f, 0.2f, 1.0f);
	m_Owner->GetObjMan()->CreateObject("EnemyColdEffect", m_Owner->GetMatrix(), m_Owner->GetEventSystem());

}
void CEnemyController::SetSlowedStatus(float _slowAmt, float _slowTimer)
{
	if (_slowAmt < 0.4f)
	{
		int stunChance = rand() % 8;
		if (stunChance == 5)
		{
			m_fSlowMult = 0.0f;
			m_fSlowTimer = .6f;
			TEventData statData;
			statData.i = 2;
			statData.m[0] = (float)m_Owner->GetID();
			CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetStatusEffect, statData);
			return;
		}
	}
	if ((m_fSlowMult == 0.0f && m_fSlowTimer > 0) || (m_fSlowMult < _slowAmt && m_fSlowTimer > 0))
		return;
	m_fSlowMult = _slowAmt;
	m_fSlowTimer = _slowTimer;
	//Make Slow Emitter ACTIVE
	//((CMeshRenderer*)m_Owner->GetComponent("MeshRenderer"))->SetColorModifier(.2f, 1.0f, 0.2f, 1.0f);
	m_Owner->GetObjMan()->CreateObject("EnemyColdEffect", m_Owner->GetMatrix(), m_Owner->GetEventSystem());

}
void CEnemyController::Update(float REMOVE_THIS_PARAMETER)
{
	float dt = CManager::GetInstance()->GetDelta();
	if (m_Owner->GetPosition().y <= -10)
	{
		m_Owner->SetPosition(XMFLOAT3(0.0f, 40.0f, 0.0f));
	}
	if (m_bIsBomber)
	{
		if (m_fXplodeTimer > 0)
		{
			m_fXplodeTimer -= dt;
			return;
		}
		if (bombTarget != nullptr)
		{
			//Create Fuze Effect
		}
	}
	if (m_fSlowTimer > 0)
	{
		m_fSlowTimer -= dt;
		m_fSlowTick += dt;
		if (m_fSlowTimer <= 0.0f)
		{
			m_fSlowTimer = 0.0f;
			m_fSlowMult = 1.0f;
			m_fSlowTick = 0.0f;

			TEventData statData;
			statData.i = -2;
			statData.m[0] = (float)m_Owner->GetID();
			CManager::GetInstance()->GetServer()->SendEventMessage(EventID::SetStatusEffect, statData);
			//Make Slow Emitter UNACTIVE.
			//((CMeshRenderer*)m_Owner->GetComponent("MeshRenderer"))->SetColorModifier(1.0f, 1.0f, 1.0f, 1.0f);

		}
		else if (m_fSlowTick > .15f)
		{
			m_Owner->GetObjMan()->CreateObject("EnemyColdEffect", m_Owner->GetMatrix(), m_Owner->GetEventSystem());
			m_fSlowTick = 0.0f;
		}
	}
	if (m_fSlowMult == 0.0f && m_fSlowTimer > 0)
		return;
	//Find attack targets
	m_fAttackTargetScanTimer += dt;
	if (m_fAttackTargetScanTimer >= m_fAttackTargetScanPeriod)
	{
		m_fAttackTargetScanTimer -= m_fAttackTargetScanPeriod;
		m_fAttackTargetScanTimer -= (float)(rand() % 50) / 1000.0f;
		if (m_bIsJavThrower)
		{
			FindNearbyObjects(SphereType::eStructure, m_Owner->GetPosition(), m_AttackScanRangeForBuildings + 200);


			if (m_NearbyObjects[SphereType::eStructure].size() == 0)
			{
				XMFLOAT3 projPos = ProjectPlacement(30);
				FindNearbyObjects(SphereType::eStructure, projPos, m_AttackScanRangeForBuildings + 200);
			}

		}
		else if (m_bIsBomber)
		{
			FindNearbyObjects(SphereType::eStructure, m_Owner->GetPosition(), 400);


			if (m_NearbyObjects[SphereType::eStructure].size() == 0)
			{
				XMFLOAT3 projPos = ProjectPlacement(15);
				FindNearbyObjects(SphereType::eStructure, projPos, 400);
				if (m_NearbyObjects[SphereType::eStructure].size() != 0)
				{
					m_CurrentDestination = Structures;
					m_NearbyObjects[SphereType::eStructure].clear();
				}
			}
			
		}
		else
		{
			FindNearbyObjects(SphereType::eStructure, m_Owner->GetPosition(), m_AttackScanRangeForBuildings);
		}
		float scanRange = m_AttackScanRange;
		if (m_bIsAssassin)
		{
			scanRange += 700.0f;
		}
		else if (m_bIsJavThrower)
		{
			scanRange += 200.0f;
		}
		FindNearbyObjects(CapsuleType::eCapPlayer, m_Owner->GetPosition(), scanRange);

		//Start attacking structures with sphere colliders
		if (m_CurrentAction == EnemyAction::Moving && m_NearbyObjects[SphereType::eStructure].size() > 0)
		{
			if (m_bIsAssassin == false)
			{
				m_CurrentAction = EnemyAction::AttackingStructure;

				CAnimationController* animInfo = (CAnimationController*)m_Owner->GetComponent("AnimationController");
				if (animInfo)
				{
					animInfo->SetAnimation("Attack");
				}
				if (!m_bIsAssassin)
				{
					m_fAttackTimer = 0.0f;
				}
			}
		}
		//Start attacking players
		if (m_CurrentAction == EnemyAction::Moving && m_NearbyObjects[CapsuleType::eCapPlayer].size() > 0)
		{
			m_CurrentAction = EnemyAction::AttackingPlayer;

			CAnimationController* animInfo = (CAnimationController*)m_Owner->GetComponent("AnimationController");
			if (animInfo)
			{
				animInfo->SetAnimation("Attack");
			}
			if (!m_bIsAssassin)
			{
				m_fAttackTimer = 0.0f;
			}
		}
	}


	//Soft collision
	m_fSoftCollisionScanTimer += dt;
	if (m_fSoftCollisionScanTimer >= m_fSoftCollisionScanPeriod)
	{
		m_fSoftCollisionScanTimer -= m_fSoftCollisionScanPeriod;
		FindNearbyObjects(CapsuleType::eCapEnemy, m_Owner->GetPosition(), m_SoftCollisionRange);
	}
	for (auto& obj : m_NearbyObjects[CapsuleType::eCapEnemy])
	{
		if (obj != m_Owner && obj->GetActive() == true)
		{
			XMVECTOR self = XMLoadFloat3(&m_Owner->GetPosition());
			XMVECTOR other = XMLoadFloat3(&obj->GetPosition());
			XMVECTOR from = self - other;
			from = XMVector3Normalize(from);
			float softspeed = 120.0f;
			from *= softspeed * dt;

			XMFLOAT3 v;
			XMStoreFloat3(&v, from);
			v.y = 0;
			m_Owner->Translate(v);
		}
	}
	//Show candy above enemy?
	CCoinIndicator* indicator = (CCoinIndicator*)m_Owner->GetComponent("CoinIndicator");
	indicator->SetIHasMoney(m_nCandy != 0);


	//Move
	if (m_CurrentAction == EnemyAction::Moving)
	{
		if (!MoveAlongPath())
			return;
		CAnimationController* animInfo = (CAnimationController*)m_Owner->GetComponent("AnimationController");
		if (animInfo)
		{
			animInfo->SetAnimation("Walk");
		}
		if (m_Owner->GetActive() == false)
		{
			return;
		}
	}
	//BOMBER stuff
	if (m_bIsBomber)
	{
		if (m_CurrentDestination != EscapePoint)
		{
			CVectorField* vectorField = CManagerplayState::GetInstance()->GetVectorField();

			int stepsToStructs = vectorField->GetDistance(m_CurrentDestination, m_Owner->GetPosition());
			if (m_CurrentDestination == Structures)
			{
				if (m_fXplodeTimer <= -0.90f)
				{
					m_fXplodeTimer = 1.65f;
				}
			}
		}

		bool structuresExist = false;
		for (auto& structStuff : m_Owner->GetObjMan()->GetAllTypeEnemy("Turret"))
		{
			if (structStuff->GetActive())
			{
				structuresExist = true;

			}
		}
		for (auto& structStuff : m_Owner->GetObjMan()->GetAllTypeEnemy("StaticWall"))
		{
			if (structStuff->GetActive())
			{
				structuresExist = true;
			}
		}

		if (m_CurrentDestination == Structures && structuresExist == FALSE)
		{
			m_CurrentDestination = CandyBarrel;
		}
	}
	//Fall
	if (m_Owner->GetGrounded() == false)
	{
		m_Velocity.y -= 25.0f * dt;
	}
	else
	{
		m_Velocity.y = 0.0f;
	}
	//if (m_Velocity.y <= -2500.0f)
	//{
	//	m_Velocity.y = -2500.0f;
	//}
	//m_Velocity.y *= dt;

	auto candyBarrel = CManager::GetInstance()->GetCandyContainer();

	bool droppedCandyPickupsExist = false;
	for (auto& candy : m_Owner->GetObjMan()->GetAllTypeEnemy("Candy"))
	{
		if (candy->GetActive())
		{
			droppedCandyPickupsExist = true;
		}
	}

	//If targeting candy barrel, and it's empty, switch to targeting candy or wizards
	if (m_CurrentDestination == CandyBarrel && candyBarrel->m_CurrCandy == 0)
	{
		if (droppedCandyPickupsExist)
		{
			m_CurrentDestination = DroppedCandy;
		}
		else
		{
			m_CurrentDestination = Wizard;
		}
	}

	//If targeting dropped candy, and none exists, switch to targeting wizards
	if (m_CurrentDestination == DroppedCandy && droppedCandyPickupsExist == false)
	{
		m_CurrentDestination = Wizard;
	}

	//If targeting wizard, and candy returns to candy barrel, switch to targeting candy barrel
	if (m_CurrentDestination == Wizard && candyBarrel->m_CurrCandy > 0)
	{
		m_CurrentDestination = CandyBarrel;
	}

	//If targeting wizard, and candy is dropped, switch to targeting dropped candy
	if (m_CurrentDestination == Wizard && droppedCandyPickupsExist && candyBarrel->m_CurrCandy == 0)
	{
		m_CurrentDestination = DroppedCandy;
	}

	//Assassin
	if (m_bIsAssassin)
	{
		m_CurrentDestination = Wizard;
	}

	//Target only wizard if in hyperwave mode
	if (m_bHyperWave)
	{
		m_CurrentDestination = Wizard;
	}

	//Hostile speedup
	if (m_CurrentDestination == Wizard)
	{
		if (m_fHostileSpeedupTimer < m_fHostileSpeedupDuration)
		{
			m_fHostileSpeedupTimer += dt;
		}
	}
	else
	{
		m_fHostileSpeedupTimer = 0.0f;
	}

	//Attack
	if (m_bIsAssassin)
	{
		m_fAttackTimer += dt;
		m_fDragonSlayerAnimTimer += dt;
	}

	if (m_CurrentAction == AttackingStructure || m_CurrentAction == AttackingPlayer)
	{
		int targetType = (m_CurrentAction == AttackingStructure) ? eStructure : eCapPlayer;

		if (m_bIsAssassin && m_fDragonSlayerAnimTimer > 1.15f && m_NearbyObjects[targetType].size() == 0)
		{
			m_CurrentAction = EnemyAction::Moving;
		}
		else if (m_bIsAssassin == false && m_NearbyObjects[targetType].size() == 0)
		{
			m_CurrentAction = EnemyAction::Moving;
		}
		else
		{
			m_Velocity.y = 0.0f;
			CGameObject* target = nullptr;
			if (m_NearbyWalls.size() != 0)
			{
				target = m_NearbyWalls[0];
			}
			else
			{
				if (m_NearbyObjects[targetType].size() == 0)
				{
					target = nullptr;
				}
				else
				{
					target = m_NearbyObjects[targetType][0];
				}
			}

			if (target != nullptr)
			{
				LookAt(XMLoadFloat3(&target->GetPosition()));
				if (m_bIsBomber)
				{
					m_fXplodeTimer = -1.0f;
					SelfDestruct();
				}
				if (!m_bIsAssassin)
				{
					m_fAttackTimer += dt;
				}

				if (m_fAttackTimer >= m_AttackCooldown)
				{
					m_fAttackTimer = 0.0f;
					if (target->GetActive() == TRUE)
					{
						if (m_bIsAssassin == TRUE)
						{
							m_fDragonSlayerAnimTimer = 0.0f;

							//Assassin attack
							XMFLOAT4X4 m;
							XMStoreFloat4x4(&m, m_Owner->GetWorldMatrix());
							//m._42 += 120.0f;
							CGameObject* pBullet = m_Owner->GetObjMan()->CreateObject("EnemyAssassinProjectile", m, m_Owner->GetEventSystem());
							auto bulletController = (CBulletController*)pBullet->GetComponent("BulletController");
							bulletController->SetPiercing(true);
							auto bulletTrigger = (CSphereTrigger*)pBullet->GetComponent("SphereTrigger");
							if (bulletController == nullptr || bulletTrigger == nullptr)
							{
								printf("Error in EnemyController: Null component.\n");
							}
							bulletController->SetLifeTime(3.0f);
							bulletController->SetWhoShotMe(m_Owner);
							bulletController->SetTarget(target);
							bulletController->SetHomingBool(false);
							bulletController->SetDamageMult(m_WknDmgMult);
							XMVECTOR position = XMLoadFloat3(&m_Owner->GetPosition());
							XMVECTOR targetPosition = XMLoadFloat3(&target->GetPosition());
							XMVECTOR to = position - targetPosition;
							targetPosition = position + to;
							XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
							bulletController->GetOwner()->LookAt(targetPosition, up);
							bulletController->GetOwner()->Translate(XMFLOAT3(0.0f, 60.0f, 0.0f));
							bulletTrigger->eTriggerType = TriggerType::eBullet;

							TEventData tdata;
							//tdata.i = (int)AK::EVENTS::PLAY_3D_FIREBALL;
							//tdata.m[12] = m_Owner->GetWorldMatrix().r[3].m128_f32[0];
							//tdata.m[13] = m_Owner->GetWorldMatrix().r[3].m128_f32[1];
							//tdata.m[14] = m_Owner->GetWorldMatrix().r[3].m128_f32[2];
							CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, tdata);
						}
						else if (m_bIsJavThrower == TRUE)
						{
							//Assassin attack
							XMFLOAT4X4 m;
							XMStoreFloat4x4(&m, m_Owner->GetWorldMatrix());
							//CGameObject* pBullet = m_Owner->GetObjMan()->CreateObject("EnemyJavelinProjectile", m, m_Owner->GetEventSystem());
							CGameObject* pBullet = m_Owner->GetObjMan()->CreateObject("EnemyMageProjectile", m, m_Owner->GetEventSystem());

							auto bulletController = (CBulletController*)pBullet->GetComponent("BulletController");
							auto bulletTrigger = (CSphereTrigger*)pBullet->GetComponent("SphereTrigger");
							if (bulletController == nullptr || bulletTrigger == nullptr)
							{
								printf("Error in EnemyController: Null component.\n");
							}
							bulletController->SetLifeTime(3.0f);
							bulletController->SetWhoShotMe(m_Owner);
							bulletController->SetTarget(target);
							bulletController->SetHomingBool(false);
							bulletController->SetDamageMult(m_WknDmgMult);
							XMVECTOR position = XMLoadFloat3(&m_Owner->GetPosition());
							XMVECTOR targetPosition = XMLoadFloat3(&target->GetPosition());
							XMVECTOR to = position - targetPosition;
							targetPosition = position + to;
							XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
							bulletController->GetOwner()->LookAt(targetPosition, up);
							bulletController->GetOwner()->Translate(XMFLOAT3(0.0f, 60.0f, 0.0f));
							bulletTrigger->eTriggerType = TriggerType::eBullet;

							TEventData tdata;
							tdata.i = (int)AK::EVENTS::PLAY_3D_FIREBALL;
							tdata.m[12] = m_Owner->GetWorldMatrix().r[3].m128_f32[0];
							tdata.m[13] = m_Owner->GetWorldMatrix().r[3].m128_f32[1];
							tdata.m[14] = m_Owner->GetWorldMatrix().r[3].m128_f32[2];
							CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, tdata);
						}
						else
						{
							string enemyName = m_Owner->GetName();
							if (enemyName == "EnemyOgre")
							{
								Cleave();

								XMFLOAT4X4 effectMatrix;
								auto forward = m_Owner->GetAxis(2);
								forward.x *= 300.0f;
								forward.y *= 300.0f;
								forward.z *= 300.0f;
								auto trans = XMMatrixTranslation(forward.x, forward.y + 60.0f, forward.z);
								XMStoreFloat4x4(&effectMatrix, m_Owner->GetWorldMatrix() * trans);
								m_Owner->GetObjMan()->CreateObject("EnemyCleaveEffect", effectMatrix, m_Owner->GetEventSystem());

								TEventData sdata;
								sdata.s = "EnemyAttack";
								XMFLOAT4X4 matrix;
								XMStoreFloat4x4(&matrix, m_Owner->GetWorldMatrix());
								memcpy(sdata.m, matrix.m, sizeof(float) * 16);
								CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, sdata);
							}
							else
							{
								CUnitStats* turretStats = (CUnitStats*)(target->GetComponent("UnitStats"));
								if (turretStats != nullptr)
								{
									turretStats->TakeDamage(-(m_AttackDamage * m_WknDmgMult), m_Owner->GetID());
									TEventData data;
									data.i = (int)turretStats->GetCurrentHealth();
									CManager::GetInstance()->GetServer()->SendPlayerEventMessage(EventID::UpdateHealth, data, target->GetID());

									XMFLOAT4X4 effectMatrix;
									auto forward = m_Owner->GetAxis(2);
									forward.x *= 150.0f;
									forward.y *= 150.0f;
									forward.z *= 150.0f;
									auto trans = XMMatrixTranslation(forward.x, forward.y + 60.0f, forward.z);
									XMStoreFloat4x4(&effectMatrix, m_Owner->GetWorldMatrix() * trans);
									m_Owner->GetObjMan()->CreateObject("EnemyHitEffect", effectMatrix, m_Owner->GetEventSystem());

									TEventData sdata;
									sdata.s = "EnemyAttack";
									XMFLOAT4X4 matrix;
									XMStoreFloat4x4(&matrix, m_Owner->GetWorldMatrix());
									memcpy(sdata.m, matrix.m, sizeof(float) * 16);
									CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, sdata);
								}
								else
								{
									printf("Error in EnemyController: Attack target has no UnitStats component.\n");
								}
							}
						}
					}
				}
			}
		}
	}
	//FALL CONTINUED
	m_Owner->Translate(m_Velocity);

	//Unstucker
	if (m_CurrentAction != EnemyAction::Moving)
	{
		m_iUnstuckCount = 0;
	}
	m_fUnstuckTimer += dt;
	if (m_fUnstuckTimer >= m_fUnstuckDuration)
	{
		m_fUnstuckTimer = 0.0f;

		XMFLOAT3 currentPosition = m_Owner->GetPosition();
		currentPosition.y = 0.0f;
		XMVECTOR posVec = XMLoadFloat3(&currentPosition);
		XMVECTOR lastStuckVec = XMLoadFloat3(&m_f3UnstuckLastPosition);
		XMVECTOR lengthVec = XMVector3Length(posVec - lastStuckVec);
		XMFLOAT3 lengthf3;
		XMStoreFloat3(&lengthf3, lengthVec);
		float length = lengthf3.x;

		if (length > m_fUnstuckDistanceLimit)
		{
			m_f3UnstuckLastPosition = currentPosition;
			m_iUnstuckCount = 0;
		}
		else
		{
			m_iUnstuckCount += 1;
			if (m_iUnstuckCount >= m_iUnstuckCountLimit)
			{
				m_iUnstuckCount = 0;

				//Effect
				XMFLOAT4X4 effectMatrix;
				XMStoreFloat4x4(&effectMatrix, m_Owner->GetWorldMatrix());
				m_Owner->GetObjMan()->CreateObject("UnstuckEffect", effectMatrix, m_Owner->GetEventSystem());

				//Move to spawn point
				m_Owner->SetPosition(XMFLOAT3(0.0f, 60.0f, 0.0f));

				//Dragonslayer escape
				if (m_bIsAssassin)
				{
					if (m_nCandy > 0)
					{
						m_bEscaped = true;
						m_Owner->SetActive(false);
					}
					else
					{
						m_Owner->SetPosition(XMFLOAT3(0.0f, -500.0f, 0.0f));
						((CUnitStats*)m_Owner->GetComponent("UnitStats"))->TakeDamage(-100000.0f);
					}
				}
			}
		}
	}
}

void CEnemyController::FindNearbyObjects(int type, XMFLOAT3 position, float radius)
{
	//Clear previous targets
	m_NearbyObjects[type].clear();
	SphereType tempType = (SphereType)-1;
	CapsuleType tempType2 = (CapsuleType)-1;
	if (type < 3)
	{
		tempType = (SphereType)type;
	}
	else
	{
		tempType2 = (CapsuleType)type;
	}
	if (tempType != (SphereType)-1)
	{
		auto colliders = m_pCollision->OverlapSphere(position, radius, tempType);
		for (auto& collider : colliders)
		{
			auto asdasd = collider;
			CComponent* comp = nullptr;
			comp = asdasd->GetComponent("Structure");
			if (comp != nullptr && ((CStructure*)comp)->GetStructureActive() == FALSE)
				continue;
			m_NearbyObjects[type].push_back(collider);
		}
	}
	else if (tempType2 != (CapsuleType)-1)
	{
		auto colliders = m_pCollision->OverlapSphere(position, radius, tempType2);
		for (auto& collider : colliders)
		{
			auto asdasd = collider;
			CComponent* comp = nullptr;
			comp = asdasd->GetComponent("Structure");
			if (comp != nullptr && ((CStructure*)comp)->GetStructureActive() == FALSE)
				continue;
			m_NearbyObjects[type].push_back(collider);
		}
	}
	//Find nearby structures


	if (type == SphereType::eStructure)
	{
		auto cols = m_pCollision->OverlapSphere(position, radius, TriggerType::eWall);
		for (auto& col : cols)
		{
			auto tempStruct = col;
			CStructure* tempPtr = nullptr;
			tempPtr = ((CStructure*)tempStruct->GetComponent("Structure"));
			if (tempPtr != nullptr)
			{
				if (tempPtr->GetStructureActive() == FALSE)
					continue;
			}
			m_NearbyObjects[type].push_back(col);
		}
	}
}

CUnitStats * CEnemyController::GetUnitStats()
{
	return (CUnitStats*)m_Owner->GetComponent("UnitStats");
}

