#include "pch.h"
#include "ClientLogic.h"
#include "../../Common/Util.h"

namespace MDClient
{
	ClientLogic::ClientLogic(PacketQueConcurrency * recvQue, PacketQueConcurrency * sendQue)
		:_recvPacketQue{ recvQue },
	_sendPacketQue{ sendQue }
	{
		initDeque();
		initData();
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

	bool ClientLogic::SendLobbyUserListPacket(FuncUserInfo21 func)
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

		return true;
	}

	short ClientLogic::SendEnterRoomPacket(bool isCreate, short index, std::wstring roomName)
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktRoomEnterReq pkt;

		pkt.IsCreate = isCreate;

		if (isCreate == true)
		{
			if (_freeRoomIndex.size() != 0)
			{
				pkt.RoomIndex = _freeRoomIndex.front();
				_freeRoomIndex.pop_front();
				wcscpy(pkt.RoomTitle, roomName.c_str());
				//wmemcpy(pkt.RoomTitle, roomName.c_str(), sizeof(pkt.RoomTitle));
			}
			
		}
		else
		{
			pkt.RoomIndex = index;
		}

		

		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::ROOM_ENTER_REQ;
		body.PacketBodySize = sizeof(pkt);
		memcpy(body.PacketData, (char*)&pkt, body.PacketBodySize);

		_sendPacketQue->push(body);
		return pkt.RoomIndex;
	}

	bool ClientLogic::SendLeaveRoomPacket()
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktRoomLeaveReq pkt;

		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::ROOM_LEAVE_REQ;
		body.PacketBodySize = 0;

		_sendPacketQue->push(body);
		return true;
	}

	bool ClientLogic::SendLobbyChatPacket(std::wstring chatStr)
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktLobbyChatReq pkt;

		wmemcpy(pkt.Msg, chatStr.c_str(), sizeof(pkt.Msg) / sizeof(wchar_t));

		_lobbyChatTempStr = chatStr;

		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::LOBBY_CHAT_REQ;
		body.PacketBodySize = sizeof(pkt);
		memcpy(body.PacketData, (char*)&pkt, body.PacketBodySize);

		_sendPacketQue->push(body);
		return false;
	}

	bool ClientLogic::SendRoomChatPacket(std::wstring chatStr)
	{
		MDClientNetworkLib::PacketBody body;
		MDClientNetworkLib::PktRoomChatReq pkt;

		wmemcpy(pkt.Msg, chatStr.c_str(), sizeof(pkt.Msg) / sizeof(wchar_t));

		_roomChatTempStr = chatStr;

		body.PacketId = (short)MDClientNetworkLib::PACKET_ID::ROOM_CHAT_REQ;
		body.PacketBodySize = sizeof(pkt);
		memcpy(body.PacketData, (char*)&pkt, body.PacketBodySize);

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
		return _isRoomScene;
	}

	bool ClientLogic::GetFuncUserInfo(FuncUserInfo5 func)
	{
		callbackRoomUserNtf = func;
		return true;
	}

	bool ClientLogic::GetFuncLobbyChat(FuncBoolWstring func)
	{
		callbackLobbyChat = func;
		return true;
	}

	bool ClientLogic::GetFuncRoomChat(FuncBoolWstring func)
	{
		callbackRoomChat = func;
		return true;
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

	void ClientLogic::initDeque()
	{
		int i = 0;
		for (const auto& room : _roomInfo)
		{
			_freeRoomIndex.push_back(i++);
		}

		i = 0;
		for (const auto& user : _userInfo)
		{
			_freeUserIndex.push_back(i++);
		}

		i = 0;
		for (const auto& rUser : _roomUserInfo)
		{
			_freeRoomUserIndex.push_back(i++);
		}
	}

	void ClientLogic::initData()
	{
		for (auto& room : _roomInfo)
		{
			room.RoomIndex = -1;
			room.RoomUserCount = 0;
		}

		for (auto& user : _userInfo)
		{
			user.LobbyUserIndex = -1;
		}

		for (auto& rUser : _roomUserInfo)
		{
			rUser.LobbyUserIndex = -1;
		}
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

				if (_freeUserIndex.size() == 0)
				{
					return;
				}

				auto index = _freeUserIndex.front();
				_freeUserIndex.pop_front();

				_userInfo[index].LobbyUserIndex = index;
				memcpy(_userInfo[index].UserID, pkt->UserID, sizeof(UserSmallInfo::UserID));

				callbackUserListRes(&_userInfo);
				break;
			}
			case PACKET_ID::LOBBY_LEAVE_RES:
			{
				if (_isLobbyScene == false)return;

				_isLobbyScene = false;
				_isLobbySelectScene = true;
				
				clearLobbyRoomInfo();
				clearLobbyUserInfo();

				break;
			}
			case PACKET_ID::LOBBY_LEAVE_USER_NTF:
			{
				if (_isLobbyScene == false)return;

				auto pkt = (PktLobbyLeaveUserInfoNtf*)body.PacketData;

				auto uId = pkt->UserID;

				int i = 0;

				for (auto& user : _userInfo)
				{
					if (strcmp(user.UserID, uId) == 0)
					{
						user.LobbyUserIndex = -1;
						_freeUserIndex.push_back(i);
					}

					++i;
				}

				callbackUserListRes(&_userInfo);
				break;
			}
			case PACKET_ID::ROOM_ENTER_RES:
			{
				if (_isLobbyScene == false)return;

				_isLobbyScene = false;
				_isRoomScene = true;

				clearLobbyRoomInfo();
				clearLobbyUserInfo();

				break;
			}
			case PACKET_ID::ROOM_CHANGED_INFO_NTF:
			{
				if (_isLobbyScene == false)return;

				auto pkt = (PktChangedRoomInfoNtf*)body.PacketData;

				bool IsRoomExist = false;

				int i = 0;

				for (auto& room : _roomInfo)
				{
					if (pkt->RoomInfo.RoomIndex == room.RoomIndex)
					{
						room.RoomUserCount = pkt->RoomInfo.RoomUserCount;
						room.RoomIndex = pkt->RoomInfo.RoomIndex;

						if (pkt->RoomInfo.RoomUserCount == 0)
						{
							room.RoomIndex = -1;
							_freeRoomIndex.push_back(i);
						}
						IsRoomExist = true;
					}
					++i;
				}

				if (IsRoomExist == false)
				{
					if (_freeRoomIndex.size() == 0) return;

					auto idx = _freeRoomIndex.front();
					_freeRoomIndex.pop_front();
					_roomInfo[idx].RoomIndex = pkt->RoomInfo.RoomIndex;
					_roomInfo[idx].RoomUserCount = pkt->RoomInfo.RoomUserCount;
					wmemcpy(_roomInfo[idx].RoomTitle, pkt->RoomInfo.RoomTitle, sizeof(_roomInfo[idx].RoomTitle)/sizeof(wchar_t));
				}

				callbackRoomListRes(&_roomInfo);
				break;
			}
			case PACKET_ID::ROOM_ENTER_USER_NTF:
			{
				if (_isRoomScene == false)return;

				auto pkt = (PktRoomEnterUserInfoNtf*)body.PacketData;

				if (_freeRoomUserIndex.size() == 0)
				{
					return;
				}

				auto idx = _freeRoomUserIndex.front();
				_freeRoomUserIndex.pop_front();

				_roomUserInfo[idx].LobbyUserIndex = -2;
				memcpy(_roomUserInfo[idx].UserID, pkt->UserID, sizeof(_roomUserInfo[idx].UserID));

				callbackRoomUserNtf(&_roomUserInfo);
				break;
			}
			case PACKET_ID::ROOM_LEAVE_RES:
			{
				if (_isRoomScene == false)return;

				_isLobbyScene = true;
				_isRoomScene = false;

				clearRoomUserInfo();

				break;
			}
			case PACKET_ID::ROOM_LEAVE_USER_NTF:
			{
				if (_isRoomScene == false)return;

				auto pkt = (PktRoomLeaveUserInfoNtf*)body.PacketData;

				auto uId = pkt->UserID;

				int i = 0;

				for (auto& user : _roomUserInfo)
				{
					if (strcmp(user.UserID, uId) == 0)
					{
						user.LobbyUserIndex = -1;
						_freeRoomUserIndex.push_back(i);
					}

					++i;
				}

				callbackRoomUserNtf(&_roomUserInfo);
				break;
			}
			case PACKET_ID::LOBBY_CHAT_RES:
			{
				if (_isLobbyScene == false)return;
				
				auto isAddMyId = true;

				callbackLobbyChat(isAddMyId, _lobbyChatTempStr);

				_lobbyChatTempStr.clear();

				break;
			}
			case PACKET_ID::LOBBY_CHAT_NTF:
			{
				if (_isLobbyScene == false)return;

				auto pkt = (PktLobbyChatNtf*)body.PacketData;

				auto id = Util::CharToWstring(pkt->UserID);

				auto msg = std::wstring(pkt->Msg);

				msg = id.append(L": ") + msg;

				auto isAddMyId = false;

				callbackLobbyChat(isAddMyId, msg);

				break;
			}
			case PACKET_ID::ROOM_CHAT_RES:
			{
				if (_isRoomScene == false)return;

				auto isAddMyId = true;

				callbackRoomChat(isAddMyId, _roomChatTempStr);

				_roomChatTempStr.clear();
				break;
			}
			case PACKET_ID::ROOM_CHAT_NTF:
			{
				if (_isRoomScene == false)return;
				auto pkt = (PktLobbyChatNtf*)body.PacketData;

				auto id = Util::CharToWstring(pkt->UserID);

				auto msg = std::wstring(pkt->Msg);

				msg = id.append(L": ") + msg;

				auto isAddMyId = false;

				callbackRoomChat(isAddMyId, msg);
				break;
			}
			default:
			{
				break;
			}
		}
	}

	void ClientLogic::clearLobbyRoomInfo()
	{
		_freeRoomIndex.clear();

		int i = 0;

		for (auto& room : _roomInfo)
		{
			room.RoomIndex = -1;
			room.RoomUserCount = 0;
			_freeRoomIndex.push_back(i++);
		}
	}

	void ClientLogic::clearLobbyUserInfo()
	{
		_freeUserIndex.clear();

		int i = 0;

		for (auto& user : _userInfo)
		{
			user.LobbyUserIndex = -1;

			_freeUserIndex.push_back(i++);
		}
	}

	void ClientLogic::clearRoomUserInfo()
	{
		_freeRoomUserIndex.clear();

		int i = 0;

		for (auto& user : _roomUserInfo)
		{
			user.LobbyUserIndex = -1;

			_freeRoomUserIndex.push_back(i++);
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
		int idx = 0;
		for (int i = 0; i < pkt->Count; ++i)
		{
			idx = _freeRoomIndex.front();
			_freeRoomIndex.pop_front();
			_roomInfo[idx].RoomIndex = pkt->RoomInfo[i].RoomIndex;
			_roomInfo[idx].RoomUserCount = pkt->RoomInfo[i].RoomUserCount;
			wmemcpy(_roomInfo[idx].RoomTitle, pkt->RoomInfo[i].RoomTitle,sizeof(_roomInfo[idx].RoomTitle)/sizeof(wchar_t));
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
		int idx = 0;
		for (int i = 0; i < pkt->Count; ++i)
		{
			idx = _freeUserIndex.front();
			_freeUserIndex.pop_front();
			_userInfo[idx].LobbyUserIndex = pkt->UserInfo[i].LobbyUserIndex;
			memcpy(_userInfo[idx].UserID, pkt->UserInfo[i].UserID, sizeof(_userInfo[idx].UserID));
		}
		return true;
	}



}
