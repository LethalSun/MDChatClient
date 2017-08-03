#pragma once
#pragma comment(lib,"ws2_32")

#include <iostream>
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <array>
#include <deque>
#include <concurrent_queue.h>
#include <concurrent_unordered_map.h>
#include <algorithm>
#include <functional>
#include <string>
#include "../../Common/Util.h"
#include "../../Common/Packet.h"
#include "../../Common/PacketID.h"
//실제 패킷의 최대 크기
const int MaxPacketSize =4096;

//내가 사용하는 버퍼의 최대 크기
const int BUFFERSIZE = 4096;
