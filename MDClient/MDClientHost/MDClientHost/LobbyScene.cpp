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

		checkButton();

		if (_isRoomChanged = true)
		{
			renewRoomWindow();
		}

		if (_isUserChanded == true)
		{
			renewUserWindow();
		}

		if (m_data->_logic->IsLobbyScene() == false)
		{
			ClearTempRoomAndUserData();

			if (m_data->_logic->IsLobbySelectScene() == true)
			{
				m_data->_scene = MDClient::SceneState::LobbySelectScene;
				changeScene(L"LobbySelect");
			}
			else if (m_data->_logic->IsRoomScene() == true)
			{
				m_data->_scene = MDClient::SceneState::RoomScene;
				changeScene(L"Room");
			}
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

		_roomInfoWindow.add(L"Creat", GUIText::Create(L"CreateRoom"));
		_roomInfoWindow.text(L"Creat").style.width = 100;
		_roomInfoWindow.addln(L"CreatButton", GUIButton::Create(L"Creat"));

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
		_chatWindow.setTitle(Format(L"Channel ID: ", m_data->_lobbyInfo.LobbyId, L" ( My ID:", m_data->_loginID, L')'));
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
	int LobbyScene::ClearTempRoomAndUserData()
	{
		m_data->_lobbyUserArray.fill(UserInfo());
	
		m_data->_roomArray.fill(RoomInfo());

		return 0;
	}
	int LobbyScene::sendLobbyRoomList()
	{
		auto func = [this](std::array<MDClientNetworkLib::RoomSmallInfo,10>* roomList)
		{
			setRoomList(roomList);
		};

		m_data->_logic->SendLobbyRoomListPakcet(func);
		return 0;
	}

	void LobbyScene::setRoomList(std::array<MDClientNetworkLib::RoomSmallInfo,10>* roomList)
	{
		if (roomList->empty() == true)
		{
			return;
		}

		int i = 0;
		for (auto& roomInfo : m_data->_roomArray)
		{
			roomInfo.IsUsed = false;
			roomInfo.RoomArrIndex = i++;
			roomInfo.RoomIndex = roomInfo.RoomArrIndex;
			roomInfo.Title = L" ";
			roomInfo.UserCount = 0;
		}

		i = 0;

		for (const auto& roomInfo : *roomList)
		{

			m_data->_roomArray[i].IsUsed = !(roomInfo.RoomUserCount == 0);
			m_data->_roomArray[i].RoomArrIndex = i;
			m_data->_roomArray[i].RoomIndex = roomInfo.RoomIndex;
			m_data->_roomArray[i].UserCount = roomInfo.RoomUserCount;
			m_data->_roomArray[i].Title = roomInfo.RoomTitle; 
			++i;
		}

		_isRoomChanged = true;
	}

	void LobbyScene::renewRoomWindow()
	{
		for (const auto& index : _roomIndex)
		{
			_roomInfoWindow.text(index).text = L" ";
		}
		auto indexNum = 0;
		for (const auto& roomInfo : m_data->_roomArray)
		{
			if (roomInfo.IsUsed == true)
			{
				roomInfo.Title;
				roomInfo.UserCount;
				auto str = Format(roomInfo.Title,L" ", roomInfo.UserCount, L"/5");
				_roomInfoWindow.text(_roomIndex[indexNum]).text = str;
			}
			indexNum++;
		}

		_isRoomChanged = false;
	}

	int LobbyScene::sendLobbyUserList()
	{
		auto func = [this](std::array<MDClientNetworkLib::UserSmallInfo,21>* userList)
		{
			setUserList(userList);
		};

		m_data->_logic->SendLobbyUserListPacket(func);
		return 0;
	}
	void LobbyScene::setUserList(std::array<MDClientNetworkLib::UserSmallInfo,21>* userList)
	{
		if (userList->empty() == true)
		{
			return;
		}

		int i = 0;
		for (auto& userInfo : m_data->_lobbyUserArray)
		{
			userInfo.IsAlive = false;
			userInfo.RoomIndex = -1;
			userInfo.UserArrIndex = i++;
			userInfo.UserId = L" ";
		}

		i = 0;

		for (const auto& userInfo : *userList)
		{
			m_data->_lobbyUserArray[i].IsAlive =!(userInfo.LobbyUserIndex == -1);
			m_data->_lobbyUserArray[i].UserArrIndex = i;
			m_data->_lobbyUserArray[i].RoomIndex = -1;
			m_data->_lobbyUserArray[i].UserId = MDClientNetworkLib::Util::CharToWstring(userInfo.UserID);
			++i;
		}

		_isUserChanded = true;
	}

	void LobbyScene::renewUserWindow()
	{
		for (const auto& index : _userIndex)
		{
			_userInfoWindow.text(index).text = L" ";
		}
		auto indexNum = 0;
		for (const auto& userInfo : m_data->_lobbyUserArray)
		{
			if (userInfo.IsAlive == true)
			{
				_userInfoWindow.text(_userIndex[indexNum]).text = userInfo.UserId;
				++indexNum;
			}
		}
		_isUserChanded = false;
	}

	int LobbyScene::checkButton()
	{
		if(_roomInfoWindow.button(L"ToLobbySelectButton").pushed)
		{
			m_data->_logic->SendLobbyLeavePacket();
		}
		else if (_roomInfoWindow.button(L"Room1:Button").pushed)
		{
			tryGoRoom(0);
		}
		else if (_roomInfoWindow.button(L"Room2:Button").pushed)
		{
			tryGoRoom(1);
		}
		else if (_roomInfoWindow.button(L"Room3:Button").pushed)
		{
			tryGoRoom(2);
		}
		else if (_roomInfoWindow.button(L"Room4:Button").pushed)
		{
			tryGoRoom(3);
		}
		else if (_roomInfoWindow.button(L"Room5:Button").pushed)
		{
			tryGoRoom(4);
		}
		else if (_roomInfoWindow.button(L"Room6:Button").pushed)
		{
			tryGoRoom(5);
		}
		else if (_roomInfoWindow.button(L"Room7:Button").pushed)
		{
			tryGoRoom(6);
		}
		else if (_roomInfoWindow.button(L"Room8:Button").pushed)
		{
			tryGoRoom(7);
		}
		else if (_roomInfoWindow.button(L"Room9:Button").pushed)
		{
			tryGoRoom(8);
		}
		else if (_roomInfoWindow.button(L"Room10:Button").pushed)
		{
			tryGoRoom(9);
		}
		else if (_roomInfoWindow.button(L"CreatButton").pushed)
		{
			createRoom();
		}
		else
		{

		}

		return 0;
	}
	int LobbyScene::tryGoRoom(int index)
	{
		if (m_data->_roomArray[index].IsUsed == false)return 0;

		m_data->_roomInfo.IsUsed = m_data->_roomArray[index].IsUsed;
		m_data->_roomInfo.RoomArrIndex = m_data->_roomArray[index].RoomArrIndex;
		m_data->_roomInfo.RoomIndex = m_data->_roomArray[index].RoomArrIndex;
		m_data->_roomInfo.Title = m_data->_roomArray[index].Title;
		m_data->_roomInfo.UserCount = m_data->_roomArray[index].UserCount;

		int idx = m_data->_roomArray[index].RoomIndex;

		m_data->_logic->SendEnterRoomPacket(false, idx, m_data->_roomArray[index].Title.c_str());

		return 0;
	}
	int LobbyScene::createRoom()
	{
		m_data->_roomInfo.Title = Format(m_data->_loginID, L"Room", m_data->_numOfRoom++);
		m_data->_roomInfo.UserCount = 1;

		m_data->_roomInfo.RoomIndex = m_data->_logic->SendEnterRoomPacket(true,0, m_data->_roomInfo.Title.c_str());
		return 0;
	}
}