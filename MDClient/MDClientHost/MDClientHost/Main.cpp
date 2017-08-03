#include "pch.h"
#include "LoginScene.h"
#include "LobbySelectScene.h"
#include "LobbyScene.h"
#include "../../MDClientLogicLib/MDClientLogicLib/ClientLogic.h"
#include "../../MDClientNetLib/MDClientNetLib/TCPNetworkBasic.h"
#include "../../Common/Packet.h"
using PacketQueConcurrency = concurrency::concurrent_queue<MDClientNetworkLib::PacketBody>;

void Main()
{
	PacketQueConcurrency recvPacketQue;
	PacketQueConcurrency sendPacketQue;

	MDClient::ClientLogic logic(&recvPacketQue, &sendPacketQue);
	MDClientNetworkLib::TCPNetworkBasic network(&recvPacketQue, &sendPacketQue);
	network.Connect("10.73.42.109", 23452);
	Manager manager;

	manager.add<MDClient::LoginScene>(L"Login");
	manager.add<MDClient::LobbySelectScene>(L"LobbySelect");
	manager.add<MDClient::LobbyScene>(L"Lobby");
	manager.get()->_logic = &logic;

	while (System::Update())
	{
		if(!manager.updateAndDraw())
		{
			break;
		}
	}
}
