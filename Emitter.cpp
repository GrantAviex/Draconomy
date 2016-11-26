#include "Emitter.h"
#include "Renderer.h"
#include "Game.h"
#include "LocalUpdateManager.h"
#include <random>
Emitter::Emitter(CGameObject* _Owner)
	: CComponent(_Owner, "Emitter")
{
	particles = nullptr;
	m_bInit = false;
	m_fGravityStop = 0.0f;
	AddSubscription(EventID::SetEmitterActive);
}

Emitter* Emitter::clone() const
{
	Emitter * other = new Emitter(*this);
	other->InitializeEmitter();
	return other;
}
Emitter::~Emitter()
{
#ifndef IARESERVER
	CGame* gameInstance = CGame::GetInstance();
	gameInstance->GetLocalUpdateManager()->RemoveEmitter(this);
	gameInstance->GetRenderer()->RemoveEmitter(this);
	delete[] particles;
#endif
}

void Emitter::InitializeEmitter() // ADD TYPE LATER ON?
{
	m_bInit = true;
	m_bLooping = true;
	if (!particles)
	{
		particles = new TParticle[m_iMaxParticles];
		for (int i = 0; i < m_iMaxParticles; ++i)
			particles[i].m_bActive = false;
	}
	else
	{
		printf("Trying to Initialize particles for second time \n");
	}

}
float Emitter::Lerp(float _start, float _end, float _pct)
{
	return ((_end - _start) * _pct + _start);
}
float Emitter::RandomRange(float _low, float _high)
{
	return (_low + ((float)rand() / (RAND_MAX / (_high - _low))));
}
void Emitter::Emit(float dt)
{
	m_fEmissionCounter += dt;

	while (m_fEmissionCounter > m_fRateofEmission)
	{
		//printf("\n");
		//printf(to_string(m_fEmissionCounter).c_str());
		//printf(" > ");
		//printf(to_string(m_fRateofEmission).c_str());

		if (m_fEmissionCounter > 1000.0f)
		{
			m_fEmissionCounter = 1000.0f;
		}

		m_fEmissionCounter -= m_fRateofEmission;
		if (m_fRateofEmission <= 0.0001f && m_fRateofEmission >= -0.0001f)
		{
			m_fEmissionCounter = 0;
		}

		if (!m_bLooping && m_iTotalExpelledParticles >= m_iMaxParticles)
		{
			m_bOn = false;
			m_iTotalExpelledParticles = 0;
			m_iCurrentParticleCount = 0;
			for (size_t i = 0; i < (size_t)m_iMaxParticles; i++)
				particles[i].m_bActive = false;
		}
		if ((m_bOn && (m_iCurrentParticleCount < (m_iMaxParticles - 1)) && m_fEmitterTimeAlive > m_fEmitterElaspedTime))
		{
			m_fEmitterElaspedTime += dt;
			m_iCurrentParticleCount += 1;
			m_iTotalExpelledParticles += 1;






			int index = 0;
			for (; index < m_iMaxParticles; index++) // looping through the particle array finding next active particle
			{
				if (particles[index].m_bActive == false)
					break;
			}
			if (index >= m_iMaxParticles)
			{
				return;
			}
			//POSITION OFFSETS DONE HERE

			XMMATRIX mat = m_Owner->GetWorldMatrix();
			XMFLOAT4X4 bloop;
			XMStoreFloat4x4(&bloop, mat);
			particles[index].m_vPos.x = bloop._41;
			particles[index].m_vPos.y = bloop._42;
			particles[index].m_vPos.z = bloop._43;

			XMVECTOR tempRotZ;
			XMVECTOR tempRotY;
			XMVECTOR tempRotX;
			CGameObject* muhParent = m_Owner->GetParent();
			if (muhParent != nullptr)
			{
				tempRotZ = -XMLoadFloat3(&muhParent->GetAxis(2));
				tempRotY = XMLoadFloat3(&muhParent->GetAxis(1));
				tempRotX = XMLoadFloat3(&muhParent->GetAxis(0));
			}
			else
			{
				tempRotZ = XMLoadFloat3(&m_Owner->GetAxis(2));
				tempRotY = XMLoadFloat3(&m_Owner->GetAxis(1));
				tempRotX = XMLoadFloat3(&m_Owner->GetAxis(0));
			}
			XMVECTOR veLocity = XMVectorZero();

			float x = (m_vEffectVelocity.x + RandomRange(m_fVarianceVelocityLow.x, m_fVarianceVelocityHigh.x));
			float y = (m_vEffectVelocity.y + RandomRange(m_fVarianceVelocityLow.y, m_fVarianceVelocityHigh.y));
			float z = (m_vEffectVelocity.z + RandomRange(m_fVarianceVelocityLow.z, m_fVarianceVelocityHigh.z));

			veLocity += (tempRotX * x);
			veLocity += (tempRotY * y);
			veLocity += (tempRotZ * z);




			XMStoreFloat3(&particles[index].m_vVel, veLocity);
			//WHY DOES THIS NOT CRASH?
			/*	float difference = (m_fLifeVarHigh - m_fLifeVarLow);
				float doesntMatter = (RAND_MAX / difference);
				float randNum = ((float)rand() / doesntMatter);*/

			particles[index].m_fLife = 0;
			particles[index].m_fMaxLife = m_fParticleLife + RandomRange(m_fLifeVarLow, m_fLifeVarHigh);
			particles[index].m_fScale = m_fStartScale + RandomRange(m_fScaleVarLow, m_fScaleVarHigh);
			particles[index].m_fRotation = m_fParticleRotation + RandomRange(m_fRotationVarLow, m_fRotationVarHigh);
			particles[index].m_bActive = true;
			particles[index].m_vColor = m_vStartColor;

		}

	}
}
void Emitter::TerminateParticles()
{
	for (size_t i = 0; i < (size_t)m_iMaxParticles; i++)
	{
		if (particles[i].m_bActive && particles[i].m_fLife > particles[i].m_fMaxLife) // if particle is active and dead
		{
			particles[i].m_bActive = false;
			m_iCurrentParticleCount -= 1;
			for (size_t j = i; j < (size_t)m_iMaxParticles - 1; j++)
			{
				particles[j].m_bActive = particles[j + 1].m_bActive;
				particles[j].m_vColor = particles[j + 1].m_vColor;
				particles[j].m_fLife = particles[j + 1].m_fLife;
				particles[j].m_fMaxLife = particles[j + 1].m_fMaxLife;
				particles[j].m_vPos = particles[j + 1].m_vPos;
				particles[j].m_vVel = particles[j + 1].m_vVel;
				particles[j].m_fRotation = particles[j + 1].m_fRotation;
				particles[j].m_fScale = particles[j + 1].m_fScale;
			}

		}
	}
}

void Emitter::Update(float dt)
{
	if (m_Owner->GetActive())
	{
		//FIND WHAT THE EMITTER IS ATTACHED TO AND IF WE SHOULD HAVE IT ACTIVATED OR NOT
		//Loop through current particle count, Update position of particles based on Velocity
		if (m_bOn)
		{
			Emit(dt);
			for (size_t i = 0; i < (size_t)m_iCurrentParticleCount; i++)
			{


				XMVECTOR tempRotZ = XMLoadFloat3(&m_Owner->GetAxis(2));
				XMVECTOR tempRotY = XMLoadFloat3(&m_Owner->GetAxis(1));
				XMVECTOR tempRotX = XMLoadFloat3(&m_Owner->GetAxis(0));

				XMVECTOR veLocity = XMVectorZero();
				veLocity += (tempRotX * particles[i].m_vVel.x);
				veLocity += (tempRotY * particles[i].m_vVel.y);
				veLocity += (tempRotZ * particles[i].m_vVel.z);
				veLocity *= dt;
				XMVECTOR tempPos = XMLoadFloat3(&particles[i].m_vPos);
				tempPos += veLocity;
				//	veLocity.m128_f32[2] = 5000;
				XMStoreFloat3(&particles[i].m_vPos, tempPos);

				particles[i].m_fLife += dt;
				float lifePct = particles[i].m_fLife / particles[i].m_fMaxLife;

				particles[i].m_vColor.x = Lerp(m_vStartColor.x, m_vEndColor.x, lifePct);
				particles[i].m_vColor.y = Lerp(m_vStartColor.y, m_vEndColor.y, lifePct);
				particles[i].m_vColor.z = Lerp(m_vStartColor.z, m_vEndColor.z, lifePct);
				particles[i].m_vColor.w = Lerp(m_vStartColor.w, m_vEndColor.w, lifePct);
				particles[i].m_fScale = Lerp(m_fStartScale, m_fEndScale, lifePct);
				if (m_bGravityOn)
				{
					if (m_fGravityStop > 0)
					{
						if (particles[i].m_vPos.y > m_fGravityStop)
						{
							particles[i].m_vVel.y -= 1490.81f*dt;
						}
						else
						{
							particles[i].m_vVel.y = 0.0f;
						}
					}
					else
					{
						particles[i].m_vVel.y -= 1490.81f*dt;
					}
				}
			
			}
			TerminateParticles();
			if (!m_bLooping && m_iTotalExpelledParticles >= m_iMaxParticles)
			{
				m_bOn = false;
				m_iTotalExpelledParticles = 0;
				m_iCurrentParticleCount = 0;
				for (size_t i = 0; i < (size_t)m_iMaxParticles; i++)
					particles[i].m_bActive = false;
			}
		}
	}
	else
	{
		m_bOn = false;
		m_iTotalExpelledParticles = 0;
		m_iCurrentParticleCount = 0;
		for (size_t i = 0; i < (size_t)m_iMaxParticles; i++)
			particles[i].m_bActive = false;
	}
}
void Emitter::Render()
{

}
void Emitter::ReceiveEvent(EventID id, TEventData const* data)
{
	if (m_Owner->GetParent() == nullptr)
		return;
	if (data->i == m_Owner->GetParent()->GetID())
	{

		if (strcmp(data->s.c_str(), "TeslaShooting") == 0 && strcmp(m_Owner->GetName().c_str(), "TeslaParticles") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			m_bLooping = false;
		}
		else if (strcmp(data->s.c_str(), "EmitterShooting") == 0 && strcmp(m_Owner->GetName().c_str(), "ShootParticles") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			//	m_vEffectVelocity.x = data->m[0] / 4.0f;
			//	m_vEffectVelocity.y = data->m[1] / 4.0f;
			//	m_vEffectVelocity.z = data->m[2] / 4.0f;
			m_bLooping = false;
		}
		else if (strcmp(data->s.c_str(), "EmitterBuilding") == 0 && strcmp(m_Owner->GetName().c_str(), "BuildParticles") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			m_bLooping = false;
		}
		else if (strcmp(data->s.c_str(), "EmitterEnemyHit") == 0 && strcmp(m_Owner->GetName().c_str(), "BloodParticles") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			m_bLooping = false;
		}

		if (strcmp(data->s.c_str(), "CandyReturn") == 0 && strcmp(m_Owner->GetName().c_str(), "CandyReturn") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			m_bLooping = false;
		}
		else if (strcmp(data->s.c_str(), "CandyTaken") == 0 && strcmp(m_Owner->GetName().c_str(), "CandyTaken") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			m_bLooping = false;
		}
		else if (strcmp(data->s.c_str(), "CandyPickup") == 0 && strcmp(m_Owner->GetName().c_str(), "CandyPickup") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			m_bLooping = false;
		}
		else if (strcmp(data->s.c_str(), "EmitterSpawning") == 0 && strcmp(m_Owner->GetName().c_str(), "SpawnParticles") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			m_bLooping = true;
		}
		else if (strcmp(data->s.c_str(), "EmitterStopSpawning") == 0 && strcmp(m_Owner->GetName().c_str(), "SpawnParticles") == 0)
		{
			m_bOn = false;
			m_iTotalExpelledParticles = 0;
			m_iCurrentParticleCount = 0;
			for (size_t i = 0; i < (size_t)m_iMaxParticles; i++)
				particles[i].m_bActive = false;
		}
		else if (strcmp(data->s.c_str(), "EmitterPoisonTrap") == 0 && strcmp(m_Owner->GetName().c_str(), "PoisonParticles") == 0)
		{
			m_bOn = true;
			m_fEmitterElaspedTime = 0;
			m_bLooping = false;
		}
	}


}
void Emitter::OnEnable()
{
	//	m_bOn = true;
	//	m_fEmitterElaspedTime = 0;
	//	m_bLooping = false;
#ifndef IARESERVER
	m_bOn = m_bStartState;
	m_fEmitterElaspedTime = 0;
	CGame* gameInstance = CGame::GetInstance();
	gameInstance->GetLocalUpdateManager()->AddEmitter(this);
	gameInstance->GetRenderer()->AddEmitter(this);
#endif
}

void Emitter::OnDisable()
{
#ifndef IARESERVER
	m_bOn = false;
	m_iTotalExpelledParticles = 0;
	m_iCurrentParticleCount = 0;
	if (particles)
	{
		for (size_t i = 0; i < (size_t)m_iMaxParticles; i++)
			particles[i].m_bActive = false;
	}
	CGame* gameInstance = CGame::GetInstance();
	gameInstance->GetLocalUpdateManager()->RemoveEmitter(this);
	gameInstance->GetRenderer()->RemoveEmitter(this);
#endif
}
