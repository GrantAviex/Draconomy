
//BulletController.h
//Created May 2015

#ifndef BULLETCONTROLLER_H
#define BULLETCONTROLLER_H

#include <string>
#include "Component.h"
#include "EventSystem.h"
using namespace std;
#include <DirectXMath.h>
using namespace DirectX;

class CBulletController : public CComponent
{
private:
	float fLiveTimer;
	CGameObject* myShooter;
	bool m_bHoming = false;
	CGameObject* myTarget = nullptr;
	int m_nDamage;
	XMFLOAT3 dir;
	bool setDir = true;
	float m_fSpin;
	float m_fDisplacement;
	float m_fFrequency;
	int m_nMaxBounces = 0;
	int m_nBouncesLeft = 0;
	string m_sHitEffect;

	float m_fSlow = 0.0f;

	int m_nExploding = 0;
	bool m_bBouncing = false;
	bool m_bBurning = false;

	float dmgMod = 1.0f;
	float amrMod = 1.0f;

	float m_fDamageMult = 1.0f;
	float gLife = 0.0f;
	XMFLOAT3 vel;
	bool gravity;
	bool m_bVortex = false;
	bool m_bBlazing = false;
	bool m_bDecay = false;
	bool m_bPiercing = false;

	int m_InitialDamage;
public:

	void SetPiercing(bool _pierce){ m_bPiercing = _pierce; }
	bool GetPiercing(){ return m_bPiercing; }

	void SetDamageMult(float _mult){ m_fDamageMult = _mult; }
	float GetDamageMult(){return m_fDamageMult; }
	void SetExploding(int _explode){ m_nExploding = _explode; }
	int GetExploding(){ return m_nExploding; }

	void SetBouncing(bool _bounce){ m_bBouncing = _bounce; }
	bool GetBouncing(){ return m_bBouncing; }

	void SetBurning(bool _burn){ m_bBurning = _burn; }
	bool GetBurning(){ return m_bBurning; }

	void SetBlazing(bool _blaze){ m_bBlazing = _blaze; }
	bool GetBlazing(){ return m_bBlazing; }

	void SetSlow(float _slow){ m_fSlow = _slow; }
	float GetSlow(){ return m_fSlow;}

	void SetWknDmg(float _curse){ dmgMod = _curse; }
	void SetWknAMR(float _curse){ amrMod = _curse; }
	void SetGravity(bool _gravity, float _gLife) { gravity = _gravity; gLife = _gLife; }
 	float GetWknDmg(){ return dmgMod; }
	float GetWknAmr(){ return amrMod; }

	bool GetDecay(){ return m_bDecay; }
	void SetDecay(bool _bool){ m_bDecay = _bool; }
	void SetVortexBool(bool _bool){ m_bVortex = _bool; }
	bool GetVortexBool(){ return m_bVortex; }
	vector<CGameObject*> vcEnemiesHit;

	int GetDamage(){ return m_nDamage; }
	int GetInitialDamage() { return m_InitialDamage; }
	void SetDamage(int _dmg){ m_nDamage = _dmg; }
	void SetLifeTime(float _life){ fLiveTimer = _life; }
	bool GetHomingBool(){ return m_bHoming; }
	void SetHomingBool(bool _homing){ m_bHoming = _homing; }
	CBulletController(CGameObject* owner);
	~CBulletController();
	virtual CBulletController* clone() const;
	void Update(float _dt) override;
	CGameObject* GetWhoShotMe(){ return myShooter; }
	void SetWhoShotMe(CGameObject* _shooter){ myShooter = _shooter; }


	void SetTarget(CGameObject* _targ) { myTarget = _targ; }
	void SetBounce(){ m_nBouncesLeft = m_nMaxBounces; }
	void SetMaxBounce(int _bounce){ m_nMaxBounces = _bounce; }
	void OnEnable() override;
	void Bounce();
	void CreateHitEffect();
	bool CanHit(CGameObject* _col);
	void OnDisable() override;
	void KilledEnemy();
	float m_MoveSpeed;
	void SetHitEffect(string hitEffect) { m_sHitEffect = hitEffect; }
};

#endif
