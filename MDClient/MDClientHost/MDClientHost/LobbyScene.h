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
		bool _isRoomChanged = false;
		std::array<String, 21> _userIndex;
		bool _isUserChanded = false;
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
		//처음 로비의 룸리스트를 세팅하는 함수
		void setRoomList(std::vector<MDClientNetworkLib::RoomSmallInfo>* roomList);
		void renewRoomWindow();

		int sendLobbyUserList();
		//처음 로비의 룸리스트를 세팅하는 함수
		void setUserList(std::vector<MDClientNetworkLib::UserSmallInfo>* userList);
		void renewUserWindow();
	};

}

