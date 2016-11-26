
//Client.cpp
//Created May 2015

#include "Client.h"
#include "ObjectManager.h"
#include "Game.h"
#include "GameObject.h"
#include "GameplayState.h"
#include "Events.h"
#include "AudioSystemWwise.h"
#include "Wwise_IDs.h"
#include "Renderer.h"
#include "RenderDebug.h"
#include "SharedDefinitions.h"
#include "PreGameState.h"
#include "LobbySearchState.h"
#include "LobbyState.h"
#include <sstream>
#include "GlobalDef.h"
CClient::CClient()
{
	m_rpiPeer = nullptr;
	clientStarted = false;
	firstCreationReceived = false;
	isHost = false;
	for (size_t i = 0; i < 8; i++)
	{
		ReadyPlayers.push_back(false);
	}
	for (size_t i = 0; i < 8; i++)
	{
		LoadOuts.push_back(0);
	}
}

CClient::~CClient()
{
	m_rpiPeer = RakNet::RakPeerInterface::GetInstance();
	RakNet::RakPeerInterface::DestroyInstance(m_rpiPeer);
}

void CClient::Initialize(NetworkingStruct info)
{
	if (!clientStarted)
	{
		if (m_rpiPeer)
		{
			RakNet::RakPeerInterface::DestroyInstance(m_rpiPeer);
		}
		m_rpiPeer = RakNet::RakPeerInterface::GetInstance();
		SocketDescriptor sd;
		m_rpiPeer->Startup(1, &sd, 1);
		clientStarted = true;
		firstCreationReceived = false;
	}
}
bool CClient::PingForServers()
{
	return m_rpiPeer->Ping("255.255.255.255", 9001, true);
}
bool CClient::ConnectToServer(int ind)
{
	if ((unsigned)ind < lobbyServers.size()) 
	{
		serverAdd = lobbyServers[ind];
		int result = m_rpiPeer->Connect(serverAdd.ToString(), 9001, 0, 0);
		if (result > 0)
			return false;
		return true;
	}
	return false;
}
bool CClient::ManuallyConnect(string ip)
{
	int result = m_rpiPeer->Connect(ip.c_str(), 9001, 0, 0);
	if (result > 0)
		return false;
	if (ip == "127.0.0.1")
		isHost = true;
	return true;
}
void CClient::Update()
{
	if (!m_rpiPeer)
	{
		return;
	}
	if (!clientStarted)
		return;
	for (m_pPacket = m_rpiPeer->Receive(); m_pPacket; m_rpiPeer->DeallocatePacket(m_pPacket), m_pPacket = m_rpiPeer->Receive())
	{
		switch (m_pPacket->data[0])
		{
			case ID_GAME_PLAYER_MESSAGE_EVENT:
			{
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int ID;
				bsIn.Read(ID);
				if (ID != CGame::GetInstance()->GetMyPlayerID())
					break;
				TEventData data;
				RakString s;
				EventID eventInc;
				bsIn.Read(eventInc);
				bsIn.Read(data.i);
				bsIn.Read(data.m);
				bsIn.Read(s);
				data.s = s;

				switch (eventInc)
				{
				case PlaySoundOnOneClient:
				{
					if (strcmp(data.s.c_str(), "Jump") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_JUMP,pos);
					}
					else if (strcmp(data.s.c_str(), "PlayerDied") == 0)
					{
						if (data.i == GlobalDef::DamageID::ePoison)
							break;
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_PLAYERDIED, pos);
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_SFX_DRAGON_DEATH, pos);

					}
					else if (strcmp(data.s.c_str(), "PlayerHit") == 0)
					{
						if (data.i == GlobalDef::DamageID::ePoison)
							break;
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_PLAYERHIT, pos);
					}
					else if (strcmp(data.s.c_str(), "CandyPickup") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_PICKUPCANDY,pos);
					}
					else if (strcmp(data.s.c_str(), "CandyReturn") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_RETURNCANDY,pos);
					}
					else if (strcmp(data.s.c_str(), "ManaPickUp") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_MANACOLLECTED,pos);
					}
					else if (strcmp(data.s.c_str(), "SellTower") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_2D_TOWERSOLD, pos);

					}
					else if (strcmp(data.s.c_str(), "Revived") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_PLAYERRESPAWN, pos);
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_SFX_REVIVE, pos);
					}
					else
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent((AkUniqueID)data.i, pos);
					}
					break;
				}
				case UpdateMana:
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case UpdateCandyOnPlayer:
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case ChargingSpecial:
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case UpdateHUDBuildIcon:
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case CursorVisibility:
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case UpdateHealth:
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case CrosshairTexture:
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case PlayerDead:
				{
					CGame::GetInstance()->GetRenderer()->DeathCamera(true);
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case Revived:
				{
					CGame::GetInstance()->GetRenderer()->DeathCamera(false);
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break;
				}
				case SetBillboardVisibility:
				{
					CGameObject* obj = CGame::GetInstance()->GetObjectManager()->GetObjectByID(data.i);
					if (obj)
						CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					else
					{
						GENERALMESSAGE msg;
						msg.data = data;
						msg.eventID = eventInc;
						msg.objectID = data.i;
						GeneralQueue.push_back(msg);
					}
					break;
				}
				default:
				{	
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					break; 
				}

				}

				break;
			}
			case ID_GAME_MESSAGE_EVENT:
			{
				EventID eventInc;
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(eventInc);

				TEventData data;
				RakString s;
				bsIn.Read(data.i);
				bsIn.Read(data.m);
				bsIn.Read(s);
				data.s = s;
				if (eventInc == EventID::PlaySoundOnClient)
				{
					if (strcmp(data.s.c_str(), "EnemiesTakeCandy") == 0)
					{
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_LOSECANDY);
					}
					else if (strcmp(data.s.c_str(), "Shoot") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_SHOOT,pos);
					}
					else if (strcmp(data.s.c_str(), "StructureHit") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_STRUCTUREHIT,pos);
					}
					else if (strcmp(data.s.c_str(), "StructureDestoryed") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_STRUCTUREDESTROYED,pos);
					}
					else if (strcmp(data.s.c_str(), "SpecialCharge") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_SPECIALCHARGE,pos);
					}
					else if (strcmp(data.s.c_str(), "SpecialShoot") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_SHOOTSPECIAL,pos);
					}
					else if (strcmp(data.s.c_str(), "HitEnemy") == 0)
					{
						if (data.i == GlobalDef::DamageID::ePoison)
							break;
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_ENEMYHIT, pos);
					}
					else if (strcmp(data.s.c_str(), "EnemyDied") == 0)
					{
						if (data.i == GlobalDef::DamageID::ePoison)
							break;
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_ENEMYKILLED, pos);
					}
					else if (strcmp(data.s.c_str(), "PoisonActivate") == 0)
					{

						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_TRAPACTIVATION, pos);
					}
					else if (strcmp(data.s.c_str(), "Win") == 0)
					{
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_GAMEBACKGROUND);
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_WINGAME);
					}
					else if (strcmp(data.s.c_str(), "Lose") == 0)
					{
						if (data.i)
						{
							AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_GAMEBACKGROUND);
							AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_LOSEGAME);
						}
						else
							AudioSystemWwise::Get()->PostEvent(AK::EVENTS::STOP_3D_LOSEGAME);
					}
					else if (strcmp(data.s.c_str(), "WaveEnd") == 0)
					{
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_WAVEEND);
					}
					else if (strcmp(data.s.c_str(), "WaveStart") == 0)
					{
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_WAVESTART);
					}
					else if (strcmp(data.s.c_str(), "CandyReturn") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_RETURNCANDY,pos);
					}
					else if (strcmp(data.s.c_str(), "EnemyAttack") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_SHOOT, pos);
					}
					else if (strcmp(data.s.c_str(), "Explision") == 0)
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent(AK::EVENTS::PLAY_3D_EXPLOSION, pos);
					}
					else
					{
						XMFLOAT3 pos{ data.m[12], data.m[13], data.m[14] };
						AudioSystemWwise::Get()->PostEvent((AkUniqueID)data.i, pos);
					}
				}
				else if (eventInc == EventID::SetEmitterActive)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}
				else if (eventInc == EventID::Win)
				{
					CGameplayState::GetInstance()->YouWin();
				}
				else if (eventInc == EventID::Lose)
				{
					CGameplayState::GetInstance()->YouLose();
				}
				else if (eventInc == EventID::UpdateEnemiesRemaning)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}
				else if (eventInc == EventID::UpdateHealth)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}
				else if (eventInc == EventID::UpdateGem)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}
				else if (eventInc == EventID::UpdateCandy)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}
				else if (eventInc == EventID::UpdateCurrency)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}
			/*	else if (eventInc == EventID::ChangeTextureColor)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}*/
				else if (eventInc == EventID::DisplayHUDText)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}
				else if (eventInc == EventID::BuildPhaseStart)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(EventID::DisplayHUDText, &data);
				}
				else if (eventInc == EventID::ActionPhaseStart)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(EventID::DisplayHUDText, &data);
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(EventID::ActionPhaseStart, &data);
				}
				else if (eventInc == EventID::UpdateCurrentWave)
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				}
				else if (eventInc == EventID::DrawDebugLine)
				{
					auto renderer = CGame::GetInstance()->GetRenderer();
					renderer->EnableDebugRendering(true);
					XMFLOAT3 start = XMFLOAT3(data.m[0], data.m[1], data.m[2]);
					XMFLOAT3 end = XMFLOAT3(data.m[3], data.m[4], data.m[5]);
					XMFLOAT4 startColor = XMFLOAT4(data.m[6], data.m[7], data.m[8], data.m[9]);
					XMFLOAT4 endColor = XMFLOAT4(data.m[10], data.m[11], data.m[12], data.m[13]);
					RenderLine line(start, end, startColor, endColor);
					renderer->QueueLine(line);
				}
				else if (eventInc == EventID::DrawDebugSphere)
				{
					//DirectX::XMFLOAT4X4 worldMatrix;
					//	DirectX::XMFLOAT4	color;
					//unsigned			id;
					//	bool				visible;
					auto renderer = CGame::GetInstance()->GetRenderer();
					renderer->EnableDebugRendering(true);

					XMFLOAT4 endColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
					RenderSphereData sphereData;
					sphereData.color = endColor;
					sphereData.id = data.i;
					sphereData.visible = true;
					memcpy(&sphereData.worldMatrix, data.m, sizeof(float) * 16);
					//	RenderSphere sphere(sphereData);
					renderer->AddSphere(sphereData);

				}
				else if (eventInc == EventID::ChangeTextureColor)
				{
					CGameObject* obj = CGame::GetInstance()->GetObjectManager()->GetObjectByID(data.i);
					// If object exist, scale, else add to queue since obj has not been spawned yet
					// Added because of object creation delay between server and client
					if (obj)
						CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
					else
					{
						ColorChangeQueue.push_back(data);
					}

				}
				else if (eventInc == EventID::SetLocalScale)
				{
					CGameObject* obj = CGame::GetInstance()->GetObjectManager()->GetObjectByID(data.i);
					// If object exist, scale, else add to queue since obj has not been spawned yet
					// Added because of object creation delay between server and client
					if (obj)
						obj->SetLocalScale(data.m[0], data.m[1], data.m[2]);
					else
					{
						SCALE_MODEL sm;
						sm.id = data.i;
						sm.x = data.m[0];
						sm.y = data.m[1];
						sm.z = data.m[2];
						LocalScaleQueue.push_back(sm);
					}
					break;
				}
				else if (eventInc == EventID::SetWorldScale)
				{
					CGameObject* obj = CGame::GetInstance()->GetObjectManager()->GetObjectByID(data.i);
					// If object exist, scale, else add to queue since obj has not been spawned yet
					// Added because of object creation delay between server and client
					if (obj)
						obj->SetWorldScale(data.m[0], data.m[1], data.m[2]);
					else
					{
						SCALE_MODEL sm;
						sm.id = data.i;
						sm.x = data.m[0];
						sm.y = data.m[1];
						sm.z = data.m[2];
						WorldScaleQueue.push_back(sm);
					}
					break;
				}
				else
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(eventInc, &data);
				break;
			}
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				printf("Another client has disconnected.\n");
				break;
			case ID_REMOTE_CONNECTION_LOST:
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				printf("Our connection request has been accepted.\n");
				serverAdd = m_pPacket->systemAddress;
				break;
			}
			case ID_NEW_INCOMING_CONNECTION:
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				printf("We have been disconnected.\n");
				break;
			case ID_CONNECTION_LOST:
			{
				printf("Connection lost.\n");
				CGame::GetInstance()->GetEventSystem()->SendEvent(EventID::HostDisconnected, &TEventData());
				break;
			}
			case ID_GAME_MESSAGE_SNAPSHOT:
			{
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				int objCount;
				bsIn.Read(objCount);
				CObjectManager* objMan = CGame::GetInstance()->GetObjectManager();
				for (int i = 0; i < objCount; i++)
				{
					unsigned int index;
					bsIn.Read(index);
					XMFLOAT4X4 tempMat;
					bsIn.Read(tempMat);
					bool active;
					bsIn.Read(active);
					CGameObject* tObj = objMan->GetObjectByID(index);
					if (tObj)
					{
						tObj->SetMatrix(&XMLoadFloat4x4(&tempMat));
						tObj->SetActive(active);
					}
				}
				break;
			}
			case ID_GAME_REGISTER_CAMERA:
			{
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(sizeof(unsigned char));
				unsigned int playerInd;
				bsIn.Read(playerInd);
				CGame::GetInstance()->SetMyPlayerID(playerInd);
				CGame::GetInstance()->GetRenderer()->SetCameraInd(playerInd + 2);
				if (isHost)
					SendSettings();
				break;
			}
			case ID_GAME_MESSAGE_CREATION:
			{	
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				unsigned int objCount;
				bsIn.Read(objCount);
				CObjectManager* objMan = CGame::GetInstance()->GetObjectManager();
				for (size_t i = 0; i < objCount; i++)
				{
					RakNet::RakString rs;
					bsIn.Read(rs);
					string prefab = rs.C_String();
					XMFLOAT4X4 startMat;
					bsIn.Read(startMat);
					int id;
					bsIn.Read(id);
					if (!objMan->GetObjectByID(id))
					{
						objMan->GenerateObject(prefab, startMat, id);
					}
				}
				if (!firstCreationReceived)
				{
					if (serverAdd.debugPort == 0)
					{
						serverAdd = m_pPacket->systemAddress;
					}
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_GAME_CREATION_RECEIVED);
					wstring wName = CLobbyState::GetInstance()->GetPlayerName();
					string name;
					name.assign(wName.begin(), wName.end());
					RakNet::RakString myName = name.c_str();
					bsOut.Write(myName);
					m_rpiPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAdd, false);
					firstCreationReceived = true;
				}
				printf("Initialized GameState's ObjectManager.\n");
				printf("Objects Added: %i \n", objCount);
				break;
			}
			case ID_UNCONNECTED_PONG:
			{
				RakNet::TimeMS time;
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(1);
				bsIn.Read(time);
				unsigned int packetLength = m_pPacket->length;
				unsigned char currPlayer;
				unsigned char maxPlayer;
				bsIn.Read(currPlayer);
				bsIn.Read(maxPlayer);
				unsigned char gameActive;
				bsIn.Read(gameActive);
				if (gameActive == 1 || currPlayer == maxPlayer)
				{
					break;
				}
				unsigned char length;
				bsIn.Read(length);
				string name = "";
				for (unsigned char i = 0; i < length; i++)
				{
					char letter;
					bsIn.Read(letter);
					name += letter;
				}
				printf("Got pong from %s with time %i\n", m_pPacket->systemAddress.ToString(), time);
				bool serverFound = false;
				for (size_t i = 0; i < lobbyServers.size(); i++)
				{
					if (SystemAddress::ToInteger(lobbyServers[i]) == SystemAddress::ToInteger(m_pPacket->systemAddress))
					{
						serverFound = true;
						break;
					}
				}
				if (!serverFound)
				{
					if (currPlayer == maxPlayer)
						break;
					lobbyServers.push_back(m_pPacket->systemAddress);
					std::stringstream temp;
					temp << name.c_str();
					temp << "'s Lobby ";
					temp << "Players: ";
					temp << (int)currPlayer;
					temp << " / ";
					temp << (int)maxPlayer;
					CLobbySearchState::GetInstance()->AddLobby(temp.str(), maxPlayer, name);
				}
				break;
			}
			case ID_LOBBY_START:
			{
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(1);
				unsigned int playerCount;
				bsIn.Read(playerCount);
				unsigned int ID;
				unsigned char prefab;
				string prefabName;
				CObjectManager* objMan = CGame::GetInstance()->GetObjectManager();
				for (size_t i = 0; i < playerCount; i++)
				{
					bsIn.Read(ID);
					bsIn.Read(prefab);
					if (prefab == 1)
					{
						prefabName = "PlayerBlue";
					}
					else if (prefab == 2)
					{
						prefabName = "PlayerRed";
					}
					else if (prefab == 3)
					{
						prefabName = "PlayerGreen";
					}
					else if (prefab == 4)
					{
						prefabName = "PlayerGold";
					}
					objMan->FillInPlayerInfo(objMan->GetObjectByID(ID), prefabName);
				}
				CLobbyState::GetInstance()->GameStart();
				break;
			}
			case ID_LOBBY_INFO:
			{
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(1);
				unsigned char playerCount;
				bsIn.Read(playerCount);
				for (size_t i = 0; i < 8; i++)
				{
					LoadOuts[i] = 0;
					ReadyPlayers[i] = false;
				}
				for (size_t i = 0; i < playerCount; i++)
				{
					bsIn.Read(LoadOuts[i]);
					unsigned char ready;
					bsIn.Read(ready);
					if (ready == 1)
						ReadyPlayers[i] = true;
					else
						ReadyPlayers[i] = false;
				}
				unsigned char namedPlayerCount;
				bsIn.Read(namedPlayerCount);
				for (size_t i = 0; i < namedPlayerCount; i++)
				{
					RakNet::RakString playerName;
					bsIn.Read(playerName);
					string name = playerName.C_String();
					if (i >= PlayerNames.size())
					{
						PlayerNames.push_back(name);
					}
					else
					{
						PlayerNames[i] = name;
					}
				}
				break;
			}
			case ID_GAME_PLAYER_INFO:
			{
				RakNet::BitStream bsIn(m_pPacket->data, m_pPacket->length, false);
				bsIn.IgnoreBytes(1);
				unsigned int playID;
				unsigned char fireable;
				bsIn.Read(playID);
				bsIn.Read(fireable);
				if (playID == CGame::GetInstance()->GetMyPlayerID())
				{
					CGame::GetInstance()->GetEventSystem()->SendEventImmediate(EventID::FireActive, nullptr);
				}
				break;
			}
			default:
			{
				printf("Message with identifier %i has arrived.\n", m_pPacket->data[0]);
				break;
			}
		}
	}

	// Added because of object creation delay between server and client
	for (int i = ColorChangeQueue.size() - 1; i >= 0; i--)
	{
		CGameObject* obj = CGame::GetInstance()->GetObjectManager()->GetObjectByID(ColorChangeQueue[i].i);
		// If object exist, scale, else add to queue since obj has not been spawned yet
		// Added because of object creation delay between server and client
		if (obj)
		{
			CGame::GetInstance()->GetEventSystem()->SendEventImmediate(EventID::ChangeTextureColor, &ColorChangeQueue[i]);
			ColorChangeQueue.pop_back();
		}
	}
	for (int i = LocalScaleQueue.size() - 1; i >= 0; i--)
	{
		CGameObject* obj = CGame::GetInstance()->GetObjectManager()->GetObjectByID(LocalScaleQueue[i].id);
		if (obj)
		{
 			obj->SetLocalScale(LocalScaleQueue[i].x, LocalScaleQueue[i].y, LocalScaleQueue[i].z);
			LocalScaleQueue.pop_back();
		}
	}
	for (int i = WorldScaleQueue.size() - 1; i >= 0; i--)
	{
		CGameObject* obj = CGame::GetInstance()->GetObjectManager()->GetObjectByID(WorldScaleQueue[i].id);
		if (obj)
		{
			obj->SetWorldScale(WorldScaleQueue[i].x, WorldScaleQueue[i].y, WorldScaleQueue[i].z);
			WorldScaleQueue.pop_back();
		}
	}

	for (int i = GeneralQueue.size() - 1; i >= 0; i--)
	{
		CGameObject* obj = CGame::GetInstance()->GetObjectManager()->GetObjectByID(GeneralQueue[i].objectID);
		if (obj)
		{
			CGame::GetInstance()->GetEventSystem()->SendEventImmediate(GeneralQueue[i].eventID, &GeneralQueue[i].data);
			GeneralQueue.pop_back();
		}
	}
}

void CClient::Reset()
{
	firstCreationReceived = false;
}
void CClient::Disconnect()
{
	m_rpiPeer->CloseConnection(serverAdd, true);
}
void CClient::SendEventMessage(EventID _id, TEventData data)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_EVENT);
	bsOut.Write(_id);
	bsOut.Write(data.i);
	bsOut.Write(data.m);
	RakString s = data.s.c_str();
	bsOut.Write(s);
	m_rpiPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAdd, false);
}
void CClient::SendLobbyReady()
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_LOBBY_READY);
	m_rpiPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAdd, false);

}
void CClient::SetSettings(wstring name, unsigned int _mapIndex, unsigned int _maxPlayers, unsigned int _startWave, unsigned int _endWave)
{
	hostName = name;
	mapIndex = (unsigned char)_mapIndex;
	maxPlayers = (unsigned char)_maxPlayers;
	startWave = _startWave;
	endWave = _endWave;
}
void CClient::SendSettings()
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_LOBBY_SETTINGS);
	unsigned char buffer[256];
	hostName.c_str();
	ZeroMemory(buffer, 256);
	bsOut.Write((unsigned char)hostName.length());
	string sendString(hostName.begin(), hostName.end());

	memcpy(buffer, sendString.c_str(), sizeof(char) * sendString.length());
	int i = 0;
	while (buffer[i] != 0)
	{
		bsOut.Write(buffer[i]);
		i++;
	}
	bsOut.Write(mapIndex);
	bsOut.Write(maxPlayers);
	bsOut.Write(startWave);
	bsOut.Write(endWave);
	m_rpiPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAdd, false);

}
void CClient::SendLoadoutInfo(unsigned char loadout)
{

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_LOBBY_LOADOUT_CHANGE);
	bsOut.Write(loadout);
	m_rpiPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAdd, false);


}	
void CClient::SendPlayerRevive(unsigned int id)
{
	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)ID_GAME_PLAYER_REVIVE);
	bsOut.Write(id);
	m_rpiPeer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverAdd, false);
}
void CClient::Shutdown()
{
	for (size_t i = 0; i < PlayerNames.size(); i++)
	{
		PlayerNames[i] = "";
	}
	clientStarted = false;
}