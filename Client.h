
//Client.h
//Created May 2015

#ifndef CLIENT_H
#define CLIENT_H

#include <string.h>
#include "RakPeerInterface.h"
#include "BitStream.h"
#include "RakNetTypes.h" 
#include "NetworkingStruct.h"
#include "EventSystem.h"
#include "GameObject.h"
#include <vector>
#include <unordered_map>
 // MessageID//
using namespace RakNet;
using namespace std;


struct TPlayersData
{
	unsigned char currPlayers;
	unsigned char maxPlayers;
};
class CClient
{
public:
	CClient();
	~CClient();
	void Initialize(NetworkingStruct info);
	void Update();
	void Shutdown();
	void SendEventMessage(EventID _id, TEventData data);
	void SendLobbyReady();
	bool PingForServers();
	bool ConnectToServer(int ind);
	void Disconnect();
	bool ManuallyConnect(string ip);
	void SetSettings(wstring name, unsigned int, unsigned int, unsigned int, unsigned int);
	void SendSettings();
	std::vector<bool> GetReadiedPlayers() { return ReadyPlayers; }
	std::vector<unsigned char> GetPlayerInfo() { return LoadOuts; }
	std::vector<string> GetPlayerNames() { return PlayerNames; }
	void SendLoadoutInfo(unsigned char loadout);	
	void SendPlayerRevive(unsigned int id);
	void Reset();
	void ClearLobbyServers() { lobbyServers.clear(); }
private:

	struct SCALE_MODEL
	{
		int		id;
		float	x;
		float	y;
		float	z;
	};
	unsigned int startWave;
	unsigned int endWave;
	unsigned char maxPlayers;
	unsigned char mapIndex;
	wstring hostName;
	bool clientStarted;
	bool firstCreationReceived;
	bool isHost = false;
	vector<SystemAddress> lobbyServers;
	unordered_map<unsigned long, TPlayersData> m_maPlayerLobby;
	vector<bool> ReadyPlayers;
	vector<unsigned char> LoadOuts;
	vector<string>PlayerNames;
	SystemAddress serverAdd;
	RakPeerInterface *m_rpiPeer;
	Packet *m_pPacket;

	struct GENERALMESSAGE
	{
		int				objectID;
		EventID			eventID;
		TEventData		data;
	};

	std::vector<SCALE_MODEL>		LocalScaleQueue;
	std::vector<SCALE_MODEL>		WorldScaleQueue;
	std::vector<TEventData>			ColorChangeQueue;
	std::vector<GENERALMESSAGE>		GeneralQueue;

};

#endif
