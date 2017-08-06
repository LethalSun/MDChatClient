#include "pch.h"
#include "LobbyScene.h"
#include "../../MDClientLogicLib/MDClientLogicLib/ClientLogic.h"
#include "../../Common/Packet.h"
#include "../../Common/Util.h"
namespace MDClient
{
	void LobbyScene::init()
	{
		Window::Resize(1280, 720);

		initData();

		makeRoomName();
		makeMemberName();

		initRoomInfoWindow();
		initInputWindow();
		initChatWindow();
		initUserWindow();

		sendLobbyRoomList();
		sendLobbyUserList();
	}

	void LobbyScene::update()
	{
		m_data->_logic->LogicFunc();

		if (_isRoomChanged = true)
		{
			renewRoomWindow();
		}

		if (_isUserChanded == true)
		{
			renewUserWindow();
		}
	}

	int LobbyScene::initRoomInfoWindow()
	{
		_roomInfoWindow.setTitle(L"Room List");
		auto i = 0;
		for (const auto& roomName : _roomIndex)
		{
			addTextAndButton(_roomInfoWindow, i, roomName, roomName);
			++i;
		}

		_roomInfoWindow.add(L"ToLobbySelect", GUIText::Create(L"Exit to Login"));
		_roomInfoWindow.text(L"ToLobbySelect").style.width = 100;
		_roomInfoWindow.addln(L"ToLobbySelectButton", GUIButton::Create(L"Exit"));

		_roomInfoWindow.setPos(Point(0, 0));
		_roomInfoWindow.style.width = 250;

		return 1;
	}

	int LobbyScene::initInputWindow()
	{
		auto pos = _roomInfoWindow.getPos();

		auto dPos = _roomInfoWindow.getRect();

		auto WindowPos = static_cast<Point>(Window::Size());

		_inputWindow.setTitle(L"Input");

		_inputWindow.addln(L"InputField", GUITextArea::Create(2, 30));

		auto dWindowPos = _inputWindow.getRect();

		_inputWindow.setPos(Point(pos.x + dPos.w, WindowPos.y - dWindowPos.h));
		_inputWindow.textArea(L"InputField").enabled = true;

		return 1;
	}

	int LobbyScene::initChatWindow()
	{
		auto pos = _roomInfoWindow.getPos();

		auto dPos = _roomInfoWindow.getRect();
		_chatWindow.setTitle(Format(L"Channel ID:", m_data->_lobbyInfo.LobbyId));
		_chatWindow.addln(L"ShowField", GUITextArea::Create(25, 30));
		_chatWindow.setPos(Point(pos.x + dPos.w, 0));
		_chatWindow.textArea(L"ShowField").enabled = true;
		return 1;
	}

	int LobbyScene::initUserWindow()
	{
		auto pos = _chatWindow.getPos();

		auto dPos = _chatWindow.getRect();

		_userInfoWindow.setTitle(L"User");

		auto i = 0;

		for (const auto& userName : _userIndex)
		{
			addText(_userInfoWindow, i, userName, userName);
			++i;
		}

		_userInfoWindow.setPos(Point(pos.x + dPos.w, 0));

		_userInfoWindow.style.width = 250;

		return 1;
	}

	int LobbyScene::addTextAndButton(GUI gui, int listIndex, String name, String index)
	{
		gui.add(index, GUIText::Create(name));
		gui.text(index).style.width = 100;

		auto indexButton = index + String(L"Button");
		gui.addln(indexButton, GUIButton::Create(L"Enter"));

		return 1;
	}

	int LobbyScene::addText(GUI gui, int listIndex, String name, String index)
	{
		gui.addln(index, GUIText::Create(name));
		gui.text(index).style.width = 100;

		return 1;
	}

	int LobbyScene::initData()
	{
		auto i = 0;
		for (const auto& index : m_data->_lobbyUserArray)
		{
			m_data->_lobbyUserIndex.push_back(i);
			++i;
		}

		i = 0;

		for (const auto& index : m_data->_roomArray)
		{
			m_data->_roomIndex.push_back(i);
			++i;
		}
		return 0;
	}

	int LobbyScene::makeRoomName()
	{
		auto i = 0;
		for (auto& roomName : _roomIndex)
		{
			roomName = Format(L"Room", i + 1, L":");
			++i;
		}
		return 0;
	}
	int LobbyScene::makeMemberName()
	{
		auto i = 0;
		for (auto& userName : _userIndex)
		{
			userName = Format(L"User", i + 1, L":");
			++i;
		}
		return 0;
	}
	int LobbyScene::SetGuiText(GUI gui, String textName, String newText)
	{

		return 0;
	}
	int LobbyScene::sendLobbyRoomList()
	{
		auto func = [this](std::vector<MDClientNetworkLib::RoomSmallInfo>* roomList)
		{
			setRoomList(roomList);
		};

		m_data->_logic->SendLobbyRoomListPakcet(func);
		return 0;
	}

	void LobbyScene::setRoomList(std::vector<MDClientNetworkLib::RoomSmallInfo>* roomList)
	{
		if (roomList->empty() == true)
		{
			return;
		}

		for (const auto& roomInfo : *roomList)
		{
			auto i = m_data->_roomIndex.front();
			m_data->_roomIndex.pop_front();

			m_data->_roomArray[i].IsUsed = true;
			m_data->_roomArray[i].RoomArrIndex = i;
			m_data->_roomArray[i].RoomIndex = roomInfo.RoomIndex;
			m_data->_roomArray[i].UserCount = roomInfo.RoomUserCount;
			m_data->_roomArray[i].Title = roomInfo.RoomTitle;
		}

	}

	void LobbyScene::renewRoomWindow()
	{

	}

	int LobbyScene::sendLobbyUserList()
	{
		auto func = [this](std::vector<MDClientNetworkLib::UserSmallInfo>* userList)
		{
			setUserList(userList);
		};

		m_data->_logic->SendLobbyUserListPacket(func);
		return 0;
	}
	void LobbyScene::setUserList(std::vector<MDClientNetworkLib::UserSmallInfo>* userList)
	{
		if (userList->empty() == true)
		{
			return;
		}

		for (const auto& userInfo : *userList)
		{
			auto i = m_data->_lobbyUserIndex.front();
			m_data->_lobbyUserIndex.pop_front();

			m_data->_lobbyUserArray[i].IsAlive = true;
			m_data->_lobbyUserArray[i].UserArrIndex = i;
			m_data->_lobbyUserArray[i].RoomIndex = -1;
			m_data->_lobbyUserArray[i].UserId = MDClientNetworkLib::Util::CharToWstring(userInfo.UserID);
		}
	}
	void LobbyScene::renewUserWindow()
	{

	}
}