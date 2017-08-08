#pragma once
#include "pch.h"
#include "../../Common/Packet.h"
namespace MDClient
{
	using PacketQueConcurrency = concurrency::concurrent_queue<MDClientNetworkLib::PacketBody>;
	using FuncVoid = std::function<void(void)>;
	using FuncLobbyInfo = std::function<void(MDClientNetworkLib::LobbyListInfo*,int)>;
	using FuncShort2 = std::function<void(short, short)>;
	using FuncRoomInfo = std::function<void(std::array<MDClientNetworkLib::RoomSmallInfo,10>*)>;
	using FuncUserInfo21 = std::function<void(std::array<MDClientNetworkLib::UserSmallInfo,21>*)>;
	using FuncUserInfo5 = std::function<void(std::array<MDClientNetworkLib::UserSmallInfo,5>*)>;

	class ClientLogic
	{
	public:
		ClientLogic(PacketQueConcurrency * recvQue, PacketQueConcurrency * sendQue);
		~ClientLogic();

		bool SendLoginPacket(std::wstring id, std::wstring pw, FuncVoid func);

		bool SendLobbyListPacket(FuncLobbyInfo func);

		bool SendLobbyEnterPacket(const short lobbyId, FuncShort2 func);

		bool SendLobbyRoomListPakcet(FuncRoomInfo func);

		bool SendLobbyUserListPacket(FuncUserInfo21 func);

		bool SendLobbyLeavePacket();

		short SendEnterRoomPacket(bool isCreate,short index, std::wstring roomName);

		bool SendLeaveRoomPacket();

		bool SendLobbyChatPacket();

		bool SendRoomChatPacket();

		bool IsLoginScene();
		bool IsLobbySelectScene();
		bool IsLobbyScene();
		bool IsRoomScene();

		bool GetFuncUserInfo(FuncUserInfo5 func);

		void LogicFunc();

	private:
		bool _isLogined = false;
		bool _isLoginScene = true;
		bool _isLobbySelectScene = false;
		bool _isLobbyScene = false;
		bool _isRoomScene = false;
		PacketQueConcurrency* _recvPacketQue;
		PacketQueConcurrency* _sendPacketQue;

		std::array<MDClientNetworkLib::RoomSmallInfo,10> _roomInfo;
		std::deque<int>_freeRoomIndex;
		std::array<MDClientNetworkLib::UserSmallInfo,21> _userInfo;
		std::deque<int>_freeUserIndex;
		std::array<MDClientNetworkLib::UserSmallInfo,5> _roomUserInfo;
		std::deque<int>_freeRoomUserIndex;
	private:

		void initDeque();
		void initData();

		void packetProcess(MDClientNetworkLib::PacketBody& body);

		//login
		FuncVoid callbackLoginRes;
		
		//get lobby list
		FuncLobbyInfo callbackLobbyListRes;

		//get in lobby
		FuncShort2 callbackLobbyEnterRes;

		//get out from lobby
		void clearLobbyRoomInfo();
		void clearLobbyUserInfo();
		void clearRoomUserInfo();
		//get lobby info - room
		bool sendLobbyRoomListPakcetByStartIndex(short startId);
		
		bool saveRoomInfo(MDClientNetworkLib::PktLobbyRoomListRes* pkt);
		
		FuncRoomInfo callbackRoomListRes;
		
		//get lobby info - user
		bool sendLobbyUserListPacketByStartIndex(short startId);

		bool saveUserInfo(MDClientNetworkLib::PktLobbyUserListRes* pkt);
		
		FuncUserInfo21 callbackUserListRes;



		//get lobby info - changed room
		FuncUserInfo5 callbackRoomUserNtf;
	};


}
