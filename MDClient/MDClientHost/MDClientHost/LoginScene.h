#pragma once
#include "pch.h"

namespace MDClient
{
	class LoginScene : public Manager::Scene
	{
	public:
		void init() override;

		void update() override;

	private:
		GUI _loginWindow{ GUIStyle::Default };
		Font font;

	private:
		void tryLogin();

		void makeLogin();
	};

}

