
//AssetManager.cpp
//Created May 2015

#include "AssetManager.h"
#include "GlobalDef.h"
#include "FBXLoader.h"


#ifndef IARESERVER 
#include "Game.h"
#include "Renderer.h"
#include "EnemyHealthBar.h"
#include "GemSocketBillboard.h"
#include "TextBillboard.h"
#include "OnFireBillboard.h"
#include "UpgradePickupBillboard.h"
#include "AnimatedBillboard.h"
#include "Deferred/LightManager.h"
#include "CarnageController.h"
#endif
#include "Model.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "Animation.h"
#include "Deformer.h"
#include "Interpolator.h"
#include "Emitter.h"
#include <assert.h>
#include "AnimationComponent.h"
#include "PointLightComponent.h"

#ifdef IARESERVER
#include "Activatable.h"
#include "AnimationController.h"
#include "BulletController.h"
#include "VortexController.h"
#include "Structure.h"
#include "CandyContainer.h"
#include "CandyPickup.h"
#include "CapsuleCollider.h"
#include "CoinIndicator.h"
#include "EnemyController.h"
#include "ManaPickup.h"
#include "MeshCollider.h"
#include "PathingAgent.h"
#include "PhaseController.h"
#include "PlayerBuildController.h"
#include "PlayerGunController.h"
#include "PlayerMovementController.h"
#include "Spawner.h"
#include "SphereCollider.h"
#include "StructureTimer.h"
#include "AABBCollider.h"
#include "UnitStats.h"
#include "UpgradePickup.h"
#include "WaveController.h"
#include "SphereTrigger.h"
#include "Turret.h"
#include "Trap.h"
#include "Wall.h"
#include "Thorns.h"
#include "ManaPickup.h"
#include "Explosion.h"
#include "ManaMagnet.h"
#include "VanguardShield.h"
#include "TimedLife.h"
#include "DragonSlayerTimer.h"
#include "Rotating.h"
#include "PathIndicator.h"
#include "BossSpawner.h"
#endif
#include <fstream>
CAssetManager::CAssetManager()
{
	m_NodeTag = 0;
}

CAssetManager::~CAssetManager()
{
	for (auto& prefab : m_maPrefabs)
	{
		if (prefab.second == nullptr)
		{
			continue;
		}

		DeletePrefab(prefab.second);
		prefab.second = nullptr;
	}
	m_maPrefabs.clear();

	for (auto& model : m_maModels)
	{
		bool isUniqueModel = true;
		for (auto& name : m_vDefaultedModels)
		{
			if (name == model.first)
			{
				isUniqueModel = false;
				break;
			}
		}

		if (isUniqueModel)
		{
			delete model.second;
		}
	}
	m_maModels.clear();

#ifndef IARESERVER
	for (auto& anData : m_maAnimData)
	{
		delete anData.second;
	}
	m_maAnimData.clear();
	for (auto& deform : m_maDeforms)
	{
		delete deform.second;
	}
	m_maDeforms.clear();

	for (auto& anim : m_maAnims)
	{
		bool isUniqueModel = true;
		for (auto& name : m_vDefaultedModels)
		{
			if (name == anim.first)
			{
				isUniqueModel = false;
				break;
			}
		}

		if (isUniqueModel)
		{
			for (auto& an : anim.second)
			{
				delete an;
			}
			anim.second.clear();
		}
	}
	m_maAnims.clear();

	for (auto& inter : m_maInterpols)
	{
		delete inter.second;
	}
	m_maInterpols.clear();

	for (auto& modelVerts : m_maModelVerts)
	{
		delete modelVerts.second;
	}
	m_maModelVerts.clear();

#endif
}
void CAssetManager::DeletePrefab(CGameObject* prefab)
{
	for (auto& child : prefab->GetChildren())
	{
		DeletePrefab(child);
	}
	delete prefab;
}

CGameObject* CAssetManager::LoadGameObject(XMLElement* pRoot, CGameObject* nObj)
{
	string obj_name = pRoot->Attribute("name");
	XMLElement* pTransform = pRoot->FirstChildElement("Transform");

	float pX, pY, pZ, rX, rY, rZ, sX, sY, sZ;
	XMLElement* pPos = pTransform->FirstChildElement("Position");
	pX = pPos->FloatAttribute("x");
	pY = pPos->FloatAttribute("y");
	pZ = pPos->FloatAttribute("z");
	XMLElement* pRot = pTransform->FirstChildElement("Rotation");
	rX = pRot->FloatAttribute("x");
	rY = pRot->FloatAttribute("y");
	rZ = pRot->FloatAttribute("z");
	XMLElement* pScl = pTransform->FirstChildElement("Scale");
	sX = pScl->FloatAttribute("x");
	sY = pScl->FloatAttribute("y");
	sZ = pScl->FloatAttribute("z");
	XMMATRIX mat = XMMatrixIdentity();
	nObj->SetMatrix(&mat);

	nObj->Rotate(rX, rY, rZ);
	nObj->Translate(XMFLOAT3(pX, pY, pZ));
	nObj->SetLocalScale(sX, sY, sZ);
	XMLElement* pComponentList = pRoot->FirstChildElement("Components");
	if (pComponentList != nullptr)
	{
		XMLElement* element = pComponentList->FirstChildElement();
		while (element != nullptr)
		{
			CComponent* component = nullptr;
			string name = element->Name();
			//TO DO: Set Stuff for all  components
			if (name == "MeshRenderer")
			{
				component = new CMeshRenderer(nObj);
				XMLElement* pMesh = element->FirstChildElement("Model");
				string name = pMesh->Attribute("name");
				((CMeshRenderer*)component)->SetRenderModelTag(name);
			}
			else if (name == "AnimationComponent")
			{

				component = new CAnimationComponent(nObj);
				XMLElement* pAnim = element->FirstChildElement("Model");
				string name = pAnim->Attribute("name");
				((CAnimationComponent*)component)->SetModelTag(name);
			}
			else if (name == "Camera")
			{
				component = new CCamera(nObj);
			}
			else if (name == "Emitter")
			{
				component = new Emitter(nObj);
				Emitter* emitter = (Emitter*)component;
				emitter->SetEmissionRate(FloatAttribute(element, "RateOfEmission"));
				emitter->SetOn(IntAttribute(element, "EmitterOn") != 0);
				emitter->SetStartState(emitter->GetOn());
				float x = FloatAttribute(element, "EffectVelocityX");
				float y = FloatAttribute(element, "EffectVelocityY");
				float z = FloatAttribute(element, "EffectVelocityZ");
				XMFLOAT3 vel;
				vel.x = x;
				vel.y = y;
				vel.z = z;
				emitter->SetEffectVelocity(vel);
				emitter->SetMaxParticleCount(IntAttribute(element, "MaxParticles"));
				XMFLOAT3 vellow;
				vellow.x = FloatAttribute(element, "VelocityLowVarX");
				vellow.y = FloatAttribute(element, "VelocityLowVarY");
				vellow.z = FloatAttribute(element, "VelocityLowVarZ");
				emitter->SetVelVarLow(vellow);
				XMFLOAT3 velhigh;
				velhigh.x = FloatAttribute(element, "VelocityHighVarX");
				velhigh.y = FloatAttribute(element, "VelocityHighVarY");
				velhigh.z = FloatAttribute(element, "VelocityHighVarZ");
				emitter->SetVelVarHigh(velhigh);

				float scale = 1.0f;
				if (element->FirstChildElement("ParticleScale") != NULL)
				{
					scale = FloatAttribute(element, "ParticleScale");
				}
				emitter->SetStartScale(scale);
				if (element->FirstChildElement("EndScale") != NULL)
				{
					scale = FloatAttribute(element, "EndScale");
				}
				emitter->SetEndScale(scale);

				XMFLOAT4 startColor{ 1.0f, 1.0f, 1.0f, 1.0f };
				if (element->FirstChildElement("StartColorX") != NULL)
				{
					startColor.x = FloatAttribute(element, "StartColorX");
					startColor.y = FloatAttribute(element, "StartColorY");
					startColor.z = FloatAttribute(element, "StartColorZ");
					startColor.w = FloatAttribute(element, "StartColorW");
				}
				emitter->SetStartColor(startColor);

				XMFLOAT4 endColor{ -1, -1, -1, -1 };

				if (element->FirstChildElement("EndColorX") != NULL)
				{
					endColor.x = FloatAttribute(element, "EndColorX");
					endColor.y = FloatAttribute(element, "EndColorY");
					endColor.z = FloatAttribute(element, "EndColorZ");
					endColor.w = FloatAttribute(element, "EndColorW");
				}
				if (endColor.x == -1 && endColor.y == -1 && endColor.z == -1 && endColor.w == -1)
				{
					endColor = startColor;
				}
				emitter->SetEndColor(endColor);

				emitter->SetParticleLife(FloatAttribute(element, "ParticleLife"));
				emitter->SetLifeVarLow(FloatAttribute(element, "LifeVarLow"));
				emitter->SetLifeVarHigh(FloatAttribute(element, "LifeVarHigh"));
				emitter->SetEmitterTime(FloatAttribute(element, "EmitterTime"));
				emitter->SetTextureName(StringAttribute(element, "TextureName"));
				emitter->SetParticleRotation(FloatAttribute(element, "ParticleRotation"));
				emitter->SetRotateVarLow(FloatAttribute(element, "RotateVarLow"));
				emitter->SetRotateVarHigh(FloatAttribute(element, "RotateVarHigh"));
				emitter->SetScaleVarLow(FloatAttribute(element, "ScaleVarLow"));
				emitter->SetScaleVarHigh(FloatAttribute(element, "ScaleVarHigh"));
				emitter->SetGravityStatus(IntAttribute(element, "Gravity") != 0);
				emitter->SetLooping(false);
				if (strcmp(obj_name.c_str(), "CandyParticles") == 0)
					emitter->SetLooping(true);
			}
			else if (name == "Emitter2")
			{
				component = new Emitter(nObj);
				Emitter* emitter = (Emitter*)component;
				emitter->SetEmissionRate(FloatAttribute(element, "RateOfEmission"));
				emitter->SetOn(IntAttribute(element, "EmitterOn") != 0);
				emitter->SetStartState(emitter->GetOn());
				float x = FloatAttribute(element, "EffectVelocityX");
				float y = FloatAttribute(element, "EffectVelocityY");
				float z = FloatAttribute(element, "EffectVelocityZ");
				XMFLOAT3 vel;
				vel.x = x;
				vel.y = y;
				vel.z = z;
				emitter->SetEffectVelocity(vel);
				emitter->SetMaxParticleCount(IntAttribute(element, "MaxParticles"));
				XMFLOAT3 vellow;
				vellow.x = FloatAttribute(element, "VelocityLowVarX");
				vellow.y = FloatAttribute(element, "VelocityLowVarY");
				vellow.z = FloatAttribute(element, "VelocityLowVarZ");
				emitter->SetVelVarLow(vellow);
				XMFLOAT3 velhigh;
				velhigh.x = FloatAttribute(element, "VelocityHighVarX");
				velhigh.y = FloatAttribute(element, "VelocityHighVarY");
				velhigh.z = FloatAttribute(element, "VelocityHighVarZ");
				emitter->SetVelVarHigh(velhigh);

				float scale = 1.0f;
				if (element->FirstChildElement("ParticleScale") != NULL)
				{
					scale = FloatAttribute(element, "ParticleScale");
				}
				emitter->SetStartScale(scale);
				if (element->FirstChildElement("EndScale") != NULL)
				{
					scale = FloatAttribute(element, "EndScale");
				}
				emitter->SetEndScale(scale);

				XMFLOAT4 startColor{ 1.0f, 1.0f, 1.0f, 1.0f };
				if (element->FirstChildElement("StartColorX") != NULL)
				{
					startColor.x = FloatAttribute(element, "StartColorX");
					startColor.y = FloatAttribute(element, "StartColorY");
					startColor.z = FloatAttribute(element, "StartColorZ");
					startColor.w = FloatAttribute(element, "StartColorW");
				}
				emitter->SetStartColor(startColor);

				XMFLOAT4 endColor{ -1, -1, -1, -1 };

				if (element->FirstChildElement("EndColorX") != NULL)
				{
					endColor.x = FloatAttribute(element, "EndColorX");
					endColor.y = FloatAttribute(element, "EndColorY");
					endColor.z = FloatAttribute(element, "EndColorZ");
					endColor.w = FloatAttribute(element, "EndColorW");
				}
				if (endColor.x == -1 && endColor.y == -1 && endColor.z == -1 && endColor.w == -1)
				{
					endColor = startColor;
				}
				emitter->SetEndColor(endColor);

				emitter->SetParticleLife(FloatAttribute(element, "ParticleLife"));
				emitter->SetLifeVarLow(FloatAttribute(element, "LifeVarLow"));
				emitter->SetLifeVarHigh(FloatAttribute(element, "LifeVarHigh"));
				emitter->SetEmitterTime(FloatAttribute(element, "EmitterTime"));
				emitter->SetTextureName(StringAttribute(element, "TextureName"));
				emitter->SetParticleRotation(FloatAttribute(element, "ParticleRotation"));
				emitter->SetRotateVarLow(FloatAttribute(element, "RotateVarLow"));
				emitter->SetRotateVarHigh(FloatAttribute(element, "RotateVarHigh"));
				emitter->SetScaleVarLow(FloatAttribute(element, "ScaleVarLow"));
				emitter->SetScaleVarHigh(FloatAttribute(element, "ScaleVarHigh"));
				emitter->SetGravityStatus(IntAttribute(element, "Gravity") != 0);
				emitter->SetGravityStop(FloatAttribute(element, "GravityStop"));
				emitter->SetLooping(false);
				if (strcmp(obj_name.c_str(), "CandyParticles") == 0)
					emitter->SetLooping(true);
			}
			else if (name == "PointLight")
			{
				component = new CPointLightComponent(nObj);
				XMLElement* pValue = element->FirstChildElement();
				reinterpret_cast<CPointLightComponent*>(component)->SetRadius(pValue->FloatAttribute("value"));

				// Color
				pValue = pValue->NextSiblingElement();
				float R = pValue->FloatAttribute("value");

				pValue = pValue->NextSiblingElement();
				float G = pValue->FloatAttribute("value");

				pValue = pValue->NextSiblingElement();
				float B = pValue->FloatAttribute("value");
				reinterpret_cast<CPointLightComponent*>(component)->SetColor(R, G, B);

				// Intensity
				pValue = pValue->NextSiblingElement();
				reinterpret_cast<CPointLightComponent*>(component)->SetIntensity(pValue->FloatAttribute("value"));
#ifndef IARESERVER
				pValue = pValue->NextSiblingElement();
				float x = pValue->FloatAttribute("value");

				pValue = pValue->NextSiblingElement();
				float y = pValue->FloatAttribute("value");

				pValue = pValue->NextSiblingElement();
				float z = pValue->FloatAttribute("value");

				reinterpret_cast<CPointLightComponent*>(component)->SetRelativePosition(x, y, z);

				CLightManager* lightmanager = CGame::GetInstance()->GetRenderer()->GetLightManager();
				reinterpret_cast<CPointLightComponent*>(component)->SetLightManager(CGame::GetInstance()->GetRenderer()->GetLightManager());
#endif
			}
#ifndef IARESERVER
			else if (name == "EnemyHealthBar")
			{
				component = new CEnemyHealthBar(nObj);
				XMLElement* pValue = element->FirstChildElement();
				((CEnemyHealthBar*)component)->SetOffset(pValue->FloatAttribute("value"));
			}
			else if (name == "UpgradePickupBillboard")
			{
				component = new CUpgradePickupBillboard(nObj);
				XMLElement* pValue = element->FirstChildElement();
				((CUpgradePickupBillboard*)component)->SetOffset(pValue->FloatAttribute("value"));
			}
			else if (name == "GemBillboard")
			{
				component = new CGemSocketBillboard(nObj);
				XMLElement* pValue = element->FirstChildElement();
				((CGemSocketBillboard*)component)->SetOffset(pValue->FloatAttribute("value"));
			}
			else if (name == "StatusBilboard")
			{
				component = new COnFireBillboard(nObj);
				XMLElement* pValue = element->FirstChildElement();
				((COnFireBillboard*)component)->SetOffset(pValue->FloatAttribute("value"));
			}
			
			else if (name == "TextBillboard")
			{
				component = new CTextBillboard(nObj);
				XMLElement* pValue = element->FirstChildElement();
				((CTextBillboard*)component)->SetOffset(pValue->FloatAttribute("value"));
				pValue = pValue->NextSiblingElement();
				string name;
				name.append(pValue->Attribute("value"));
				
				((CTextBillboard*)component)->SetText(name);

			}
			else if (name == "AnimatedBillboard")
			{
				component = new CAnimatedBillboard(nObj);
				CAnimatedBillboard* billboard = reinterpret_cast<CAnimatedBillboard*>(component);
				XMLElement* pValue = element->FirstChildElement();

				// Name

				std::string name = pValue->Attribute("value");

				std::wstring wname;
				wname.assign(name.begin(), name.end());
				billboard->SetFileName(wname);

				// Width
				pValue = pValue->NextSiblingElement();
				billboard->SetWidth(pValue->IntAttribute("value"));

				// Height
				pValue = pValue->NextSiblingElement();
				billboard->SetHeight(pValue->IntAttribute("value"));

				// Cell Width
				pValue = pValue->NextSiblingElement();
				billboard->SetCellWidth(pValue->IntAttribute("value"));

				// Cell Height
				pValue = pValue->NextSiblingElement();
				billboard->SetCellHeight(pValue->IntAttribute("value"));

				// Cell Count
				pValue = pValue->NextSiblingElement();
				billboard->SetCellCount(pValue->IntAttribute("value"));

				// Set Scale
				pValue = pValue->NextSiblingElement(); 
				float x = pValue->FloatAttribute("value"); 
				pValue = pValue->NextSiblingElement();
				float y = pValue->FloatAttribute("value");
				billboard->SetScale(x,y);

				// Duration
				pValue = pValue->NextSiblingElement();
				float duration = pValue->FloatAttribute("value");
				billboard->SetDuration(duration);

				// Y Offset
				pValue = pValue->NextSiblingElement();
				billboard->SetOffset(pValue->FloatAttribute("value"));

				// Relative location
				pValue = pValue->NextSiblingElement();
				x = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				y = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				float z = pValue->FloatAttribute("value");

				billboard->SetRelativeLocation(x, y, z);


				pValue = pValue->NextSiblingElement();
				float t = pValue->FloatAttribute("value");
				billboard->SetUseWorld(t);
				
				
				pValue = pValue->NextSiblingElement();
 				float r = pValue->FloatAttribute("value");
				billboard->SetRotation(r);

			}
			else if (name == "CarnageController")
			{
				component = new CCarnageController(nObj);
			}
#endif
#ifdef IARESERVER
			else if (name == "PlayerMovementController")
			{
				component = new CPlayerMovementController(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float movespeed = pValue->FloatAttribute("value");
				((CPlayerMovementController*)component)->m_MoveSpeed = movespeed;


				//To do: Load jump power, sprint capacity, etc.
			}
			else if (name == "Structure")
			{
				component = new CStructure(nObj);
				CStructure* pStruct = ((CStructure*)component);
				float cost;
				cost = FloatAttribute(element, "Cost");
				float resourceCost;
				resourceCost = FloatAttribute(element, "ResourceCost");

				pStruct->SetCost((int)cost);
				pStruct->SetResourceCost((int)resourceCost);


			}
			else if (name == "ManaPickup")
			{
				component = new CManaPickup(nObj);
			}
			else if (name == "UpgradePickup")
			{
				component = new CUpgradePickup(nObj);
			}
			else if (name == "ManaMagnet")
			{
				component = new CManaMagnet(nObj);
				auto magnet = (CManaMagnet*)component;
				magnet->SetSpeed(FloatAttribute(element, "Speed"));
				magnet->SetRange(FloatAttribute(element, "Range"));
			}
			else if (name == "VanguardShield")
			{
				component = new CVanguardShield(nObj);
				auto vgs = (CVanguardShield*)component;
				vgs->SetRange(FloatAttribute(element, "Range"));
			}
			else if (name == "PathIndicator")
			{
				component = new CPathIndicator(nObj);
			}
			else if (name == "BossSpawner")
			{
				component = new CBossSpawner(nObj);
				auto bsp = (CBossSpawner*)component;

				string prefab = StringAttribute(element, "Prefab");
				string message = StringAttribute(element, "Message");
				unsigned int amount = (unsigned int)IntAttribute(element, "Amount");
				BossSpawnerLocationType location = NoSpawnLocation;
				string locationString = StringAttribute(element, "Location");
				if (locationString == "SpawnerIndex")
				{
					location = SpawnerIndex;
				}
				else if (locationString == "RandomSpawner")
				{
					location = RandomSpawner;
				}
				else if (locationString == "RandomPlayer")
				{
					location = RandomPlayer;
				}
				else if (locationString == "GoldPile")
				{
					location = GoldPile;
				}
				unsigned int index = (unsigned int)IntAttribute(element, "SpawnerIndex");
				float initialDelay = FloatAttribute(element, "InitialDelay");
				float spawnPeriod = FloatAttribute(element, "SpawnPeriod");

				bsp->SetSpawner(prefab, message, amount, location, index, initialDelay, spawnPeriod);
			}
			else if (name == "Rotating")
			{
				component = new CRotating(nObj);
			}
			else if (name == "TimedLife")
			{
				component = new CTimedLife(nObj);
				auto timedLife = (CTimedLife*)component;
				timedLife->SetTimeLimit(FloatAttribute(element, "TimeLimit"));
			}
			else if (name == "DragonSlayerTimer")
			{
				component = new CDragonSlayerTimer(nObj);
				auto timedLife = (CDragonSlayerTimer*)component;
				timedLife->SetTimeLimit(FloatAttribute(element, "TimeLimit"));
				timedLife->SetTarget(IntAttribute(element, "Target"));
			}
			else if (name == "StructureTimer")
			{
				component = new CStructureTimer(nObj);
				auto timedLife = (CStructureTimer*)component;
				timedLife->SetTimeLimit(FloatAttribute(element, "TimeLimit"));
				timedLife->SetSpawnTime(FloatAttribute(element, "SpawnTime"));
				timedLife->SetTarget(IntAttribute(element, "Target"));
			}
			else if (name == "PlayerBuildController")
			{
				component = new CPlayerBuildController(nObj);
			}
			else if (name == "Trap")
			{
				component = new CTrap(nObj);
				TTrapStats stats;
				XMLElement* pValue = element->FirstChildElement();
				float radius = pValue->FloatAttribute("value");
				stats.fRadius = radius;

				pValue = pValue->NextSiblingElement();
				float cd = pValue->FloatAttribute("value");
				stats.fCoolDown = cd;

				pValue = pValue->NextSiblingElement();
				float expelTime = pValue->FloatAttribute("value");
				stats.fExpelTime = expelTime;

				pValue = pValue->NextSiblingElement();
				int dmg = pValue->IntAttribute("value");
				stats.nDamage = dmg;

				pValue = pValue->NextSiblingElement();
				int charges = pValue->IntAttribute("value");
				stats.nMaxCharge = charges;
				stats.nCharge = stats.nMaxCharge;

				pValue = pValue->NextSiblingElement();
				int cost = pValue->IntAttribute("value");
				stats.nCost = cost;

				((CTrap*)component)->SetStats(stats);
			}
			else if (name == "Wall")
			{
				component = new CWall(nObj);
				TWallStats tempStats;
				XMLElement* pValue = element->FirstChildElement();
				float hp = pValue->FloatAttribute("value");
				tempStats.fHP = hp;
				int cost = IntAttribute(element, "Cost");
				tempStats.nCost = cost;

				((CWall*)component)->SetStats(tempStats);
			}
			else if (name == "Explosion")
			{
				component = new CExplosion(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float radius = pValue->FloatAttribute("value");
				((CExplosion*)component)->SetExplosionRadius(radius);

			}
			else if (name == "Thorns")
			{
				component = new CThorns(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float damage = pValue->FloatAttribute("value");
				((CThorns*)component)->SetDamage((int)damage);

			}
			else if (name == "Turret")
			{
				component = new CTurret(nObj);
				TTurretStats tTempStats;
				XMLElement* pValue = element->FirstChildElement();
				float range = pValue->FloatAttribute("value");
				tTempStats.fRange = range;

				pValue = pValue->NextSiblingElement();
				float damage = pValue->FloatAttribute("value");
				tTempStats.fDamage = damage;
				pValue = pValue->NextSiblingElement();

				float health = pValue->FloatAttribute("value");
				tTempStats.fHP = health;
				pValue = pValue->NextSiblingElement();

				int cost = pValue->IntAttribute("value");
				tTempStats.nCost = cost;
				pValue = pValue->NextSiblingElement();

				float fireRate = pValue->FloatAttribute("value");
				tTempStats.fFireRate = fireRate;
				pValue = pValue->NextSiblingElement();

				float angle = pValue->FloatAttribute("value");
				tTempStats.fAngle = angle;
				pValue = pValue->NextSiblingElement();

				float bulletSpeed = pValue->FloatAttribute("value");
				tTempStats.fBulletSpeed = bulletSpeed;
				pValue = pValue->NextSiblingElement();
				((CTurret*)component)->SetStats(tTempStats);


				//		std::string bulletName = pValue->Attribute("name");
				//		((CTurret*)component)->bulletName = bulletName;

			}
			else if (name == "PlayerGunController")
			{
				component = new CPlayerGunController(nObj);
				CPlayerGunController* pGunControl = ((CPlayerGunController*)component);
				XMLElement* pValue = element->FirstChildElement();
				float fireRate = pValue->FloatAttribute("value");
				pGunControl->m_FireRate = fireRate;
				pValue = pValue->NextSiblingElement();

				float chargeDuration = pValue->FloatAttribute("value");
				pGunControl->m_ChargeDuration = chargeDuration;
				pValue = element->FirstChildElement("BulletPrefab");

				pGunControl->m_ProjectilePrefab = StringAttribute(element, "BulletPrefab");
				pGunControl->m_SpecialPrefab = StringAttribute(element, "SpecialPrefab");


				pValue = element->FirstChildElement("GunType");
				int gunType = pValue->IntAttribute("value");

				pGunControl->nWizardType = gunType;

				pGunControl->m_FireTimer = 0;
				pGunControl->m_AttackRange = 0;

			}
			else if (name == "MeshCollider")
			{
				component = new CMeshCollider(nObj);
			}
			else if (name == "CapsuleCollider")
			{
				component = new CCapsuleCollider(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float radius = pValue->FloatAttribute("value");
				((CCapsuleCollider*)component)->m_fRadius = radius;
				pValue = element->FirstChildElement("SphereOffset");
				float offset = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				if (pValue != nullptr)
				{
					int capType = (int)pValue->FloatAttribute("value");
					((CCapsuleCollider*)component)->eCapsuleType = (CapsuleType)capType;
				}
				((CCapsuleCollider*)component)->m_fOffset = offset;
				((CCapsuleCollider*)component)->SetUpSpheres();
			}
			else if (name == "SphereCollider")
			{
				component = new CSphereCollider(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float radius = pValue->FloatAttribute("value");
				((CSphereCollider*)component)->m_Radius = radius;
			}
			else if (name == "AABBCollider")
			{
				component = new CAABBCollider(nObj);
				CAABBCollider* pAABB = ((CAABBCollider*)component);
				//		XMLElement* pTransform = element->FirstChildElement("AABBCollider");

				XMLElement* pMin = element->FirstChildElement("Min");
				float localFloatX = pMin->FloatAttribute("x");
				pAABB->m_min.x = localFloatX;
				pAABB->m_min.y = pMin->FloatAttribute("y");
				pAABB->m_min.z = pMin->FloatAttribute("z");
				XMLElement* pMax = element->FirstChildElement("Max");
				pAABB->m_max.x = pMax->FloatAttribute("x");
				pAABB->m_max.y = pMax->FloatAttribute("y");
				pAABB->m_max.z = pMax->FloatAttribute("z");

				((CAABBCollider*)component)->m_min = pAABB->m_min;
				((CAABBCollider*)component)->m_max = pAABB->m_max;

			}
			else if (name == "SphereTrigger")
			{
				component = new CSphereTrigger(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float radius = pValue->FloatAttribute("value");
				((CSphereTrigger*)component)->m_Radius = radius;
			}
			else if (name == "PathingAgent")
			{
				component = new CPathingAgent(nObj);
			}
			else if (name == "CandyPickup")
			{
				component = new CCandyPickup(nObj);
			}
			else if (name == "AnimationController")
			{
				component = new CAnimationController(nObj);
				XMLElement* pValue = element->FirstChildElement();
				string name = pValue->Attribute("value");
				((CAnimationController*)component)->SetAnimation(name);
			}
			else if (name == "WaveController")
			{
				component = new CWaveController(nObj);
				XMLElement* pValue = element->FirstChildElement();
				int maxWaves = pValue->IntAttribute("value");
				pValue = pValue->NextSiblingElement();
				XMLElement* pEnemy = pValue->FirstChildElement();
				while (pEnemy != nullptr)
				{
					string prefabName = pEnemy->Attribute("prefabName");
					int spawnWave = pEnemy->IntAttribute("SpawnWave");
					float spawnamtMult = pEnemy->FloatAttribute("spawnamtMult");
					int spawnLocation = pEnemy->IntAttribute("SpawnLocation");

					((CWaveController*)component)->m_MaxWaves = maxWaves;
					((CWaveController*)component)->m_vEnemySpawnInfo.push_back(TEnemyInfo(prefabName, spawnWave, spawnamtMult, spawnLocation));
					pEnemy = pEnemy->NextSiblingElement();
				}
			}
			else if (name == "Spawner")
			{
				component = new CSpawner(nObj);
				XMLElement* pValue = element->FirstChildElement();

				while (pValue != nullptr)
				{
					float spawnRate = pValue->FloatAttribute("SpawnRate");
					string prefabName = pValue->Attribute("prefabName");
					int spawnGroupSize = pValue->IntAttribute("SpawnGroupSize");
					float spawnRateBetweenGroup = pValue->FloatAttribute("SpawnRateBetweenGroup");

					((CSpawner*)component)->prefabName = prefabName;
					((CSpawner*)component)->spawnSettings[prefabName].spawnRate = spawnRate;
					((CSpawner*)component)->spawnSettings[prefabName].spawnGroupSize = spawnGroupSize;
					((CSpawner*)component)->spawnSettings[prefabName].spawnRateBetweenGroup = spawnRateBetweenGroup;

					pValue = pValue->NextSiblingElement();
				}
			}
			else if (name == "PhaseController")
			{
				component = new CPhaseController(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float duration = pValue->FloatAttribute("duration");
				((CPhaseController*)component)->SetBuildPhaseDuration(duration);
			}
			else if (name == "CandyContainer")
			{
				component = new CCandyContainer(nObj);
				XMLElement* pValue = element->FirstChildElement();
				int amt = pValue->IntAttribute("value");
				((CCandyContainer*)component)->SetCandy(amt);
			}
			else if (name == "ManaPickup")
			{
				component = new CManaPickup(nObj);
			}
			else if (name == "CoinIndicator")
			{
				component = new CCoinIndicator(nObj);
			}
			else if (name == "UnitStats")
			{
				component = new CUnitStats(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float hp = pValue->FloatAttribute("value");
				((CUnitStats*)component)->SetMaxHealth(hp);
				((CUnitStats*)component)->SetInitialHealth(hp);
				((CUnitStats*)component)->SetCurrentHealth(hp);

			}
			else if (name == "EnemyController")
			{
				component = new CEnemyController(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float moveSpeed = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				float acceleration = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				float TurnSpeed = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				float AttackRange = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				float AttackDamage = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				float AttackBackswing = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				float AttackCooldown = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				bool isAssassin = pValue->BoolAttribute("value");
				pValue = pValue->NextSiblingElement();
				bool isJav = pValue->BoolAttribute("value");
				pValue = pValue->NextSiblingElement();
				bool isBomb = pValue->BoolAttribute("value");
				pValue = pValue->NextSiblingElement();
				float manaValue = pValue->FloatAttribute("value");
				pValue = pValue->NextSiblingElement();
				bool showCarnage = pValue->IntAttribute("value") == 1;

				((CEnemyController*)component)->m_MaxMoveSpeed = moveSpeed;
				((CEnemyController*)component)->m_Acceleration = acceleration;
				((CEnemyController*)component)->m_TurnSpeed = TurnSpeed;
				((CEnemyController*)component)->m_AttackRange = AttackRange;
				((CEnemyController*)component)->m_AttackDamage = AttackDamage;
				((CEnemyController*)component)->m_AttackBackswing = AttackBackswing;
				((CEnemyController*)component)->m_AttackCooldown = AttackCooldown;
				((CEnemyController*)component)->m_bIsAssassin = isAssassin;
				((CEnemyController*)component)->m_bIsJavThrower = isJav;
				((CEnemyController*)component)->m_bIsBomber = isBomb;
				((CEnemyController*)component)->m_ManaValue = manaValue;
				((CEnemyController*)component)->m_bShowCarnage = showCarnage;

			}
			else if (name == "BulletController")
			{
				component = new CBulletController(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float moveSpeed = pValue->FloatAttribute("value");
				((CBulletController*)component)->m_MoveSpeed = moveSpeed;
				int dmg = (int)FloatAttribute(element, "Damage");
				((CBulletController*)component)->SetDamage(dmg);
				pValue = element->FirstChildElement("HitEffect");
				string effect = pValue->Attribute("value");
				((CBulletController*)component)->SetHitEffect(effect);

				pValue = element->FirstChildElement("MaxBounce");
				int bounce = 0;
				bounce = pValue->IntAttribute("value");
				if (bounce < 3)
					bounce = 3;
				((CBulletController*)component)->SetMaxBounce(bounce);
				((CBulletController*)component)->SetBounce();
			}
			else if (name == "Activatable")
			{
				component = new CActivatable(nObj);
				XMLElement* pValue = element->FirstChildElement();
				float cooldown = FloatAttribute(element,"Cooldown");
				int ID = IntAttribute(element,"ID");

				((CActivatable*)component)->SetID(ID);
				((CActivatable*)component)->SetCooldown(cooldown);

			}
			else if (name == "VortexController")
			{
				component = new CVortexController(nObj);
				XMLElement* pValue = element->FirstChildElement();
				int range = 0;
				range = pValue->IntAttribute("value");
				((CVortexController*)component)->SetRange(range);
			}
#endif
			else
			{
			}
			if (component != nullptr)
				nObj->AddComponent(component);
			element = element->NextSiblingElement();
		}
	}
	XMLElement* pChildList = pRoot->FirstChildElement("ChildObjects");
	if (pChildList != nullptr)
	{
		XMLElement* pChild = pChildList->FirstChildElement();
		while (pChild != nullptr)
		{
			CGameObject* cObj = new CGameObject(0, nullptr, nullptr, nullptr, true);
			LoadGameObject(pChild, cObj);
			nObj->AddChild(cObj);
			pChild = pChild->NextSiblingElement();
		}
	}
	nObj->SetName(obj_name);
	return nObj;
}

void CAssetManager::LoadPrefab(string _path)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(_path.c_str()) != XML_NO_ERROR)
		return;

	XMLElement* pRoot = doc.RootElement();

	CGameObject* nObj = new CGameObject(0, NULL, nullptr, nullptr, true);
	string obj_name = pRoot->Attribute("name");
	nObj = LoadGameObject(pRoot, nObj);
	nObj->SetActive(false);
	m_maPrefabs[obj_name] = nObj;
}

void CAssetManager::LoadModel(string _path, unsigned int setTag)
{
#ifndef IARESERVER

	string name;
	if (!GlobalDef::exportingToBinary)
	{
		string temp = _path;

		for (size_t i = 0; i < temp.length(); i++)
		{
			if (temp[i] == '.')
				break;
			name += temp[i];
			if (temp[i] == '/')
				name.clear();
		}    
		CDeformer * deform = new CDeformer;
		CInterpolator * interpol = new CInterpolator;
		LoadMeshFromBinary(name);
		LoadAnimationFromBinary(name);

		auto modelptr = m_maModels[name];

			if (modelptr != nullptr && m_maModels[name]->size() > 0)
		{
			auto& model = m_maModels[name];
			auto& mesh = model->front();
			deform->SetMesh(&mesh);
			interpol->SetAnimations(m_maAnims[name]);
			m_maDeforms[name] = deform;
			m_maInterpols[name] = interpol;
		}
			else
			{
				string defaultName = "Default";
				if (m_bLoadedDefaultModels == false)
				{
					LoadMeshFromBinary(defaultName);
					LoadAnimationFromBinary(defaultName);
					m_bLoadedDefaultModels = true;
				}
				m_maModels[name] = m_maModels[defaultName];
				m_maAnims[name] = m_maAnims[defaultName];
				m_vDefaultedModels.push_back(name);

				deform->SetMesh(&m_maModels[defaultName]->front());
				interpol->SetAnimations(m_maAnims[defaultName]);
				m_maDeforms[defaultName] = deform;
				m_maInterpols[defaultName] = interpol;
			}
	}
	else
	{
		CDeformer * deform = new CDeformer;
		CInterpolator * interpol = new CInterpolator;


		//load fbx since no binary file present
		FBXLoader fbxLoader;
		std::vector<CMesh>* meshes = new vector<CMesh>();
		vector<CAnimation*> anim;
		fbxLoader.Load(_path.c_str(), *meshes, anim, 1);
		string temp = _path;
		for (size_t i = 0; i < temp.length(); i++)
		{
			if (temp[i] == '.')
				break;
			name += temp[i];
			if (temp[i] == '/')
				name.clear();
		}
		m_maModels[name] = meshes;
		deform->SetMesh(&meshes->front());
		interpol->SetAnimations(anim);
		if (anim.size() > 0)
		{
			interpol->SetAnimation(anim[0]);
			m_maAnims[name] = anim;
		}
		m_maDeforms[name]	= deform;
		m_maInterpols[name] = interpol;
	}
	if (m_maAnims[name].size() > 0)
	{
		if (m_maAnims[name][0]->m_vChannels.size() > 0)
		{
			CAnimationComponent* data = new CAnimationComponent(nullptr);
			data->SetModelTag(name);
			data->InitializeAnimationData();
			m_maAnimData[name] = data;
		}
	}
	for (size_t i = 0; i < m_maModels[name]->size(); i++)
	{
		auto& mesh = (*m_maModels[name])[i];
		auto& verts = mesh.GetVerts();
		auto& indices = mesh.GetIndices();

		int TESTAMOUNT = 1000; //Placeholder

		CGame::GetInstance()->GetRenderer()->QueueMesh_RDM(
			0, m_NodeTag, verts.size(), indices.size(), verts.data(), indices.data(), TESTAMOUNT, name);
		m_modelRenders[name] = TRenderInfo(m_NodeTag, setTag);
		m_NodeTag += 1;
	}
#endif
	//nMod->SetMeshes(meshes);
	//m_maModels[name] = nMod;

}
void CAssetManager::LoadServerModel(string _path)
{
	string temp;
	for (size_t i = 2; i < _path.size(); i++)
	{
		temp += _path[i];
	}
	string name;
	for (size_t i = 0; i < temp.length(); i++)
	{
		if (temp[i] == '.')
			break;
		name += temp[i];
		if (temp[i] == '/')
			name.clear();
	}
	//Server load mesh from binary
	std::string fileName = "../SirCuddles/Assets/Models/Binary/" + name + ".bin";
	vector<CMesh>* tempmesh = new vector < CMesh > ;
	std::ifstream ifs(fileName, std::ios::binary | ios::in);

	if (ifs.is_open())
	{
		unsigned int nummeshes = 0;

		ifs.read((char*)&nummeshes, sizeof(unsigned int));

		for (unsigned int i = 0; i < nummeshes; ++i)
		{
			CMesh mesh;
			unsigned int meshNameLength = 0;
			unsigned int vertCount = 0;
			unsigned int indexCount = 0;
			unsigned int influenceCount = 0;
			unsigned int jointCount = 0;
			ifs.read((char*)&meshNameLength, sizeof(unsigned int));
			mesh.GetName().resize(meshNameLength);
			ifs.read((char*)mesh.GetName().c_str(), meshNameLength);

			ifs.read((char*)&vertCount, sizeof(unsigned int));
			mesh.GetVerts().resize(vertCount);
			ifs.read((char*)&(mesh.GetVerts()[0]), vertCount * sizeof(UniqueVertex));

			ifs.read((char*)&indexCount, sizeof(unsigned int));
			mesh.GetIndices().resize(indexCount);
			ifs.read((char*)&(mesh.GetIndices()[0]), indexCount * sizeof(unsigned int));

			ifs.read((char*)&influenceCount, sizeof(unsigned int));
			mesh.GetInfluences().resize(influenceCount);
			for (size_t j = 0; j < influenceCount; j++)
			{
				unsigned int infCount = 0;
				ifs.read((char*)&infCount, sizeof(unsigned int));
				if (infCount > 0)
				{
					mesh.GetInfluences()[i].resize(infCount);
					ifs.read((char*)&(mesh.GetInfluences()[i][0]), infCount * sizeof(CMesh::JointInfluence));
				}
			}

			ifs.read((char*)&jointCount, sizeof(unsigned int));
			if (jointCount > 0)
			{
				mesh.GetJoints().resize(jointCount);

				for (size_t j = 0; j < jointCount; j++)
				{
					CMesh::Joint joint;
					unsigned int childCount;
					ifs.read((char*)&childCount, sizeof(unsigned int));
					joint.children.resize(childCount);
					if (childCount > 0)
						ifs.read((char*)&joint.children[0], childCount * sizeof(unsigned int));

					unsigned int parent;
					ifs.read((char*)&parent, sizeof(unsigned int));
					joint.parent = parent;
					ifs.read((char*)&joint.local_bind_pose_transform, sizeof(XMFLOAT4X4));
					ifs.read((char*)&joint.world_bind_pose_transform, sizeof(XMFLOAT4X4));
					unsigned int nameSize;
					ifs.read((char*)&nameSize, sizeof(unsigned int));
					joint.name.resize(nameSize);
					if (nameSize > 0)
						ifs.read((char*)&joint.name[0], nameSize);
					mesh.GetJoints()[j] = joint;
				}
			}

			tempmesh->push_back(mesh);
		}
		m_maModels[name] = tempmesh;
		ifs.close();
	}
}

void CAssetManager::ExportMeshesToBinary()
{
	unordered_map<string, vector<CMesh>*>::iterator meshitr = m_maModels.begin();
	for (; meshitr != m_maModels.end(); meshitr++)
	{
		string filePath = "Assets/Models/Binary/" + meshitr->first + ".bin";
		ofstream ofs(filePath, ios::binary | ios::out | ios::trunc);
		if (ofs.is_open())
		{
			size_t numofmeshes = meshitr->second->size();
			ofs.write((char*)&numofmeshes, sizeof(size_t));
			for (size_t i = 0; i < numofmeshes; i++)
			{
				vector<CMesh> vec = meshitr->second[i];
				unsigned int meshnamelength = vec[i].GetName().size();;
				unsigned int numverts = vec[i].GetVerts().size();
				unsigned int numindices = vec[i].GetIndices().size();
				unsigned int numInfluences = vec[i].GetInfluences().size();
				unsigned int numJoints = vec[i].GetJoints().size();
				ofs.write((char*)&meshnamelength, sizeof(unsigned int));
				if (meshnamelength > 0)
					ofs.write((char*)vec[i].GetName().c_str(), meshnamelength);
				ofs.write((char*)&numverts, sizeof(unsigned int));
				if (numverts > 0)
					ofs.write((char*)&vec[i].GetVerts()[0], numverts * sizeof(UniqueVertex));
				ofs.write((char*)&numindices, sizeof(unsigned int));
				if (numindices > 0)
					ofs.write((char*)&vec[i].GetIndices()[0], numindices * sizeof(unsigned int));
				if (numInfluences > 0)
				{
					ofs.write((char*)&numInfluences, sizeof(unsigned int));
					for (size_t j = 0; j < numInfluences; j++)
					{
						unsigned int numInfluence = vec[i].GetInfluences()[j].size();
						ofs.write((char*)&numInfluence, sizeof(unsigned int));
						if (numInfluence > 0)
						{
							ofs.write((char*)&vec[i].GetInfluences()[j][0], numInfluence*sizeof(CMesh::JointInfluence));
						}
					}
				}
				ofs.write((char*)&numJoints, sizeof(unsigned int));
				if (numJoints > 0)
				{
					for (size_t j = 0; j < numJoints; j++)
					{
						unsigned int childCount = vec[i].GetJoints()[j].children.size();
						ofs.write((char*)&childCount, sizeof(unsigned int));
						if (childCount > 0)
							ofs.write((char*)&vec[i].GetJoints()[j].children[0], childCount * sizeof(unsigned int));

						ofs.write((char*)&vec[i].GetJoints()[j].parent, sizeof(unsigned int));
						ofs.write((char*)&vec[i].GetJoints()[j].local_bind_pose_transform, sizeof(XMFLOAT4X4));
						ofs.write((char*)&vec[i].GetJoints()[j].world_bind_pose_transform, sizeof(XMFLOAT4X4));
						unsigned int nameSize = vec[i].GetJoints()[j].name.size();
						ofs.write((char*)&nameSize, sizeof(unsigned int));
						if (nameSize > 0)
							ofs.write((char*)&vec[i].GetJoints()[j].name[0], nameSize);
					}
				}
			}
		}
		ofs.close();
	}

}
bool CAssetManager::LoadMeshFromBinary(string MeshName)
{
	std::string fileName = "Assets/Models/Binary/" + MeshName + ".bin";
	vector<CMesh>* tempmesh = new vector < CMesh > ;
	std::ifstream ifs(fileName, std::ios::binary | ios::in);

	if (ifs.is_open())
	{
		unsigned int nummeshes = 0;

		ifs.read((char*)&nummeshes, sizeof(unsigned int));

		for (unsigned int i = 0; i < nummeshes; ++i)
		{
			CMesh mesh;
			unsigned int meshNameLength = 0;
			unsigned int vertCount = 0;
			unsigned int indexCount = 0;
			unsigned int influenceCount = 0;
			unsigned int jointCount = 0;
			ifs.read((char*)&meshNameLength, sizeof(unsigned int));
			mesh.GetName().resize(meshNameLength);
			ifs.read((char*)mesh.GetName().c_str(), meshNameLength);

			ifs.read((char*)&vertCount, sizeof(unsigned int));
			mesh.GetVerts().resize(vertCount);
			ifs.read((char*)&(mesh.GetVerts()[0]), vertCount * sizeof(UniqueVertex));

			ifs.read((char*)&indexCount, sizeof(unsigned int));
			mesh.GetIndices().resize(indexCount);
			ifs.read((char*)&(mesh.GetIndices()[0]), indexCount * sizeof(unsigned int));

			ifs.read((char*)&influenceCount, sizeof(unsigned int));
			mesh.GetInfluences().resize(influenceCount);
			for (size_t j = 0; j < influenceCount; j++)
			{
				unsigned int infCount = 0;
				ifs.read((char*)&infCount, sizeof(unsigned int));
				if (infCount > 0)
				{
					mesh.GetInfluences()[j].resize(infCount);
					ifs.read((char*)&(mesh.GetInfluences()[j][0]), infCount * sizeof(CMesh::JointInfluence));
				}
			}

			ifs.read((char*)&jointCount, sizeof(unsigned int));
			if (jointCount > 0)
			{
				mesh.GetJoints().resize(jointCount);

				for (size_t j = 0; j < jointCount; j++)
				{
					CMesh::Joint joint;
					unsigned int childCount;
					ifs.read((char*)&childCount, sizeof(unsigned int));
					joint.children.resize(childCount);
					if (childCount > 0)
						ifs.read((char*)&joint.children[0], childCount * sizeof(unsigned int));

					unsigned int parent;
					ifs.read((char*)&parent, sizeof(unsigned int));
					joint.parent = parent;
					ifs.read((char*)&joint.local_bind_pose_transform, sizeof(XMFLOAT4X4));
					ifs.read((char*)&joint.world_bind_pose_transform, sizeof(XMFLOAT4X4));
					unsigned int nameSize;
					ifs.read((char*)&nameSize, sizeof(unsigned int));
					joint.name.resize(nameSize);
					if (nameSize > 0)
						ifs.read((char*)&joint.name[0], nameSize);
					mesh.GetJoints()[j] = joint;
				}
			}

			tempmesh->push_back(mesh);
		}
		m_maModels[MeshName] = tempmesh;
		ifs.close();
		return true;
	}
	return false;
}
void CAssetManager::ExportAnimationsToBinary()
{
	for (auto& animList : m_maAnims)
	{
		if (animList.second.size() == 0)
			continue;
		if (animList.second[0]->m_vChannels.size() == 0)
			continue;
		string filePath = "Assets/Animations/" + animList.first + ".bin";
		ofstream ofs(filePath, ios::binary | ios::out | ios::trunc);
		if (ofs.is_open())
		{
			unsigned int listSize = animList.second.size();
			ofs.write((char*)&listSize, sizeof(unsigned int));
			for (auto& anim : animList.second)
			{
				vector<CAnimation::TChannel> vec = anim->m_vChannels;
				string name = anim->GetName();
				unsigned int nameSize = name.size();
				ofs.write((char*)&nameSize, sizeof(unsigned int));
				ofs.write(anim->GetName().c_str(), anim->GetName().size());
				unsigned int size = vec.size();
				ofs.write((char*)&size, sizeof(unsigned int));
				for (size_t i = 0; i < size; i++)
				{
					vector<CAnimation::TKey> keys = vec[i].m_vKeys;
					unsigned int size2 = keys.size();
					ofs.write((char*)&size2, sizeof(unsigned int));
					for (size_t j = 0; j < size2; j++)
					{
						float time = keys[j].m_fTime;
						XMFLOAT4X4 tempmat = keys[j].m_mNode;
						ofs.write((char*)&time, sizeof(unsigned int));
						ofs.write((char*)&tempmat, sizeof(XMFLOAT4X4));
					}
				}
			}
		}
		ofs.close();
	}
}
bool CAssetManager::LoadAnimationFromBinary(string AnimName)
{
	std::string fileName = "Assets/Animations/" + AnimName + ".bin";
	vector<CAnimation*> animations;
	std::ifstream ifs(fileName, std::ios::binary | ios::in);
	if (ifs.is_open())
	{
		unsigned int listSize;
		ifs.read((char*)&listSize, sizeof(unsigned int));
		if (listSize > 0)
		{
			for (size_t i = 0; i < listSize; i++)
			{
				CAnimation * canim = new CAnimation;
				unsigned int nameSize;
				ifs.read((char*)&nameSize, sizeof(unsigned int));
				string name(nameSize, '\0');
				ifs.read(&name[0], nameSize);
				canim->m_sName = name;
				unsigned int size;
				ifs.read((char*)&size, sizeof(unsigned int));
				for (size_t k = 0; k < size; k++)
				{
					CAnimation::TChannel channel;
					unsigned int size2;
					ifs.read((char*)&size2, sizeof(unsigned int));
					for (size_t j = 0; j < size2; j++)
					{
						CAnimation::TKey key;
						ifs.read((char*)&key.m_fTime, sizeof(unsigned int));
						ifs.read((char*)&key.m_mNode, sizeof(XMFLOAT4X4));
						channel.m_vKeys.push_back(key);
					}
					canim->m_vChannels.push_back(channel);
				}
				animations.push_back(canim);
			}
			m_maAnims[AnimName] = animations;
		}
		ifs.close();
		return true;
	}
	return false;
}

void CAssetManager::LoadAnimationFromBinary(string _modelName, string _fileName)
{
	std::string		fileName = "Assets/Animations/" + _fileName + ".bin";
	vector<CAnimation*> animations;
	std::ifstream ifs(fileName, std::ios::binary | ios::in);
	if (ifs.is_open())
	{
		unsigned int listSize;
		ifs.read((char*)&listSize, sizeof(unsigned int));
		if (listSize > 0)
		{
			for (size_t i = 0; i < listSize; i++)
			{
				CAnimation * canim = new CAnimation;
				unsigned int nameSize;
				ifs.read((char*)&nameSize, sizeof(unsigned int));
				string name(nameSize, '\0');
				ifs.read(&name[0], nameSize);
				canim->m_sName = name;
				unsigned int size;
				ifs.read((char*)&size, sizeof(unsigned int));
				for (size_t k = 0; k < size; k++)
				{
					CAnimation::TChannel channel;
					unsigned int size2;
					ifs.read((char*)&size2, sizeof(unsigned int));
					for (size_t j = 0; j < size2; j++)
					{
						CAnimation::TKey key;
						ifs.read((char*)&key.m_fTime, sizeof(unsigned int));
						ifs.read((char*)&key.m_mNode, sizeof(XMFLOAT4X4));
						channel.m_vKeys.push_back(key);
					}
					canim->m_vChannels.push_back(channel);
				}
				animations.push_back(canim);
			}
			m_maAnims[_fileName] = animations;
		}
		ifs.close();
	}
}

void CAssetManager::LoadAnimationFromFBX(string _modelName, string _fileName, CAnimationComponent* _ac)
{
	std::string		filePath = "Assets/Animations/fbx/" + _fileName + ".fbx";

	FBXLoader			loader;
	std::vector<CMesh>* meshes = new vector<CMesh>();
	CAnimation *		anim = new CAnimation;



	//loader.Load( filePath, *meshes, *anim, 1 );

	std::pair<string, vector<CMesh>*> meshPair(_fileName, meshes);
	m_maModels.insert(meshPair);

	if (HasModelAnimationFrames(_modelName))
	{
		animationFrames.at(_modelName).push_back(anim);
	}
	else
	{
		animationFrames.emplace(_modelName, std::vector< CAnimation* >());
		animationFrames.at(_modelName).push_back(anim);
	}
}


float CAssetManager::FloatAttribute(XMLElement* element, string name)
{
	return (element->FirstChildElement(name.c_str()))->FloatAttribute("value");
}

int CAssetManager::IntAttribute(XMLElement* element, string name)
{
	return (element->FirstChildElement(name.c_str()))->IntAttribute("value");
}
std::string CAssetManager::StringAttribute(XMLElement* element, string name)
{
	return (element->FirstChildElement(name.c_str()))->Attribute("value");
}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
std::vector< CAnimation* >* CAssetManager::GetModelAnimationFrames(std::string _modelTag)
{
	return &m_maAnims[_modelTag];
}

bool CAssetManager::HasModelAnimationFrames(std::string _modelTag)
{
	if (m_maAnims.count(_modelTag) == 0)
		return false;

	return true;
}
//------------}

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
