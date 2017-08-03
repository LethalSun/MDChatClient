#pragma once
#include <Siv3D.hpp>
#include <HamFramework.hpp>
#include <array>
#include <unordered_map>
#include <concurrent_queue.h>
#include <vector>
#include <deque>
#include <string>
#include "IScene.h"

using Manager = SceneManager<String, MDClient::IScene>;
