#pragma once
#include <DirectXMath.h>
using namespace DirectX;

#include <string>
#include <d3d11.h>
#include "Component.h"
#include "GameObject.h"
struct TParticle
{
	XMFLOAT3 m_vPos;
	XMFLOAT3 m_vVel;
	float m_fLife;
	float m_fMaxLife;
	float m_fRotation;
	float m_fScale;
	bool m_bActive;
	XMFLOAT4 m_vColor;
};
class Emitter : public CComponent
{
public:
	Emitter(CGameObject * _Owner);
	~Emitter();
	virtual Emitter* clone()const;
	void ReceiveEvent(EventID id, TEventData const* data) override;

	void SetEmissionRate(float rate) { m_fRateofEmission = rate; }
	void SetParticleLife(float life) { m_fParticleLife = life; }
	void SetMaxParticleCount(int count) { m_iMaxParticles = count; }
	void SetOn(bool active) { m_bOn = active; }
	void SetLooping(bool loop) { m_bLooping = loop; }
	void SetEmitterTime(float time) { m_fEmitterTimeAlive = time; }
// 	ID3D11ShaderResourceView * GetParticleTexture() { return m_pTexture; }
// 	void SetParticleTexture(ID3D11ShaderResourceView * texture) { m_pTexture = texture; }
	bool GetOn() { return m_bOn; }
	bool IsLooping() { return m_bLooping; }
	int GetMaxParticles() { return m_iMaxParticles; }
	int GetCurrentParticleCount() { return m_iCurrentParticleCount; }
	int GetTotalExpelledParts(){ return m_iTotalExpelledParticles; }
	float GetParticleLife() { return m_fParticleLife; }
	float GetEmissionRate() { return m_fRateofEmission; } 
	float GetEmitterTime() { return m_fEmitterTimeAlive; }
	void SetEffectVelocity(XMFLOAT3 vel) { m_vEffectVelocity = vel; }
	void SetTextureName(std::string name) { m_sTextureName = name; }

	void SetStartColor(XMFLOAT4 color) { m_vStartColor = color; }
	void SetEndColor(XMFLOAT4 color) { m_vEndColor = color; }
	string GetTextureName() { return m_sTextureName; }
	void SetVelVarLow(XMFLOAT3 velvarlow) { m_fVarianceVelocityLow = velvarlow; }
	void SetVelVarHigh(XMFLOAT3 velvarHigh) { m_fVarianceVelocityHigh = velvarHigh; }
	void SetStartScale(float scale) { m_fStartScale = scale; }
	void SetEndScale(float scale) { m_fEndScale = scale; }
	void SetScaleVarLow(float var) { m_fScaleVarLow = var; }
	void SetScaleVarHigh(float var) { m_fScaleVarHigh = var; }
	void SetRotateVarLow(float var) { m_fRotationVarLow = var; }
	void SetRotateVarHigh(float var) { m_fRotationVarHigh = var; }
	void SetParticleRotation(float rot) { m_fParticleRotation = rot; }
	void SetParticleTransparency(float alpha) { m_fParticleTransparency = alpha; }
	void SetGravityStop(float y) { m_fGravityStop = y; }
	void SetTransVarLow(float var) { m_fTransVarLow = var; }
	void SetTransVarHigh(float var) { m_fTransVarHigh = var; }
	void SetGravityStatus(bool on){ m_bGravityOn = on; }
	void SetLifeVarLow(float low) { m_fLifeVarLow = low; }
	void SetLifeVarHigh(float high) { m_fLifeVarHigh = high; }
	void SetStartState(bool state) { m_bStartState = state; }
	void InitializeEmitter();
	void Emit(float dt);
	void TerminateParticles();
	// The particles update locally in the client!
	void Update(float dt) override;
	void Render();

	TParticle* GetParticleArray( void ) { return particles; }
	
	void OnEnable() override;
	void OnDisable() override;
	float Lerp(float _start, float _end, float _pct);
	float RandomRange(float _low, float _high);

protected:
	std::string m_sTextureName;
	XMFLOAT3 m_vEffectVelocity;

	XMFLOAT4 m_vStartColor;
	XMFLOAT4 m_vEndColor;

	float m_fEmitterTimeAlive;
	float m_fEmitterElaspedTime;
	float m_fRateofEmission;
	float m_fEmissionCounter;
	float m_fParticleLife;
	float m_fLifeVarLow;
	float m_fLifeVarHigh;
	float m_fStartScale, m_fEndScale;
	XMFLOAT3 m_fVarianceVelocityLow;
	XMFLOAT3 m_fVarianceVelocityHigh;
	float m_fRotationVarLow;
	float m_fRotationVarHigh;
	float m_fScaleVarLow;
	float m_fScaleVarHigh;
	float m_fParticleTransparency;
	float m_fParticleRotation;
	float m_fTransVarLow;
	float m_fTransVarHigh;
	bool m_bLooping;
	bool m_bOn = false;
	bool m_bStartState = false;
	bool m_bInit;
	bool m_bGravityOn;
	float m_fGravityStop;
	int m_iMaxParticles;
	int m_iCurrentParticleCount = 0;
	int m_iTotalExpelledParticles = 0;
	TParticle * particles;
	// ID3D11ShaderResourceView * m_pTexture;
};

