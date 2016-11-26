
//BossSpawner.h
//Created August 2015

#ifndef BOSSSPAWNER_H
#define BOSSSPAWNER_H

#include "Component.h"
#include "EventSystem.h"
#include "ObjectManager.h"
#include "ManagerplayState.h"
#include <string>
using namespace std;
#include <DirectXMath.h>

enum BossSpawnerLocationType { NoSpawnLocation, SpawnerIndex, RandomSpawner, RandomPlayer, GoldPile };

class CBossSpawner : public CComponent
{
public:
	CBossSpawner(CGameObject* owner);
	~CBossSpawner() {}
	CBossSpawner* clone() const { return new CBossSpawner(*this); };
	void Update(float _dt) override;
	void OnDisable() override;
	void OnEnable() override;
	void ReceiveEvent(EventID id, TEventData const* data) override;
	void SetSpawner(string prefab, string message, unsigned int amount,	BossSpawnerLocationType location, 
		int spawnerIndex, float initialDelay, float spawnPeriod);
	void SetHealthMultiplier(float healthMultiplier) { m_fHealthMultiplier = healthMultiplier; }
	void SetLocation(BossSpawnerLocationType location) { m_eLocation = location; }
	void SetSpawnerIndex(unsigned int index) { m_iSpawnerIndex = index; }
	void SetInitalDelay(float delay) { m_fSpawnInitialDelay = delay; }
	void SetAmount(unsigned int amount) { m_iAmountTotal = amount; }
	void SetSpawnPeriod(float period) { m_fSpawnPeriod = period; }
	void SetPrefab(string prefab) { m_sPrefabName = prefab; }
	void SetMessage(string message) { m_sMessage = message; }
	void SetActive(bool active) { m_bIsActive = active; }
	string GetPrefabName() { return m_sPrefabName; }

	int GetAmount() { return m_iAmountTotal; }

private:
	bool m_bIsActive;
	string m_sPrefabName;
	unsigned int m_iAmountTotal;
	unsigned int m_iAmountSpawned;
	BossSpawnerLocationType m_eLocation;
	unsigned int m_iSpawnerIndex;
	float m_fSpawnInitialDelay;
	float m_fSpawnPeriod;
	float m_fSpawnTimer;
	float m_fHealthMultiplier;
	bool m_bIsActionPhase;
	string m_sMessage;
	CEventSystem* m_pEventSystem;
	CObjectManager* m_pObjectManager;
	CManagerplayState* m_pManagerplayState;
	void SpawnEnemy();
	void ShowMessage();
};
#endif
