
//BossSpawner.cpp
//Created August 2015

#include "BossSpawner.h"
#include "EnemyController.h"
#include "Manager.h"
#include "EventSystem.h"
#include "Server.h"
#include "PathingManager.h"
#include "UnitStats.h"
#include "WaveController.h"
#include "Wwise_IDs.h"

CBossSpawner::CBossSpawner(CGameObject* _Owner)
	: CComponent(_Owner, "BossSpawner")
{
	AddSubscription(EventID::ActionPhaseStart);
	AddSubscription(EventID::BuildPhaseStart);
	m_pEventSystem = nullptr;
	m_pObjectManager = nullptr;
	m_sPrefabName = "";
	m_sMessage = "";
	m_iAmountTotal = 0;
	m_iAmountSpawned = 0;
	m_eLocation = NoSpawnLocation;
	m_iSpawnerIndex = 0;
	m_fSpawnInitialDelay = 0.0f;
	m_fSpawnPeriod = 0.0f;
	m_fSpawnTimer = 0.0f;
	m_fHealthMultiplier = 1.0f;
	m_bIsActionPhase = false;
	m_bIsActive = false;
}

void CBossSpawner::OnEnable()
{
	m_pEventSystem = m_Owner->GetEventSystem();
	m_pObjectManager = m_Owner->GetObjMan();
	m_pManagerplayState = CManagerplayState::GetInstance();
}

void CBossSpawner::OnDisable()
{
}

void CBossSpawner::SetSpawner(string prefab, string message, unsigned int amount, BossSpawnerLocationType location,
	int spawnerIndex, float initialDelay, float spawnPeriod)
{
	m_sPrefabName = prefab;
	m_sMessage = message;
	m_iAmountTotal = amount;
	m_iAmountSpawned = 0;
	m_eLocation = location;
	m_iSpawnerIndex = spawnerIndex;
	m_fSpawnInitialDelay = initialDelay;
	m_fSpawnPeriod = spawnPeriod;
	m_fSpawnTimer = 0.0f;
	m_fHealthMultiplier = 1.0f;
	m_bIsActionPhase = false;
}

void CBossSpawner::Update(float _dt)
{
	if (m_bIsActive == false ||
		m_bIsActionPhase == false ||
		m_iAmountTotal == m_iAmountSpawned ||
		m_eLocation == NoSpawnLocation)
	{
		return;
	}

	auto m = CManager::GetInstance();
	float dt = m->GetDelta();
	m_fSpawnTimer += dt;
	if (m_fSpawnTimer >= m_fSpawnInitialDelay)
	{
		if (m_fSpawnTimer >= m_fSpawnInitialDelay + m_fSpawnPeriod)
		{
			m_fSpawnTimer -= m_fSpawnPeriod;
			m_iAmountSpawned += 1;
			SpawnEnemy();
			ShowMessage();
		}
	}
}

void CBossSpawner::SpawnEnemy()
{
	XMFLOAT4X4 spawnPosition;
	switch (m_eLocation)
	{
	case SpawnerIndex:
	{
		auto spawners = m_pManagerplayState->GetSpawners();
		if (m_iSpawnerIndex >= 0 && m_iSpawnerIndex < spawners.size())
		{
			XMFLOAT3 position = spawners[m_iSpawnerIndex]->GetPosition();
			XMStoreFloat4x4(&spawnPosition, XMMatrixTranslation(position.x, position.y + 50.0f, position.z));
		}
		else
		{
			printf("Error: Spawner index out of bounds.\n");
			XMStoreFloat4x4(&spawnPosition, XMMatrixTranslation(0.0f, 50.0f, 0.0f));
		}
		break;
	}
	case RandomSpawner:
	{
		auto spawners = m_pManagerplayState->GetSpawners();
		int i = rand() % spawners.size();
		XMFLOAT3 position = spawners[i]->GetPosition();
		XMStoreFloat4x4(&spawnPosition, XMMatrixTranslation(position.x, position.y + 50.0f, position.z));
		break;
	}
	case RandomPlayer:
	{
		int i = rand() % m_pManagerplayState->GetPlayerCount();
		CGameObject* player = m_pManagerplayState->GetPlayer(i);
		XMFLOAT3 position = player->GetPosition();
		position.y = 0.0f;
		XMStoreFloat4x4(&spawnPosition, XMMatrixTranslation(position.x, position.y + 50.0f, position.z));
		break;
	}
	case GoldPile:
	{
		XMStoreFloat4x4(&spawnPosition, XMMatrixTranslation(0.0f, 50.0f, 0.0f));
		break;
	}
	default:
	{
		XMStoreFloat4x4(&spawnPosition, XMMatrixTranslation(0.0f, 50.0f, 0.0f));
		break;
	}
	}

	//Create
	CGameObject* newEnemy = m_pObjectManager->CreateObject(m_sPrefabName, spawnPosition, m_pEventSystem);
	if (newEnemy == nullptr)
	{
		printf("BossSpawner: Error creating enemy.\n");
	}

	if (newEnemy->GetName() == "EnemyVanguard")
	{
		TEventData tdata;
		tdata.i = AK::EVENTS::PLAY_SFX_VG_ANNOUNCE;
		CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, tdata);
	}
	else
	{
		TEventData tdata;
		tdata.i = AK::EVENTS::PLAY_SFX_DS_ANNOUNCE;
		CManager::GetInstance()->GetServer()->SendEventMessage(EventID::PlaySoundOnClient, tdata);
	}

	//Set health
	CUnitStats* stats = (CUnitStats*)newEnemy->GetComponent("UnitStats");
	if (stats != nullptr)
	{
		float initial = stats->GetInitialMaxHealth();
		float maxHealth = initial * m_fHealthMultiplier;
		stats->SetMaxHealth(maxHealth);
		stats->SetCurrentHealth(maxHealth);
	}
}

void CBossSpawner::ShowMessage()
{
	if (m_sMessage != "")
	{
		//...
		TEventData tdata;
		tdata.s = m_sMessage;
		CManager::GetInstance()->GetServer()->SendEventMessage(EventID::BossIncoming, tdata);
	}
}

void CBossSpawner::ReceiveEvent(EventID id, TEventData const* data)
{
	switch (id)
	{
	case EventID::BuildPhaseStart:
	{
		m_bIsActionPhase = false;
		break;
	}
	case EventID::ActionPhaseStart:
	{
		m_bIsActionPhase = true;
		m_iAmountSpawned = 0;
		m_fSpawnTimer = 0.0f;
		break;
	}
	default:
	{
		break;
	}
	}
}
