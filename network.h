#ifndef __NETWORK_H_
#define __NETWORK_H_

#include <winsock2.h>//windows socket API头文件
#include <WS2tcpip.h>//windows socket IP相关头文件
#pragma comment(lib,"ws2_32.lib")	//链接windows socket库

#include <string>
#include <queue>
#include <mutex>	//互斥锁，用于线程同步
#include <thread>	//线程支持

//消息类型枚举，定义网络通信中的不同消息类型
enum MessageType
{
	MSG_CONNECT_REQUEST,	//连接请求消息
	MSG_GAME_START,			//游戏开始消息
	MSG_PLANT_ACTION,		//种植植物消息
	MSG_ZOMBIE_ACTION,		//释放僵尸消息
	MSG_SUNSHINE_UPDATE,	//更新阳光消息
	MSG_GAME_STATE,			//游戏开始消息
	MSG_GAME_OVER,			//游戏结束消息
	MSG_HEARTBEAT,			//心跳包，检测连接状态
	MSG_DISCONNECT,			//断开连接消息
};

//网络消息头结构体，每个网络消息都以此开头
struct MessageHeader
{
	MessageType msgType;	//消息类型
	uint32_t msgSize;		//消息体大小
	uint32_t seqNum;		//系列号，用于消息顺序和丢包检测
	uint32_t timestamp;		//时间戳，用于延迟计算
};

//连接请求消息结构体，用于建立连接时交换信息
struct ConnectRequest
{
	char playerName[32];	//玩家名称
	uint8_t playerRole;		//角色，0:植物方;1:僵尸方
	uint32_t version;		//游戏版本
};


//植物动作消息结构体，当植物方种植植物时发送
struct plantAction
{
	int plantType;			//植物类型(豌豆射手=0，……)
	int row;				//行号0-4
	int col;				//列号0-8
	uint32_t plantID;		//植物唯一ID
};

//僵尸动作消息结构体，当玩家放置僵尸时发送
struct zombieAction
{
	int zombieType;			//僵尸类型
	int row;				//行号
	float startX;			//起始x坐标
	uint32_t zombieID;		//僵尸唯一ID
};

//阳光更新消息结构体，同步阳光数量变化
struct sunshineUpdate
{
	int sunshineAmount;		//当前阳光数量
	int changeAmount;		//阳光的变化量（增加或减少）
};

//游戏状态同步消息结构体，定期发送完整的游戏状态
struct gameState
{
	uint32_t frameNumber;	//帧号，用于状态同步
	int plantCount;			//植物数量
	int zombieCount;		//僵尸数量
	int sunshine;			//当前阳光数

	//goto,植物和僵尸详细信息
};

//网络管理类，封装所有网络操作
class NetworkManager
{
private:
	SOCKET socket_;			//套接字句柄，用于网络通信
	bool isConnected_;		//连接状态标志
	bool isServer_;			//是否为服务器模式
	std::string serverIP_;	//服务器IP地址(客户端模式使用)
	int port_;				//端口号

	//接收缓冲区相关
	char recvBuffer_[4096];	//接收缓冲区，4096字节
	int recvBufferPos_;		//接收缓冲区当前位置

	//消息队列和线程相关
	std::queue<std::string> messageQueue_;//接收到的消息队列
	std::mutex queueMutex_;	//消息队列互斥锁，防止多线程冲突
	std::thread recvThread_;//接收线程，独立处理网络接收
	bool threadRunning_;	//线程运行标志，用于控制线程退出

	//连接信息
	std::string remoteIP_;	//远程IP地址
	int remotePort_;		//远程端口

	//序列号管理	
	uint32_t nextSeqNum_;	//下一个消息序列号

public:
	NetworkManager();

	~NetworkManager();

	//初始化winsock，必须在其他网络操作前调用
	bool initializeWinsock();

	//作为服务器启动，监听指定端口
	bool startAsServer(int port);

	//作为客户端连接到服务器
	bool connectToServer(const std::string& ip, int port);

	//发送消息，将消息结构体序列化并发送
	bool sendMessage(MessageType type, const void* data = nullptr, uint32_t dataSize = 0);

	//接收消息，从消息队列中取出一个消息
	bool receiveMessage(std::string& msg);

	//检查是否连接
	bool isConnected() const { return isConnected_; };

	//获取连接信息
	std::string getRemoteInfo() const { return remoteIP_ + ":" + std::to_string(remotePort_); }

	//断开连接
	void disconnect();

private:
	//接收线程函数，独立运行接收网络数据
	void receiveThreadFunc();

	//处理接收到的数据，解析为完整消息
	void processReceivedData(const char* data, int length);

	//创建消息包，将消息头和消息体组合成完整包
	std::string createPacket(MessageType type, const void* data, uint32_t dataSize);

	//解析消息头，从数据中提取消息头信息
	bool parseHeader(const char* data, MessageHeader& header);

	//序列化函数，将各种结构体转换为字节流
	template<typename T>
	std::string serialize(const T& obj);

	//反序列化函数，将字节流转换为结构体
	template<typename T>
	T deserialize(const std::string& data);
};

#endif