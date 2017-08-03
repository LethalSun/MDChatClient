#include "pch.h"
#include "TCPNetworkBasic.h"

namespace MDClientNetworkLib
{
	TCPNetworkBasic::TCPNetworkBasic(PacketQueConcurrency* recvPacket, PacketQueConcurrency* sendPacket)
		:_recvPacketQue{ recvPacket },
		_sendPacketQue{ sendPacket }
	{
		initSock();
	}

	TCPNetworkBasic::~TCPNetworkBasic()
	{
		release();

		if (_state != STATE::DISCONNECT)
		{
			//TODO:������ �� �ȵ�(�����峪 ����) ���� �޽��� 
			//�ٵ� �Ƹ� ���� �� �׷���
		}
	}

	CLIENT_ERROR TCPNetworkBasic::initSock()
	{
		//���� �ʱ�ȭ
		WSADATA wsadata;
		auto retval = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (retval != 0)
		{
			return CLIENT_ERROR::SOCKET_INIT_FAIL;
		}

		//���� ����
		_sockFD = socket(AF_INET, SOCK_STREAM, 0);

		if (_sockFD == INVALID_SOCKET)
		{
			return CLIENT_ERROR::SOCKET_ALLOC_FAIL;
		}

		return CLIENT_ERROR::NONE;
	}

	CLIENT_ERROR TCPNetworkBasic::Connect(const char * ipAddr, const short portNum)
	{
		//Ŭ���̾�Ʈ�ϱ� Ŀ��Ʈ ������� ���ε� ���� ���Ʈ
		_state = STATE::CONNECTING;

		SOCKADDR_IN serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = inet_addr(ipAddr);
		serverAddr.sin_port = htons(portNum);

		auto retVal = connect(_sockFD, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
		if (retVal == SOCKET_ERROR)
		{
			return CLIENT_ERROR::CONNECT_ERROR;
		}

		_state = STATE::CONNECTED;
		
		//����� ������ ���� ���ú� �ϱ� ����.
		_threadRecv = std::thread([this]() { ReceiveThreadFunc(); });
		_threadSend = std::thread([this]() { SendThreadFunc(); });

		return CLIENT_ERROR::NONE;
	}

	void TCPNetworkBasic::ReceiveThreadFunc()
	{
		while (true)
		{
			

			//Ŀ�� ���۷� ���� �����͸� �����Ѵ�. ���� �Ҽ� �ִ� ��ŭ
			auto length = receive();

			//���ݿ��� ����
			if (length ==0)
			{
				break;
			}

			if (length < 0)
			{
				//TODO:���� ó��
				/*
				ResultConnectClosedRemotly = -4,
				ResultReciveError = -5,
				ResultNone = -6,
				ResultPacketIsBig = -7,
				*/
				break;
			}
			//����ϳ� ��ŭ ����� ���۸� ����.
			auto err = receiveProcess();

			if (err != CLIENT_ERROR::NONE)
			{
				//TODO:���� ó��
			}

		}
	}

	int TCPNetworkBasic::receive()
	{
		if (_state != STATE::CONNECTED)
		{
			return static_cast<int>(CLIENT_ERROR::NOT_CONNECTED);
		}

		auto receivedLen = recv(_sockFD, &_recvBuffer[_remainDataSize], BUFFERSIZE - _remainDataSize, 0);
		//���۸� �������۷� �����ѵ� ���ú꿡�� �ѹ��� �����ϰ�
		//�����ͷ� ó���ϸ� ���� ����� �Ƴ��� �ִ�.

		if (receivedLen == 0)
		{
			return static_cast<int>(CLIENT_ERROR::CONNECT_CLOSED_REMOTLY);//���� ��Ŵ
		}
		else if (receivedLen > PACKET_HEADER_SIZE + BUFFERSIZE)
		{
			return static_cast<int>(CLIENT_ERROR::PACKET_IS_BIG);
		}
		else if (receivedLen < 0)
		{
			return static_cast<int>(CLIENT_ERROR::RECEIVE_ERROR);
		}

		_remainDataSize += receivedLen;

		return receivedLen;
	}

	CLIENT_ERROR TCPNetworkBasic::receiveProcess()
	{
		auto readPos = 0;
		const auto datasize = _remainDataSize;
		PacketHeder* packetHeader;
		auto packetHeadersize = sizeof(PacketHeder);
		while (datasize - readPos >= packetHeadersize)
		{
			packetHeader = (PacketHeder*)&_recvBuffer[readPos];
			readPos += packetHeadersize;
			auto bodysize = packetHeader->BodySize;
			if (bodysize > 0)
			{
				if (bodysize > (datasize - readPos))
				{
					readPos -= packetHeadersize;
					break;
				}

				if (bodysize > MaxPacketSize)
				{
					return CLIENT_ERROR::PACKET_IS_BIG;
				}
			}

			addRawPacketToQue(packetHeader->Id, bodysize, &_recvBuffer[readPos]);
			readPos += bodysize;
		}

		_remainDataSize -= readPos;

		if (_remainDataSize > 0)
		{
			memcpy(_recvBuffer, &_recvBuffer[readPos], _remainDataSize);
		}
		return CLIENT_ERROR::NONE;
	}

	CLIENT_ERROR TCPNetworkBasic::addRawPacketToQue(const short packetID, const short bodySize, char* bufferStartPos)
	{
		PacketBody packetBody;
		packetBody.PacketBodySize = bodySize;
		packetBody.PacketId = packetID;
		memcpy(packetBody.PacketData, bufferStartPos, bodySize);

		_recvPacketQue->push(packetBody);

		return CLIENT_ERROR::NONE;
	}

	void TCPNetworkBasic::SendThreadFunc()
	{
		while (true)
		{
			PacketBody tempPacket;
			if (_sendPacketQue->try_pop(tempPacket))
			{
				auto packetID = tempPacket.PacketId;

				if (packetID == static_cast<short>(PACKET_ID::END_THREAD))
				{
					break;
				}

				auto dataSize = tempPacket.PacketBodySize;

				char data[MaxPacketSize] = { 0 };

				PacketHeder header{ packetID,dataSize };

				memcpy(&data[0], (char*)&header, sizeof(header));

				if (dataSize != 0)
				{
					memcpy(&data[PACKET_HEADER_SIZE], (char*)tempPacket.PacketData, dataSize);
				}

				send(_sockFD, data, dataSize + sizeof(header), 0);

			}
			
		}
	}

	CLIENT_ERROR TCPNetworkBasic::release()
	{
		_threadRecv.join();
		closesocket(_sockFD);
		_remainDataSize = 0;
		_prevReadPosition = 0;
		_state = STATE::DISCONNECT;

		return CLIENT_ERROR::NONE;
	}



}