#ifndef EVENTS_H
#define EVENTS_H
#include <string>

enum EventID
{
	RotateIconHUD,
	BossIncoming,
	ChangeTextureColor,
	CreateExplosion,
	GoldCheat,
	SetStatusEffect,
	GemBought,
	SocketGem,
	SetTowerPriceOnHUD,
	GetTowerPriceOnHUD,
	PlaySoundOnOneClient,
	InitialTowerPlacement,
	RotationTowerPlacement,
	GamePlayStateShutdown,
	SetEmitterActive,
	Shooting,
	SelectStruct,
	CancelSelection,
	CursorVisibility,
	DisplayCBTText, //CBT == Can't_Build_Tower
	SellStructure,
	Special,
	SpecialRelease,
	Jump,
	Move,
	Rotate,
	RotateCamera,
	Build,
	Keypress,
	EnemyKilled,
	Spawn,
	SpawnReset,
	AllWaveEnemiesKilled,
	BuildPhaseStart,
	ActionPhaseStart,
	CandyTaken,
	EnemyEscaped,
	TowerCreated,
	SkipBuildPhase,
	PlaySoundOnClient,
	CandyReturned,
	Win,
	Lose,
	MouseWheel,
	Mouse,
	MouseUp,
	UpdateEnemiesRemaning,
	UpdateHealth,
	UpdateHUDBuildIcon,
	CandyPickUp,
	UpdateMana,
	UpdateGem,
	UpdateCandy,
	UpdateCandyOnPlayer,
	UpdateCurrency,
	UpdateCurrentWave,
	DisplayHUDText,
	FireActive,
	DrawDebugSphere,
	DrawDebugLine,
	ChargingSpecial,
	CrosshairTexture,
	Revived,
	Dead,
	MostMana,
	MostKills,
	MostTowers,
	MostCandyReturned,
	MostDeaths,
	MostAcurrate,
	UpdateTowerCap,
	UpdateBuildPhaseTimer,
	PlayerDead,
	UnlockedTower,
	HostDisconnected,
	SpecialWaveHUD,
	UpdateBillboardHealth,
	SetGemInBillboard,
	SetBillborededText,
	SetBillborededTextScale,
	SetBillborededTextOffset,
	SetBillborededTextColor,
	SetLocalScale,
	SetWorldScale,
	CreateWaveInfoBillboard,
	ClearWaveInfoBillboards,
	UpdateWaveInfoBillboard,
	GameStarted,
	ShowEnemyPath,
	UpdateMoveSpeed,
	UpdateMaxHealth,
	UpdateAttackMult,
	UpdateChargeTime,
	UpdateDragonSouls,
	UpdateCompanionHUD,
	SetUpWaveController,
	SetBillboardVisibility,
	UpdatePointLight,
	CameraShake,
	DropMana
};

struct TEventData
{
	std::string s;
	int i;
	float m[16];
};

#endif
