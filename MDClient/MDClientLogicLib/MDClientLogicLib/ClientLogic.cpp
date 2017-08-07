#include "pch.h"
#include "ClientLogic.h"
#include "../../Common/Util.h"

namespace MDClient
{
	ClientLogic::ClientLogic(PacketQueConcurrency * recvQue, PacketQueConcurrency * sendQue)
		:_recvPacketQue{ recvQue },
	_sendPacketQue{ sendQue }
	{
	}


	ClientLogic::~ClientLogic()
	{
	}

	bool ClientLogic::SendLoginPacket(std::wstring id, std::wstring pw, FuncVoid func)
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktLogInReq loginPkt;

		auto idStr = MDClientNetworkLib::Util::WstringToString(id);
		auto pwStr = MDClientNetworkLib::Util::WstringToString(pw);

		strncpy_s(loginPkt.szID, 
			MDClientNetworkLib::MAX_USER_ID_SIZE + 1, 
			&idStr[0], idStr.size());
		strncpy_s(loginPkt.szPW, 
			MDClientNetworkLib::MAX_USER_PASSWORD_SIZE + 1, 
			&pwStr[0], pwStr.size());
		
		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::LOGIN_IN_REQ;
		body.PacketBodySize = sizeof(MDClientNetworkLib::PktLogInReq);
		
		memcpy(body.PacketData, (char*)&loginPkt, body.PacketBodySize);
		
		_sendPacketQue->push(body);
		callbackLoginRes = func;
		return true;
	}

	bool ClientLogic::SendLobbyListPacket(FuncLobbyInfo func)
	{
		MDClientNetworkLib::PacketBody body;
		
		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::LOBBY_LIST_REQ;
		body.PacketBodySize = 0;

		_sendPacketQue->push(body);
		callbackLobbyListRes = func;
		return true;
	}

	bool ClientLogic::SendLobbyEnterPacket(const short lobbyId, FuncShort2 func)
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktLobbyEnterReq pkt;
		pkt.LobbyId = lobbyId;

		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::LOBBY_ENTER_REQ;
		body.PacketBodySize = sizeof(MDClientNetworkLib::PktLobbyEnterReq);

		memcpy(body.PacketData, (char*)&pkt, body.PacketBodySize);

		_sendPacketQue->push(body);

		callbackLobbyEnterRes = func;

		return true;
	}

	bool ClientLogic::SendLobbyRoomListPakcet(FuncRoomInfo func)
	{
		sendLobbyRoomListPakcetByStartIndex(0);

		//함수가 요청될때 마다 콜백함수를 복사하지만
		//어차피 신이 바뀔때 마다 1번씩 복사 될것이므로 일단 넘어가자.
		callbackRoomListRes = func;

		return true;
	}

	bool ClientLogic::SendLobbyUserListPacket(FuncUserInfo func)
	{
		sendLobbyUserListPacketByStartIndex(0);

		callbackUserListRes = func;
		return true;
	}

	bool ClientLogic::SendLobbyLeavePacket()
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktLobbyLeaveReq pkt;

		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::LOBBY_LEAVE_REQ;
		body.PacketBodySize = 0;

		_sendPacketQue->push(body);

		return false;
	}

	bool ClientLogic::IsLobbyScene()
	{
		return _isLobbyScene;
	}

	bool ClientLogic::IsLoginScene()
	{
		return _isLoginScene;
	}

	bool ClientLogic::IsLobbySelectScene()
	{
		return _isLobbySelectScene;
	}

	bool ClientLogic::IsRoomScene()
	{
		return _isRoom;
	}

	void ClientLogic::LogicFunc()
	{
		MDClientNetworkLib::PacketBody body;
		if (!_recvPacketQue->try_pop(body))
		{
			return;
		}
		else
		{
			packetProcess(body);
		}
		return;
	}

	void ClientLogic::packetProcess(MDClientNetworkLib::PacketBody & body)
	{
		using namespace MDClientNetworkLib;
		auto pId = static_cast<PACKET_ID>(body.PacketId);
		switch (pId)
		{
			case PACKET_ID::LOGIN_IN_RES:
			{
				callbackLoginRes();
				_isLogined = true;
				_isLoginScene = false;
				_isLobbySelectScene = true;
				break;
			}
			case PACKET_ID::LOBBY_LIST_RES:
			{
				auto pkt = (PktLobbyListRes*)body.PacketData;
				if (_isLobbySelectScene == true)
				{
					callbackLobbyListRes(pkt->LobbyList, pkt->LobbyCount);
				}
				break;
			}
			case PACKET_ID::LOBBY_ENTER_RES:
			{
				auto pkt = (PktLobbyEnterRes*)body.PacketData;
				auto roomNum = pkt->MaxRoomCount;
				auto userNum = pkt->MaxUserCount;

				callbackLobbyEnterRes(roomNum, userNum);
				_isLobbySelectScene = false;
				_isLobbyScene = true;
				break;
			}
			case PACKET_ID::LOBBY_ENTER_ROOM_LIST_RES:
			{
				if (_isLobbyScene == false)return;
				auto pkt = (PktLobbyRoomListRes*)body.PacketData;

				saveRoomInfo(pkt);

				if (pkt->IsEnd == true)
				{
					if (_isLobbyScene == true)
					{
						callbackRoomListRes(&_roomInfo);
					}
				}
				else
				{
					auto newStartId = _roomInfo.size();

					sendLobbyRoomListPakcetByStartIndex(newStartId);
				}
				break;
			}
			case PACKET_ID::LOBBY_ENTER_USER_LIST_RES:
			{
				if (_isLobbyScene == false)return;

				auto pkt = (PktLobbyUserListRes*)body.PacketData;

				saveUserInfo(pkt);

				if (pkt->IsEnd == true)
				{

					callbackUserListRes(&_userInfo);

				}
				else
				{
					auto newStartId = _userInfo.size();

					sendLobbyUserListPacketByStartIndex(newStartId);
				}
				break;
			}
			case PACKET_ID::LOBBY_ENTER_USER_NTF:
			{
				if (_isLobbyScene == false)return;

				auto pkt = (PktLobbyNewUserInfoNtf*)body.PacketData;

				UserSmallInfo newUser;
				newUser.LobbyUserIndex = _userInfo.back().LobbyUserIndex + 1;
				memcpy(newUser.UserID, pkt->UserID,sizeof(UserSmallInfo::UserID));
				_userInfo.push_back(newUser);

				callbackUserListRes(&_userInfo);
				break;
			}
			case PACKET_ID::LOBBY_LEAVE_RES:
			{
				if (_isLobbyScene == false)return;

				_isLobbyScene = false;
				_isLobbySelectScene = true;
				_roomInfo.clear();
				_userInfo.clear();
				break;
			}
			case PACKET_ID::LOBBY_LEAVE_USER_NTF:
			{
				if (_isLobbyScene == false)return;

				auto pkt = (PktLobbyLeaveUserInfoNtf*)body.PacketData;

				for(_userInfo)
				break;
			}
			case PACKET_ID::ROOM_ENTER_RES:
			{
				break;
			}
			case PACKET_ID::ROOM_ENTER_USER_NTF:
			{
				break;
			}
			case PACKET_ID::ROOM_LEAVE_RES:
			{
				break;
			}
			case PACKET_ID::ROOM_LEAVE_USER_NTF:
			{
				break;
			}
			case PACKET_ID::ROOM_CHANGED_INFO_NTF:
			{
				break;
			}
			case PACKET_ID::LOBBY_CHAT_RES:
			{
				break;
			}
			case PACKET_ID::LOBBY_CHAT_NTF:
			{
				break;
			}
			case PACKET_ID::ROOM_CHAT_RES:
			{
				break;
			}
			case PACKET_ID::ROOM_CHAT_NTF:
			{
				break;
			}
			default:
			{
				break;
			}
		}
	}

	bool ClientLogic::sendLobbyRoomListPakcetByStartIndex(short startId)
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktLobbyRoomListReq pkt;
		pkt.StartRoomIndex = startId;

		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::LOBBY_ENTER_ROOM_LIST_REQ;
		body.PacketBodySize = sizeof(MDClientNetworkLib::PktLobbyRoomListReq);

		memcpy(body.PacketData, (char*)&pkt, body.PacketBodySize);

		_sendPacketQue->push(body);

		return true;
	}

	bool ClientLogic::saveRoomInfo(MDClientNetworkLib::PktLobbyRoomListRes * pkt)
	{
		for (int i = 0; i < pkt->Count; ++i)
		{
			_roomInfo.push_back(pkt->RoomInfo[i]);
		}

		return true;
	}

	bool ClientLogic::sendLobbyUserListPacketByStartIndex(short startId)
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktLobbyUserListReq pkt;
		pkt.StartUserIndex = startId;

		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::LOBBY_ENTER_USER_LIST_REQ;
		body.PacketBodySize = sizeof(MDClientNetworkLib::PktLobbyUserListReq);

		memcpy(body.PacketData, (char*)&pkt, body.PacketBodySize);

		_sendPacketQue->push(body);
		return false;
	}

	bool ClientLogic::saveUserInfo(MDClientNetworkLib::PktLobbyUserListRes * pkt)
	{
		for (int i = 0; i < pkt->Count; ++i)
		{
			_userInfo.push_back(pkt->UserInfo[i]);
		}

		return true;
	}


}
