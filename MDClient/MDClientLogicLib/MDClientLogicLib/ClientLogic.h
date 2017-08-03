#pragma once
#include "pch.h"
#include "../../Common/Packet.h"
namespace MDClient
{
	using PacketQueConcurrency = concurrency::concurrent_queue<MDClientNetworkLib::PacketBody>;
	using FuncVoid = std::function<void(void)>;
	using FuncLobbyInfo = std::function<void(MDClientNetworkLib::LobbyListInfo*,int)>;
	using FuncShort2 = std::function<void(short, short)>;
	using FuncRoomInfo = std::function<void(std::vector<MDClientNetworkLib::RoomSmallInfo>*)>;

	class ClientLogic
	{
	public:
		ClientLogic(PacketQueConcurrency * recvQue, PacketQueConcurrency * sendQue);
		~ClientLogic();

		bool SendLoginPacket(std::wstring id, std::wstring pw, FuncVoid func);

		bool SendLobbyListPacket(FuncLobbyInfo func);

		bool SendLobbyEnterPacket(const short lobbyId, FuncShort2 func);

		bool SendLobbyRoomListPakcet(FuncRoomInfo func);



		bool SendLobbyUserListPacket();
		bool TryGetLobbyUserList();

		bool SendLobbyLeavePacket();
		bool IsLobbyLeavedPermitted();

		bool SendEnterRoomPacket();
		bool IsRoomEnterPermitted();

		bool SendRoomUserList();
		bool TryGetRoomUserList();
		
		bool SendLeaveRoomPacket();
		bool IsRoomLeavePermitted();

		bool SendLobbyChatPacket();
		bool IsLobbyChatSended();
		bool TryGetLobbyChat();
		
		bool SendRoomChatPacket();
		bool IsRoomChatSended();
		bool TryGetRoomChat();

		void LogicFunc();

	private:
		bool _isLogined = false;
		bool _isLoginScene = true;
		bool _isLobbySelectScene = false;
		bool _isLobbyScene = false;

		PacketQueConcurrency* _recvPacketQue;
		PacketQueConcurrency* _sendPacketQue;

		std::vector<MDClientNetworkLib::RoomSmallInfo> _roomInfo;

	private:
		void packetProcess(MDClientNetworkLib::PacketBody& body);

		bool sendContinueLobbyRoomListPakcet(short startId);
		bool saveRoomInfo(MDClientNetworkLib::PktLobbyRoomListRes* pkt);

		FuncVoid callbackLoginRes;
		FuncLobbyInfo callbackLobbyListRes;
		FuncShort2 callbackLobbyEnterRes;
		FuncRoomInfo callbackRoomListRes;
	};


}
