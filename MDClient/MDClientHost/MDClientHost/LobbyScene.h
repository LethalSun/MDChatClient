#pragma once
#include "pch.h"
#include "../../Common/Packet.h"
namespace MDClient
{
	class LobbyScene :public Manager::Scene
	{
	public:
		void init() override;

		void update() override;
	private:
		GUI _chatWindow{ GUIStyle::Default };
		GUI _inputWindow{ GUIStyle::Default };
		GUI _roomInfoWindow{ GUIStyle::Default };
		GUI _userInfoWindow{ GUIStyle::Default };

		String _presentText{ L"Lobby에 입장하셨습니다.\n" };

		//TODO: 룸과 유저의 명수를 모른다면?
		std::array<String, 10> _roomIndex;
		std::array<String, 50> _userIndex;
	private:
		//각 gui초기화
		int initRoomInfoWindow();
		int initInputWindow();
		int initChatWindow();
		int initUserWindow();

		// 각각의 gui 에 버튼과 텍스트를 추가해주는 함수 
		int addTextAndButton(GUI gui, int listIndex, String name, String index);
		int addText(GUI gui, int listIndex, String name, String index);
		int initData();

		//각각의 siv3d용 함수
		int makeRoomName();
		int makeMemberName();
		int SetGuiText(GUI gui, String textName, String newText);

		int sendLobbyRoomList();
		void setRoomList(std::vector<MDClientNetworkLib::RoomSmallInfo>* roomList);
		void renewRoomWindow();
		int sendLobbyUserList();
	};

}

