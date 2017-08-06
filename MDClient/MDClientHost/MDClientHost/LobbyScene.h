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

		String _presentText{ L"Lobby�� �����ϼ̽��ϴ�.\n" };

		//TODO: ��� ������ ����� �𸥴ٸ�?
		std::array<String, 10> _roomIndex;
		bool _isRoomChanged = false;
		std::array<String, 21> _userIndex;
		bool _isUserChanded = false;
	private:
		//�� gui�ʱ�ȭ
		int initRoomInfoWindow();
		int initInputWindow();
		int initChatWindow();
		int initUserWindow();

		// ������ gui �� ��ư�� �ؽ�Ʈ�� �߰����ִ� �Լ� 
		int addTextAndButton(GUI gui, int listIndex, String name, String index);
		int addText(GUI gui, int listIndex, String name, String index);
		int initData();

		//������ siv3d�� �Լ�
		int makeRoomName();
		int makeMemberName();
		int SetGuiText(GUI gui, String textName, String newText);

		int sendLobbyRoomList();
		//ó�� �κ��� �븮��Ʈ�� �����ϴ� �Լ�
		void setRoomList(std::vector<MDClientNetworkLib::RoomSmallInfo>* roomList);
		void renewRoomWindow();

		int sendLobbyUserList();
		//ó�� �κ��� �븮��Ʈ�� �����ϴ� �Լ�
		void setUserList(std::vector<MDClientNetworkLib::UserSmallInfo>* userList);
		void renewUserWindow();
	};

}

