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

		int _selectedRoomIndex = -1;
	private:
		//각 gui초기화
		int initRoomInfoWindow();
		int initInputWindow();
		int initChatWindow();
		int initUserWindow();

		// 각각의 gui 에 버튼과 텍스트를 추가해주는 함수 
		int addTextAndButton(GUI gui, int listIndex, String name, String index);
		int addText(GUI gui, int listIndex, String name, String index);

		//각각의 siv3d용 함수
		int makeRoomName();
		int makeMemberName();
		int SetGuiText(GUI gui, String textName, String newText);
		int ClearTempRoomAndUserData();

		//로비의 방 리스트를 받아 온다.
		int sendLobbyRoomList();
		//처음 로비의 룸리스트를 세팅하는 함수
		void setRoomList(std::array<MDClientNetworkLib::RoomSmallInfo,10>* roomList);
		void renewRoomWindow();

		//로비의 유저 리스트를 받아온다.
		int sendLobbyUserList();
		//처음 로비의 룸리스트를 세팅하는 함수
		void setUserList(std::array<MDClientNetworkLib::UserSmallInfo,21>* userList);
		void renewUserWindow();

		//로비에 누군가가 들어 왔을때
		//결국 셋리스트를 이용하자.

		//로비에서 나갈 때
		int checkButton();

		//로비의 누군가가 나갔을때
		//결국 셋리스트를 이용하자.

		//방정보가 갱신되었을태

		//채팅이 추가 되었을때

		//방에 들어갈때
		int tryGoRoom(int index);
		int createRoom();
	};

}

