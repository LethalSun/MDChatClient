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
		std::array<String, 50> _userIndex;
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
		void setRoomList(std::vector<MDClientNetworkLib::RoomSmallInfo>* roomList);
		void renewRoomWindow();
		int sendLobbyUserList();
	};

}

