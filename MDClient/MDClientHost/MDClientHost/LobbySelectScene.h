#pragma once
#include "pch.h"
#include "../../Common/Packet.h"
namespace MDClient
{
	class LobbySelectScene :public Manager::Scene
	{
	public:
		void init() override;

		void update() override;

	private:
		GUI LobbySelectWindow{ GUIStyle::Default };

		std::array<LobbyInfo, 5> _lobbyList{};

		void addTextAndButtonToGui(int lobbyNum);

		void renewLobbyInfo();

		short checkButton();

		void tryEnterLobby(int idx);

		//callback
		void setLobbyList(MDClientNetworkLib::LobbyListInfo* lobbyList, int lobbyNum);

		void setEnterLobbyInfo(short roomNum, short userNum);
	};

}

