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
//���� ��Ŷ�� �ִ� ũ��
const int MaxPacketSize =4096;

//���� ����ϴ� ������ �ִ� ũ��
const int BUFFERSIZE = 4096;
