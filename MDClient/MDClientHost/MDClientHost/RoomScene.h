#pragma once
#include "pch.h"
#include "../../Common/Packet.h"
namespace MDClient
{
	class RoomScene: public Manager::Scene
	{
	public:

		void init() override;

		void update() override;

	private:
		GUI _chatWindow{ GUIStyle::Default };
		GUI _inputWindow{ GUIStyle::Default };
		GUI _userInfoWindow{ GUIStyle::Default };

		std::array<String, 5> _userIndex;
		bool _isUserChanded = false;

	private:

		int initUserWindow();
		int initInputWindow();
		int initChatWindow();
		
		int addTextAndButton(GUI gui, int listIndex, String name, String index);
		int addText(GUI gui, int listIndex, String name, String index);
		
		int makeUserIndex();

		int ClearRoomAndUserInfo();
		
		int checkButton();
		int renewUserWindow();

		void setRoomUserList(std::array<MDClientNetworkLib::UserSmallInfo,5>* userList);
	};

}


