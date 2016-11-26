
//EnemyController.h
//Created May 2015

#ifndef ENEMYCONTROLLER_H
#define ENEMYCONTROLLER_H

#include "Component.h"
#include <vector>
#include <unordered_map>
#include "SharedDefinitions.h"
#include "CollisionManager.h"
#include "VectorField.h"
#include "UnitStats.h"
using namespace std;

class CCollisionManager;
class CUnitStats;
enum EnemyAction { Moving, AttackingStructure, AttackingPlayer };

class CEnemyController : public CComponent
{
public:
	CEnemyController(CGameObject* owner);
	~CEnemyController();
	CEnemyController* clone() const { return new CEnemyController(*this); };
	void Update(float REMOVE_THIS_PARAMETER) override;

	void LookAt(XMVECTOR pos);
	void AddPathPoint(XMFLOAT3 point) { /*m_vPath.push_back(point);*/ }
	void OnDisable() override;
	void OnEnable() override;
	float m_Acceleration;
	float m_MaxMoveSpeed;
	float m_MoveSpeedMult;
	float m_TurnSpeed;
	float m_AttackRange;
	float m_AttackDamage;
	float m_AttackBackswing;
	float m_AttackCooldown;
	float m_AttackScanRange;
	float m_WknDmgMult;
	float m_AttackScanRangeForBuildings;
	float m_fSlowMult = 1.0f;
	float m_fSlowTimer = 0.0f;
	float m_fSlowTick = 0.0f;
	float m_SoftCollisionRange;
	int m_nCandy;
	bool m_bIsAssassin;
	bool m_bShowCarnage;
	bool m_bIsJavThrower;
	bool m_bIsBomber;
	float m_fXplodeTimer = 0.0f;
	float m_ManaValue = 0.0f;
	float  pushbackTimer = 0.0f;
	void Cleave();
	void SetSlowedStatus(float _slowAmt);
	void SetSlowedStatus(float _slowAmt, float _slowTimer);
	XMFLOAT3 ProjectPlacement(int _steps);
	EnemyAction m_CurrentAction;
	PathingGoal m_CurrentDestination;
	void ReceiveEvent(EventID id, TEventData const* data) override;
	CGameObject* bombTarget = nullptr;
	CUnitStats*	GetUnitStats();
	void		SetHyperWave(bool b) { m_bHyperWave = b; }
	void SelfDestruct();
private:
	//vector<XMFLOAT3> m_vPath;
	XMFLOAT3 m_Velocity;
	int currentPath;
	int pathDirection;
	CCollisionManager* m_pCollision;
	unordered_map<int, vector<CGameObject*>> m_NearbyObjects;
	vector<CGameObject*> m_NearbyWalls;
	void FindNearbyObjects(int type, XMFLOAT3 position, float radius);
	void FindNearbyTriggerObjects(XMFLOAT3 position, float radius);
	float m_fAttackTargetScanTimer;
	float m_fAttackTargetScanPeriod;
	float m_fSoftCollisionScanTimer;
	float m_fSoftCollisionScanPeriod;
	float m_fAttackTimer;

	float m_fHostileSpeedupSpeed;
	float m_fHostileSpeedupDuration;
	float m_fHostileSpeedupTimer;

	bool m_bEscaped;
	float m_fDragonSlayerAnimTimer;

	XMFLOAT3 m_f3UnstuckLastPosition;
	int m_iUnstuckCount;
	int m_iUnstuckCountLimit;
	float m_fUnstuckDuration;
	float m_fUnstuckTimer;
	float m_fUnstuckDistanceLimit;

	bool m_bHyperWave;

	bool MoveAlongPath();

};

#endif
