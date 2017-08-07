#pragma once
#include "pch.h"
namespace MDClientNetworkLib
{
	using PacketQueConcurrency = concurrency::concurrent_queue<PacketBody>;
	enum class STATE
	{
		DISCONNECT = 0,
		CONNECTING = 1,
		CONNECTED = 2,
	};

	class TCPNetworkBasic
	{
	public:
		TCPNetworkBasic(PacketQueConcurrency* recvPacket, PacketQueConcurrency* sendPacket);
		~TCPNetworkBasic();

		CLIENT_ERROR Connect(const char* ipAddr, const short portNum);
		
		void Disconnect();
	private:
		CLIENT_ERROR initSock();

		void ReceiveThreadFunc();

		int receive();

		CLIENT_ERROR addRawPacketToQue(const short packetID, const short bodySize,char* bufferStartPos);

		CLIENT_ERROR receiveProcess();

		CLIENT_ERROR release();

		void SendThreadFunc();

		STATE _state{ STATE::DISCONNECT };
		
		SOCKET _sockFD = INVALID_SOCKET;

		PacketQueConcurrency* _recvPacketQue;

		PacketQueConcurrency* _sendPacketQue;
		std::thread _threadRecv;
		std::thread _threadSend;
		//리시브시 필요한정보
		int _remainDataSize = 0;
		int _prevReadPosition = 0;
		char _recvBuffer[BUFFERSIZE];
	};

}

