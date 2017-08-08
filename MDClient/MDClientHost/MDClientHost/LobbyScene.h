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

		int _selectedRoomIndex = -1;
	private:
		//�� gui�ʱ�ȭ
		int initRoomInfoWindow();
		int initInputWindow();
		int initChatWindow();
		int initUserWindow();

		// ������ gui �� ��ư�� �ؽ�Ʈ�� �߰����ִ� �Լ� 
		int addTextAndButton(GUI gui, int listIndex, String name, String index);
		int addText(GUI gui, int listIndex, String name, String index);

		//������ siv3d�� �Լ�
		int makeRoomName();
		int makeMemberName();
		int SetGuiText(GUI gui, String textName, String newText);
		int ClearTempRoomAndUserData();

		//�κ��� �� ����Ʈ�� �޾� �´�.
		int sendLobbyRoomList();
		//ó�� �κ��� �븮��Ʈ�� �����ϴ� �Լ�
		void setRoomList(std::array<MDClientNetworkLib::RoomSmallInfo,10>* roomList);
		void renewRoomWindow();

		//�κ��� ���� ����Ʈ�� �޾ƿ´�.
		int sendLobbyUserList();
		//ó�� �κ��� �븮��Ʈ�� �����ϴ� �Լ�
		void setUserList(std::array<MDClientNetworkLib::UserSmallInfo,21>* userList);
		void renewUserWindow();

		//�κ� �������� ��� ������
		//�ᱹ �¸���Ʈ�� �̿�����.

		//�κ񿡼� ���� ��
		int checkButton();

		//�κ��� �������� ��������
		//�ᱹ �¸���Ʈ�� �̿�����.

		//�������� ���ŵǾ�����

		//ä���� �߰� �Ǿ�����

		//�濡 ����
		int tryGoRoom(int index);
		int createRoom();
	};

}

