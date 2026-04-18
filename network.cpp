#include "network.h"
#include <iostream>
#include <string>
#include <chrono>		//时间相关功能
#include <sstream>		//字符串流，用于序列化

//构造函数，初始化所有成员变量
NetworkManager::NetworkManager() :
	socket_(INVALID_SOCKET),		//初始化为无效套接字
	isConnected_(false),			//初始未连接
	isServer_(false),				//初始非服务器状态
	port_(0),						//端口初始化为0
	recvBufferPos_(0),				//接收缓冲区位置从0开始
	threadRunning_(false),			//接收线程未运行
	nextSeqNum_(1){					//序列号从1开始
	memset(recvBuffer_, 0, sizeof(recvBuffer_));//清空接收缓冲区
	serverIP_ = "127.0.0.1";		//默认本地回环地址
}

//析构函数
NetworkManager::NetworkManager()
{
	disconnect();		//断开连接，清理所有资源
}

//初始化winsock，返回成功或失败
bool NetworkManager::initializeWinsock()
{
	WSADATA wsaData;		//winsock数据结构，包含版本信息

	//启动winsock,请求2.2版本？？？？？？？？？？
	//WSAStartup初始化Windows socket DLL,参数1：版本号，参数2：返回的WSA数据
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)	//如果返回值不为0，表示初始化失败
	{
		std::cerr << "WSAStartup failed: " << result << std::endl;
		return false;	//返回失败
	}

	//检查Winsock版本是否符合要求
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		std::cerr << "找不到Winsock.dll的可用版本" << std::endl;
		WSACleanup();		//清理winsock资源
		return false;		//返回错误
	}

	std::cout << "Winsocket initialized successfully." << std::endl;
	return true;//初始化成功
}

//作为服务器启动，监听指定端口
bool NetworkManager::startAsServer(int port)
{
	//创建TCP套接字
	//AF_INET:IPv4地址族
	//SOCK_STREAM:流式套接字（TCP）
	//IPPROTO_TCP:TCP协议
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_ == INVALID_SOCKET)//检查套接字是否创建成功
	{
		std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
		return false;//创建失败
	}

	//设置套接字选项，允许地址重用
	//避免“Address already in use”错误
	int opt = 1;
	if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0)
	{
		std::cerr << "Failed to set socket options" << std::endl;
		closesocket(socket_);	//关闭套接字
		return false;			//返回失败
	}

	//设置服务器地址结构
	sockaddr_in serverAddr;		//IPv4地址结构
	serverAddr.sin_family = AF_INET;//地址族：IPv4
	serverAddr.sin_port = htons(port);//端口号，htons转换为网络字节序
	serverAddr.sin_addr.s_addr = INADDR_ANY;//绑定到所有可用接口

	//绑定套接字到指定地址和端口
	if (bind(socket_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		std::cerr << "Bind failed:" << WSAGetLastError() << std::endl;
		closesocket(socket_);//关闭套接字
		return false;		//返回失败
	}
	
	//开始监听，最大等待队列长度为5
	if (listen(socket_, 5) == SOCKET_ERROR)
	{
		std::cerr << "Listen failed:" << WSAGetLastError() << std::endl;
		closesocket(socket_);//关闭套接字
		return false;		//返回失败
	}

	std::cout << "服务启动，等待连接到端口 " << port << std::endl;

	//设置为阻塞模式，等待客户端连接
	//设置接收超时时间：5秒
	DWORD timeout = 5000;	//5秒超时
	setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

	sockaddr_in clientAddr;		//客户端地址结构
	int clientAddrLen = sizeof(clientAddr);//地址结构长度

	//接受客户端连接
	SOCKET clientSocket = accept(socket_, (sockaddr*)&clientAddr, &clientAddrLen); 
	if (clientSocket == INVALID_SOCKET)//检查是否接受成功
	{
		std::cerr << "accepted failed:" << WSAGetLastError() << std::endl;
		closesocket(socket_);	//关闭监听套接字
		return false;			//返回失败
	}

	//关闭监听套接字，只使用客户端套接字进行通信
	closesocket(socket_);		//关闭监听套接字
	socket_ = clientSocket;		//使用客户端套接字

	//设置套接字为非阻塞模式，以便后续使用
	u_long mode = 1;			//1：非阻塞模式
	ioctlsocket(socket_, FIONBIO, &mode);

	//存储连接信息
	char ipStr[INET_ADDRSTRLEN];	//IP地址字符串缓冲区
	inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);//转换IP为字符串
	remoteIP_ = ipStr;				//保存远程IP
	remotePort_ = ntohs(clientAddr.sin_port);	//保存远程端口，ntohs转化为主机字节序

	isConnected_ = true;			//设置连接状态为已连接
	isServer_ = true;				//设置为服务器模式
	port_ = port;					//保存端口号

	//启动接收线程
	threadRunning_ = true;			//设置线程运行状态
	recvThread_ = std::thread(&NetworkManager::receiveThreadFunc, this);//创建并启动线程

	std::cout << "Client connected from " << remoteIP_ << ":" << remotePort_ << std::endl;
	return true;					//启动成功
}

//作为客户端连接到服务器
bool NetworkManager::connectToServer(const std::string& ip,int port)
{ 
	//创建TCP套接字
	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_ == INVALID_SOCKET)//检查套接字创建是否成功
	{
		std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;
		return false;//创建失败
	}

	//设置服务器地址结构
	sockaddr_in serverAddr;			//服务器地址结构
	serverAddr.sin_family = AF_INET;//IPv4地址族
	serverAddr.sin_port = htons(port);//服务器端口，转换为网络字节序

	//将IP地址字符串转换为二进制格式
	if (inet_pton(AF_INET, ip.c_str(), &serverAddr.sin_addr) <= 0)
	{
		std::cerr << "Invalid address: " << ip << std::endl;
		closesocket(socket_);//关闭套接字
		return false;		//返回失败
	}

	std::cout << "Connecting to server " << ip << ":" << port << std::endl;

	//连接到服务器，设置连接超时
	//设置连接超时时间：3秒

	DWORD timeout = 3000;
	setsockopt(socket_, SOL_SOCKET, SO_SNDTIMEO, (const char*)&timeout, sizeof(timeout));

	//尝试连接服务器
	if (connect(socket_, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		int error = WSAGetLastError();	//获取错误码
		std::cerr << "Connect failed :" << error << std::endl;
		closesocket(socket_);//关闭套接字

		//根据错误码给出具体提示
		if (error == WSAECONNREFUSED)
		{
			std::cerr << "Connection refused.Is the server running?" << std::endl;
		}
		else if (error == WSAETIMEDOUT)
		{
			std::cerr <<
		}
	}
}

	

