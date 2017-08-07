#include "pch.h"
#include "LobbySelectScene.h"
#include "../../MDClientLogicLib/MDClientLogicLib/ClientLogic.h"
namespace MDClient
{
	void LobbySelectScene::init()
	{
		LobbySelectWindow.setTitle(L"Select Lobby");

		for (int i = 0; i < 5; ++i)
		{
			addTextAndButtonToGui(i);
		}

		LobbySelectWindow.setCenter(Window::Center());
		//�κ� ����Ʈ ó�� ��û
		auto func = [this](MDClientNetworkLib::LobbyListInfo* lobbyList, int lobbyNum)
		{
			setLobbyList(lobbyList, lobbyNum);
		};

		m_data->_logic->SendLobbyListPacket(func);
	}

	void LobbySelectScene::update()
	{
		m_data->_logic->LogicFunc();

		
		renewLobbyInfo();

		//Ŭ���Է��� Ȯ���ϰ� ������ ������ ������
		auto indexInt = checkButton();

		tryEnterLobby(indexInt);

		//������ �κ����� ���¸� Ȯ���Ѵ�.

		if (m_data->_logic-> IsLobbyScene() == true)
		{
			m_data->_scene = SceneState::LobbyScene;
			changeScene(L"Lobby");
		}

		//TODO:������ ���� �ѹ��� �κ񸮽�Ʈ�� ������ �޾ƿ´� ��� �����ϰ� ������ 
		//��Ŷ�� ��� ������ �Ѵ�.
		if (m_data->_scene == SceneState::LobbyScene)
		{
			auto func = [this](MDClientNetworkLib::LobbyListInfo* lobbyList, int lobbyNum)
			{
				setLobbyList(lobbyList, lobbyNum);
			};
			m_data->_logic->SendLobbyListPacket(func);
		}
	}

	void LobbySelectScene::setLobbyList(MDClientNetworkLib::LobbyListInfo * lobbyList, int lobbyNum)
	{
		if (m_data->_scene != SceneState::LobbySelectScene)
		{
			return;
		}

		for (int i = 0; i < lobbyNum; ++i)
		{
			auto id = m_data->_lobbyArray[i].LobbyId = lobbyList[i].LobbyId;
			auto count = m_data->_lobbyArray[i].UserCount = lobbyList[i].LobbyUserCount;
			m_data->_lobbyArray[i].name = Format(L"Lobby", id,
				L" ", count, L"/50");
		}
	}

	void LobbySelectScene::renewLobbyInfo()
	{
		int i = 0;
		for (const auto &lobby : m_data->_lobbyArray)
		{
			auto index = ToString(i, 10);
			LobbySelectWindow.text(index).text = lobby.name;
			++i;
		}
	}

	void LobbySelectScene::setEnterLobbyInfo(short roomNum, short userNum)
	{
		m_data->_lobbyInfo.RoomCount = roomNum;
		m_data->_lobbyInfo.UserCount = userNum;
	}


	short LobbySelectScene::checkButton()
	{
		if (LobbySelectWindow.button(L"0").pushed)
		{

			return 0;
		}
		else if (LobbySelectWindow.button(L"1").pushed)
		{

			return 1;
		}
		else if (LobbySelectWindow.button(L"2").pushed)
		{

			return 2;
		}
		else if (LobbySelectWindow.button(L"3").pushed)
		{

			return 3;
		}
		else if (LobbySelectWindow.button(L"4").pushed)
		{

			return 4;
		}
		else
		{

			return -1;
		}
	}

	void LobbySelectScene::tryEnterLobby(int indexInt)
	{
		if (indexInt >= 0 && indexInt <= 4)
		{
			auto func = [this](short roomNum, short userNum)
			{
				setEnterLobbyInfo(roomNum, userNum);
			};
			m_data->_lobbyInfo.LobbyId = indexInt;
			m_data->_lobbyInfo.name = m_data->_lobbyArray[indexInt].name;
			m_data->_logic->SendLobbyEnterPacket(indexInt, func);
		}
	}

	void LobbySelectScene::addTextAndButtonToGui(int lobbyNum)
	{
		auto index = ToString(lobbyNum, 10);

		auto id = m_data->_lobbyArray[lobbyNum].LobbyId = 0;
		auto count = m_data->_lobbyArray[lobbyNum].UserCount = 0;
		auto name = m_data->_lobbyArray[lobbyNum].name = Format(L"Lobby", id,
			L" ", count, L"/50");

		LobbySelectWindow.add(index, GUIText::Create(name));
		LobbySelectWindow.text(index).style.width = 100;

		LobbySelectWindow.addln(index, GUIButton::Create(L"Enter"));

		return;
	}
}
