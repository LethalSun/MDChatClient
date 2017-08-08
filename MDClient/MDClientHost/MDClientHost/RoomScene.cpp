#include "pch.h"
#include "RoomScene.h"
#include "../../MDClientLogicLib/MDClientLogicLib/ClientLogic.h"
#include "../../Common/Packet.h"
namespace MDClient
{
	void RoomScene::init()
	{
		Window::Resize(1280, 720);

		makeUserIndex();

		initUserWindow();
		initInputWindow();
		initChatWindow();

		auto func = [this](std::array<MDClientNetworkLib::UserSmallInfo,5>* userList)
		{
			setRoomUserList(userList);
		};

		m_data->_logic->GetFuncUserInfo(func);
	}

	void RoomScene::update()
	{
		m_data->_logic->LogicFunc();

		checkButton();

		if (_isUserChanded == true)
		{
			renewUserWindow();
		}

		if (m_data->_logic->IsRoomScene() == false)
		{
			ClearRoomAndUserInfo();
			changeScene(L"Lobby");
		}

	}

	int RoomScene::initUserWindow()
	{
		_userInfoWindow.setTitle(L"User List");
		auto i = 0;
		for (const auto& userName : _userIndex)
		{
			addText(_userInfoWindow, i, userName, userName);
			++i;
		}

		_userInfoWindow.add(L"ToLobby", GUIText::Create(L"Exit to Lobby"));
		_userInfoWindow.text(L"ToLobby").style.width = 100;
		_userInfoWindow.addln(L"ToLobbyButton", GUIButton::Create(L"Exit"));

		_userInfoWindow.setPos(Point(0, 0));
		_userInfoWindow.style.width = 250;

		return 1;
	}
	int RoomScene::initInputWindow()
	{
		auto pos = _userInfoWindow.getPos();

		auto dPos = _userInfoWindow.getRect();

		auto WindowPos = static_cast<Point>(Window::Size());

		_inputWindow.setTitle(L"Input");

		_inputWindow.addln(L"InputField", GUITextArea::Create(2, 30));

		auto dWindowPos = _inputWindow.getRect();

		_inputWindow.setPos(Point(pos.x + dPos.w, WindowPos.y - dWindowPos.h));
		_inputWindow.textArea(L"InputField").enabled = true;

		return 1;
	}
	int RoomScene::initChatWindow()
	{
		auto pos = _userInfoWindow.getPos();

		auto dPos = _userInfoWindow.getRect();
		_chatWindow.setTitle(Format(L"Room ID:", m_data->_roomInfo.Title , L" ( My ID:", m_data->_loginID, L')'));
		_chatWindow.addln(L"ShowField", GUITextArea::Create(25, 30));
		_chatWindow.setPos(Point(pos.x + dPos.w, 0));
		_chatWindow.textArea(L"ShowField").enabled = true;
		return 1;
	}

	int RoomScene::addTextAndButton(GUI gui, int listIndex, String name, String index)
	{
		gui.add(index, GUIText::Create(name));
		gui.text(index).style.width = 100;

		auto indexButton = index + String(L"Button");
		gui.addln(indexButton, GUIButton::Create(L"Enter"));

		return 1;
	}

	int RoomScene::addText(GUI gui, int listIndex, String name, String index)
	{
		gui.addln(index, GUIText::Create(name));
		gui.text(index).style.width = 100;

		return 1;
	}

	int RoomScene::makeUserIndex()
	{
		auto i = 0;
		for(auto& userIndex : _userIndex)
		{
			userIndex = Format(L"User", i + 1, L":");
			++i;
		}
		return 0;
	}
	int RoomScene::ClearRoomAndUserInfo()
	{
		m_data->_roomUserArray.fill(UserInfo());

		m_data->_roomInfo.IsUsed = false;
		m_data->_roomInfo.RoomArrIndex = -1;
		m_data->_roomInfo.RoomIndex = -1;
		m_data->_roomInfo.Title = L"\0";
		m_data->_roomInfo.UserCount = 0;
		return 0;
	}
	int RoomScene::checkButton()
	{
		if (_userInfoWindow.button(L"ToLobbyButton").pushed)
		{
			m_data->_logic->SendLeaveRoomPacket();
		}
		return 0;
	}
	int RoomScene::renewUserWindow()
	{
		for (const auto& index : _userIndex)
		{
			_userInfoWindow.text(index).text = L" ";
		}
		auto indexNum = 0;
		for (const auto& userInfo : m_data->_roomUserArray)
		{
			if (userInfo.IsAlive == true)
			{
				_userInfoWindow.text(_userIndex[indexNum]).text = userInfo.UserId;
				++indexNum;
			}
		}
		_isUserChanded = false;
		return 0;
	}
	void RoomScene::setRoomUserList(std::array<MDClientNetworkLib::UserSmallInfo,5>* userList)
	{
		if (userList->empty() == true)
		{
			return;
		}

		int i = 0;
		
		for (const auto& userInfo : *userList)
		{
			m_data->_roomUserArray[i].IsAlive = !(userInfo.LobbyUserIndex == -1);
			m_data->_roomUserArray[i].UserArrIndex = i;
			m_data->_roomUserArray[i].RoomIndex = m_data->_roomInfo.RoomIndex;
			m_data->_roomUserArray[i].UserId = MDClientNetworkLib::Util::CharToWstring(userInfo.UserID);
			++i;
		}

		_isUserChanded = true;
	}
}