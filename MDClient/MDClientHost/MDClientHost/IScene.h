#pragma once

namespace MDClient
{
	class ClientLogic;
	enum class SceneState
	{
		LoginScene = 0,
		LobbySelectScene = 1,
		LobbyScene = 2,
		RoomScene = 3,
	};

	struct UserInfo
	{
		String UserId;
		bool IsAlive = false;
		short RoomIndex;
		int UserArrIndex;
	};

	struct RoomInfo
	{
		bool IsUsed = false;
		short RoomIndex;
		short UserCount;
		int RoomArrIndex;
		String Title;
	};

	struct LobbyInfo
	{
		short LobbyId = -1;
		short UserCount = -1;
		short RoomCount = -1;
		String name;
	};

	struct IScene
	{
	public:
		IScene() = default;
		~IScene() = default;
		SceneState _scene = SceneState::LoginScene;

		ClientLogic* _logic;

		//tempInfo
		std::array<LobbyInfo, 5> _lobbyArray;

		std::array<UserInfo, 21> _lobbyUserArray;
		std::deque<int> _lobbyUserIndex;

		std::array<RoomInfo, 10> _roomArray;
		std::deque<int> _roomIndex;

		std::array<UserInfo, 5> _roomUserArray;

		//LoginScene
		bool _isLogedIn = false;

		String _loginID{};

		String _loginPassWord{};

		//LobbySelectScene
		LobbyInfo _lobbyInfo;

		bool _isLobbyEnterPermitted = false;

		//LobbyScene
		RoomInfo _roomInfo;
	};

}

