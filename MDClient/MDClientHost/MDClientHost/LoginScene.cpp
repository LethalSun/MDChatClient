#include "pch.h"
#include "LoginScene.h"
#include "../../MDClientLogicLib/MDClientLogicLib/ClientLogic.h"
namespace MDClient
{
	void LoginScene::init()
	{
		_loginWindow.setTitle(L"Login");

		_loginWindow.addln(L"LoginId", GUITextField::Create(none));

		_loginWindow.addln(L"LoginPassword", GUITextField::Create(none));

		_loginWindow.addln(L"LoginButton", GUIButton::Create(L"Login"));

		_loginWindow.setCenter(Window::Center());

		m_data->_scene = SceneState::LoginScene;
	}
	void LoginScene::update()
	{
		m_data->_logic->LogicFunc();

		if (m_data->_scene != SceneState::LoginScene)
		{
			return;
		}

		if (_loginWindow.button(L"LoginButton").pushed)
		{
			tryLogin();
		}

		if (m_data->_isLogedIn == true)
		{
			m_data->_scene = SceneState::LobbySelectScene;
			changeScene(L"LobbySelect");
		}
		return;
	}
	void LoginScene::tryLogin()
	{
		bool retVal = 0;

		m_data->_loginID = _loginWindow.textField(L"LoginId")._get_text();

		m_data->_loginPassWord = _loginWindow.textField(L"LoginPassword")._get_text();

		if (m_data->_isLogedIn == false)
		{
			auto func = [this]() { makeLogin(); };
			std::wstring id(m_data->_loginID.c_str());
			std::wstring pw(m_data->_loginPassWord.c_str());

			retVal = m_data->_logic->SendLoginPacket(id, pw, func);
		}

		if (retVal == false)
		{
			font(L"Send Failed").draw();
		}
	}
	void LoginScene::makeLogin()
	{
		if (m_data->_isLogedIn == false)
		{
			m_data->_isLogedIn = true;
		}
	}
}