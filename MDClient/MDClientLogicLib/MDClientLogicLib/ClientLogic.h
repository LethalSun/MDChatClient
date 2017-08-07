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
	using FuncUserInfo = std::function<void(std::vector<MDClientNetworkLib::UserSmallInfo>*)>;


	class ClientLogic
	{
	public:
		ClientLogic(PacketQueConcurrency * recvQue, PacketQueConcurrency * sendQue);
		~ClientLogic();

		bool SendLoginPacket(std::wstring id, std::wstring pw, FuncVoid func);

		bool SendLobbyListPacket(FuncLobbyInfo func);

		bool SendLobbyEnterPacket(const short lobbyId, FuncShort2 func);

		bool SendLobbyRoomListPakcet(FuncRoomInfo func);



		bool SendLobbyUserListPacket(FuncUserInfo func);

		bool SendLobbyLeavePacket();

		bool IsLoginScene();
		bool IsLobbySelectScene();
		bool IsLobbyScene();
		bool IsRoomScene();

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
		bool _isRoom = false;
		PacketQueConcurrency* _recvPacketQue;
		PacketQueConcurrency* _sendPacketQue;

		std::vector<MDClientNetworkLib::RoomSmallInfo> _roomInfo;
		std::vector<MDClientNetworkLib::UserSmallInfo> _userInfo;

	private:
		void packetProcess(MDClientNetworkLib::PacketBody& body);

		FuncVoid callbackLoginRes;
		
		FuncLobbyInfo callbackLobbyListRes;
		
		FuncShort2 callbackLobbyEnterRes;

		//로비에서 방리스트 불러올때 필요한 함수
		bool sendLobbyRoomListPakcetByStartIndex(short startId);
		
		bool saveRoomInfo(MDClientNetworkLib::PktLobbyRoomListRes* pkt);
		
		FuncRoomInfo callbackRoomListRes;
		
		//로비에서 유저 리스트 불러올때 필요한 함수

		bool sendLobbyUserListPacketByStartIndex(short startId);

		bool saveUserInfo(MDClientNetworkLib::PktLobbyUserListRes* pkt);
		
		FuncUserInfo callbackUserListRes;
	};


}
