/*
* 本文件功能描述：事件队列(使用链式队列),处理事件
* 关键字：对象池;链式队列(事件队列);生产者-消费者模型;消息队列
*/

#ifndef __EVENT_H
#define __EVENT_H

#include <windows.h>			//GetTickCount获取时间戳
#include <string>			
#include <memory>				//智能指针

enum { START,GOING, WIN, FAIL ,BREAK, PAUSE,RESTART,BACK_TO_MENU};//游戏的状态(开始新游戏，游戏中，胜利，失败，退出，暂停,重新开始，返回主菜单)
extern int gameStatus;//游戏状态


class GameEventSystem;			//前向声明

//事件枚举
enum class GameEventType		//枚举类//避免命名冲突，使用枚举量时需要显示转换
{
	EVENT_NONE = 0,				//无事件(占位符)
	EVENT_SUNSHINE_COLLECTED,	//阳光收集事件
	EVENT_ZOMBIE_SPAWN,			//僵尸生成事件
	EVENT_ZOMBIE_KILLED,		//僵尸死亡事件
	EVENT_PLANT_PLANTED,		//植物种植事件
	EVENT_BULLET_FIRED,			//发射子弹事件
	EVENT_BULLET_HIT,			//子弹命中事件
	EVENT_GAME_START,			//新游戏开始事件
	EVENT_GAME_GOING,			//游戏进行事件
	EVENT_GAME_OVER,			//游戏失败事件
	EVENT_GAME_WIN,				//游戏胜利事件
	EVENT_GAME_PAUSE,			//游戏暂停事件
	EVENT_GAME_BREAK,			//游戏退出事件
	EVENT_PLANT_DESTROYED,		//植物死亡事件
	EVENT_ZOMBIE_EATING,		//僵尸进食事件
	EVENT_ZOMBIE_WALKING,		//僵尸行走事件
	EVENT_PLANTS_REMOVE,		//铲除植物事件
	EVENT_COUNT					//事件总数
};

//链式队列节点(事件节点类)
class GameEventNode				//表示单个游戏事件
{
private:
	GameEventType m_type;		//当前节点事件类型
	int m_param1;			
	int m_param2;
	int m_param3;				//通用参数1，2，3
	unsigned long m_timestamp;	//记录时间发生的事件(单位:ms)
	std::string m_description;	//事件描述
	GameEventNode* m_next;		//指向下一个事件节点的指针
public:
	GameEventNode(GameEventType type = GameEventType::EVENT_NONE,
		int param1 = 0, int param2 = 0, int param3 = 0, const std::string& desc = "");

	//设置事件类型
	void setType(GameEventType type) { m_type = type; }
	//获取事件类型
	GameEventType getType() const { return m_type; }//const：不允许当前方法修改成员变量

	//设置参数1
	void setParam1(int param) { m_param1 = param; }
	//获取参数1
	int getParam1()const { return m_param1; }

	//设置参数2
	void setParam2(int param) { m_param2 = param; }
	//获取参数2
	int getParam2()const { return m_param2; }

	//设置参数3
	void setParam3(int param) { m_param3 = param; }
	//获取参数3
	int getParam3()const { return m_param3; }

	//设置时间戳
	void setTimeStamp(unsigned long timestamp) { m_timestamp = timestamp; }
	//获取时间戳
	unsigned long getTimeStamp() const { return m_timestamp; }

	//设置事件描述
	void setDescription(const std::string& desc) { m_description = desc; }
	//获取事件描述
	std::string getDescription() const { return m_description; }

	//设置next指针
	void setNext(GameEventNode* next) { m_next = next; }
	//获取next指针
	GameEventNode* getNext() const { return m_next; }

	//重置节点状态，在回收节点时清理数据
	void reset();
};


//管理对象池（管理预分配的事件节点）
class GameEventPool{
private:
	GameEventNode* m_pool;		//对象池数组指针(预分配的内存块)
	int m_poolSize;				//对象池大小(预分配的节点数量)
	GameEventNode* m_freeList;	//空闲链表头指针：指向可用节点的链表

	//私有拷贝构造函数（禁止拷贝构造，防止意外复制对象池）
	GameEventPool(const GameEventPool&) = delete;
	//禁止赋值运算符//防止意外赋值对象池
	GameEventPool& operator=(const GameEventPool&) = delete;

public:
	//创建指定大小对象池
	GameEventPool(int size = 30);

	//析构函数//释放对象池内存
	~GameEventPool();

	//从对象池分配一个新节点
	GameEventNode* allocateNode();

	//释放节点回对象池
	void freeNode(GameEventNode* node);

	//获取对象池总大小
	int getSize() const{ return m_poolSize; }

	//估算空闲节点数量
	int getFreeCount() const;
};

//管理事件队列
class GameEventQueue {
private:
	GameEventNode* m_head;		//队列头指针
	GameEventNode* m_tail;		//队列尾指针
	int m_size;					//当前队列节点数量
	int m_maxSize;				//队列最大长度（防止溢出）

	//禁止拷贝构造函数
	GameEventQueue(const GameEventQueue&) = delete;//显示删除拷贝构造函数
	//禁止赋值运算符
	GameEventQueue& operator=(const GameEventQueue&) = delete;

public:
	//创建指定最大容量的队列
	GameEventQueue(int maxSize = 50);

	//清理队列
	~GameEventQueue();

	//入队操作（将节点添加到队列尾部）
	bool enqueue(GameEventNode* node);

	//出队操作（移除节点并返回队列头部节点）
	GameEventNode* dequeue();
	
	//查看操作：返回队列头节点，但是不移除
	GameEventNode* peek() const;

	//清空队列（移除所有的节点）
	void clear();

	//获取当前队列大小
	int getSize() const { return m_size; }

	//检查队列是否为空
	bool isEmpty() const { return m_size == 0; }

	//检查队列是否已满
	bool isFull() const { return m_size >= m_maxSize; }

	//设置队列最大容量
	void setMaxSize(int maxSize) { m_maxSize = maxSize; }

};

//事件队列系统主类（单例模式）
class GameEventSystem		//单例模式确保全局唯一实例
{
private:
		static GameEventSystem* m_instance;//单例实例指针
		GameEventPool m_pool;		//对象池实例
		GameEventQueue m_queue;		//事件队列实例

		//私有构造函数（单例模式核心，防止外部创建实例，确保唯一实例）
		GameEventSystem(int poolSize = 30, int queueSize = 50);

		//禁止拷贝构造函数：防止意外赋值单例
		GameEventSystem(const GameEventSystem&) = delete;
		//禁止赋值运算符：防止意外赋值单例
		GameEventSystem& operator=(const GameEventSystem&) = delete;
public:
	//获取单例实例的静态方法
	static GameEventSystem* getInstance();
	
	//销毁单例实例的静态方法
	static void destroyInstance();

	
	~GameEventSystem();

	//发送事件到队列
	void sendEvent(GameEventType type, int param1 = 0, int param2 = 0, int param3 = 0, const std::string& desc = "");

	//处理所有待处理事件
	void processEvents();

	//获取当前队列大小
	int getQueueSize() const { return m_queue.getSize(); }

	//清空事件队列
	void clearQueue() { m_queue.clear(); }

	//获取对象池大小
	int getPoolSize() const { return m_pool.getSize(); }

	//获取空闲节点数量
	int getFreeNodeCount() const { return m_pool.getFreeCount(); }

	//获取当前游戏状态
	int getGameStatus() const;

	//设置当前游戏状态
	void setGameStatus(int status);
};

//便捷宏定义
//发送事件
#define EVENT_SEND(type,p1,p2,p3,desc)  GameEventSystem::getInstance()->sendEvent(type,p1,p2,p3,desc)

//发送简单事件
#define Event_SEND_SIMPLE(type) \
GameEventSystem::getInstance()->sendEvent(type,0,0,0,#type)	//'\'为续行符，'#'将type转换为字符串

#endif