#include "event.h"
#include <stdio.h>
#include <string>
#include <easyx.h>
#include "tools.h"
#include <iostream>
#include "AudioManager.h"


int gameStatus;//游戏状态
/*
* GameEventNode类
*/

GameEventNode::GameEventNode(GameEventType type, int param1, int param2, int param3, const std::string& desc)
	:m_type(type), 
	m_param1(param1),
	m_param2(param2), 
	m_param3(param3), 
	m_timestamp(0),
	m_description(desc),
	m_next(nullptr){
}//初始化列表

//重置节点状态
void GameEventNode::reset()
{
	m_type = GameEventType::EVENT_NONE;//无事件
	m_param1 = 0;
	m_param2 = 0;
	m_param3 = 0;
	m_timestamp = 0;
	m_description.clear();	//清空描述字符串
	m_next = nullptr;		//将下一个指针设置为空
}

/*
* GameEventPool类
*/
GameEventPool::GameEventPool(int size)
	:m_pool(nullptr),		//对象池指针初始化为空
	m_poolSize(size),		//设置对象池大小
	m_freeList(nullptr){	//空闲链表初始化为空

	//检查预设对象池大小是否有效
	if (m_poolSize > 0)
	{
		m_pool = new GameEventNode[m_poolSize];//分配对象池内存
		if (!m_pool)
		{
			printf("错误！对象池内存分配失败！\n");
			m_poolSize = 0;
			return;
		}

		//初始化空闲链表，串联链表节点
		for (int i = 0; i < m_poolSize - 1; i++)//遍历除尾节点之外的所有节点
		{
			m_pool[i].setNext(&m_pool[i + 1]);//让当前节点的m_next指向下一个节点
		}
		//设置最后一个节点的m_next指针为nullptr
		m_pool[m_poolSize - 1].setNext(nullptr);

		//设置空闲链表头指向第一个节点
		m_freeList = m_pool;

		printf("对象池初始化完成，大小：%d\n", m_poolSize);
	}
}

GameEventPool::~GameEventPool()
{
	if (m_pool != nullptr)//如果对象池指针不为空
	{
		delete[]m_pool;	//释放对象池数组内存
		m_pool = nullptr;//置空，防止野指针
	}
	m_freeList = nullptr;//置空空闲链表指针
	m_poolSize = 0;		//重置对象池大小为0

	printf("对象池已销毁！\n");
}
	
//分配节点
GameEventNode* GameEventPool::allocateNode()
{
	if (m_freeList == nullptr)//如果空闲链表为空（表示没有空闲节点，那么就没办法分配节点）
	{
		printf("警告：对象池已耗尽！\n");
		return nullptr;		//返回空指针
	}

	//从空闲链表中取出一个节点
	GameEventNode* node = m_freeList;//取出空闲链表的头节点
	m_freeList = m_freeList->getNext();//将头节点移动到当前头节点的下一个节点
	node->reset();//重置当前节点状态，清理可能遗留的旧数据

	return node;//返回分配的节点
}

//释放节点
void GameEventPool::freeNode(GameEventNode* node)//更新空闲链表
{
	if (node == nullptr) return;//若节点为空，直接返回

	//将释放的节点添加到空闲链表的头部
	node->setNext(m_freeList);//被释放节点指向链表头节点
	m_freeList = node;//移动头节点到被释放的节点处
}

//获取当前空闲节点数量
int GameEventPool::getFreeCount() const
{
	int count = 0;
	GameEventNode* cur = m_freeList;

	while (cur)
	{
		count++;
		cur = cur->getNext();	
	}
	return count;
}

/*
* GameEventQueue类
*/
GameEventQueue::GameEventQueue(int maxSize)
	:m_head(nullptr),
	m_tail(nullptr),
	m_size(0),
	m_maxSize(maxSize){
	printf("事件队列初始化完成！最大容量：%d\n",m_maxSize);
}

GameEventQueue::~GameEventQueue()
{
	clear();
	printf("事件队列已销毁!\n");
}

//入队（从队列尾部进入）
bool GameEventQueue::enqueue(GameEventNode* node)
{
	if (node == nullptr)
	{
		printf("错误！尝试入队空节点!\n");
		return false;
	}

	//检查事件队列是否已满
	if (m_size >= m_maxSize)
	{
		printf("事件队列已满，丢弃新事件！\n"); 
		return false;//入队失败
	}

	node->setNext(nullptr);
	//将节点插入到队列尾部
	if (m_tail == nullptr)//如果队列为空
	{
		m_head = node;//!!!!!!!!!!!!!!!!!!!!!!//bug所在
		//忘记设置头结点，导致dequeue方法中,if (m_head == nullptr) ,队列明明不为空，
		// 但由于未设置m_head，m_head一直为nullptr，导致dequeue方法一直返回空指针，出队一直失败！！！
		//但m_queue.isEmpty()，队列实际上又不是空的，故陷入死循环，卡在了GameEventSystem::processEvents函数中无限循环！
														
		m_tail = node;//新节点成为唯一节点
	}
	else
	{
		m_tail->setNext(node);//否则让尾节点指向该节点
		m_tail = node;		//移动尾节点指针
	}

	m_size++;	//队列长度加1
	return true;//入队成功
}

//出队（从队列头部出队）
GameEventNode* GameEventQueue::dequeue()
{
	if (m_head == nullptr)
	{
		return nullptr;//队列为空，返回空指针
	}

	GameEventNode* node = m_head;//取出队首节点
	printf("dequeue:出队前队列长度：%d\n", m_size);
	m_head = m_head->getNext();//移动头节点指针，将头节点设为头节点的下一个节点

	if (!m_head)//如果队列变为空，则更新尾指针为空
	{
		m_tail = nullptr;
	}

	//清空节点的next指针
	node->setNext(nullptr);	//避免该节点仍指向队列中的其他节点
	m_size--;
	printf("dequeue:出队后队列长度：%d\n", m_size);
	return node;			//返回出队的节点
}

//查看队首节点
GameEventNode* GameEventQueue::peek() const
{
	return m_head;	//返回头节点指针
}

//清空队列(不释放节点内存，只将队列置空)
void GameEventQueue::clear()
{
	m_head = nullptr;
	m_tail = nullptr;
	m_size = 0;
}

/*
* GameEventSystem类(单例模式)
*/

GameEventSystem* GameEventSystem::m_instance = nullptr;	//将静态实例指针初始化为空

//私有构造函数
GameEventSystem::GameEventSystem(int poolSize, int queueSize)
	:m_pool(poolSize),
	m_queue(queueSize){
	printf("事件系统：初始化完毕!\n");
}

//获取单例实例的静态方法实现
GameEventSystem* GameEventSystem::getInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new GameEventSystem();		//如果事件系统单例实例还未创建，则创建新实例
	}
	return m_instance;
}

//销毁单例实例的静态方法
void GameEventSystem::destroyInstance()
{
	if (m_instance != nullptr)
	{
		delete m_instance;
		m_instance = nullptr;
		printf("事件系统 ：单例实例已被销毁!\n");
	}
}

GameEventSystem::~GameEventSystem()
{
	//自动调用成员对象的析构函数(m_pool,m_queue)
	printf("事件系统：析构完成!\n");
}

void GameEventSystem::sendEvent(GameEventType type, int param1, int param2, int param3, const std::string& desc)
{
	//首先进行安全检查，确保事件类型是有效的
	if (type >= GameEventType::EVENT_COUNT)//事件类型超出有效范围
	{
		printf("警告！尝试发送无效事件！\n");
		return;
	}
	// 调试信息
	printf("sendEvent: 发送事件类型 %d，当前队列大小: %d\n",
		static_cast<int>(type), m_queue.getSize());

	//然后从对象池分配节点
	GameEventNode* node = m_pool.allocateNode();//分配新节点
	if (node == nullptr)
	{
		printf("错误！分配节点失败！\n");
		return;
	}
	//接着填充事件数据
	node->setType(type);
	node->setParam1(param1);
	node->setParam2(param2);
	node->setParam3(param3);
	node->setTimeStamp(GetTickCount());//设置当前时间戳

	//设置事件描述
	if (!desc.empty())//如果提供了事件描述
	{
		node->setDescription(desc);
	}
	else//如果没提供事件描述，则使用事件类型作为默认事件描述
	{
		const char* typeNames[] = {
			"无事件","阳光收集","僵尸生成","僵尸死亡","植物种植","发射子弹",
			"子弹命中","游戏开始","游戏失败","游戏胜利","植物被毁","僵尸进食"
		};

		int typeIndex = static_cast<int>(type);	//将枚举转换为整数
		if (typeIndex < static_cast<int>(GameEventType::EVENT_COUNT))	//如果事件类型有效
		{
			node->setDescription(typeNames[typeIndex]);
		}
		else
		{
			node->setDescription("未知事件");
		}
	}

	//最后将事件入队
	if (!m_queue.enqueue(node))//如果入队失败
	{
		m_pool.freeNode(node);//回收节点
		printf("警告！事件入队失败，已回收节点！\n");
	}
	else
	{
		printf("sendEvent:入队成功，队列大小：%d\n", m_queue.getSize());
	}
}

void GameEventSystem::setGameStatus(int status)
{
	gameStatus = status;
}

int GameEventSystem::getGameStatus() const
{
	return gameStatus;
}

//处理事件！！！
void GameEventSystem::processEvents()
{
	AudioManager* audio = AudioManager::getInstance();
	static int i;
	while (!m_queue.isEmpty())//如果事件队列不为空时，循环处理
	{
		
		//取出队首事件
		GameEventNode* event = m_queue.dequeue();//从队首取出事件(出队)
		if (!event)//如果事件为空，跳过本次循环
		{
			continue;
		}
		std::cout << "处理第" << ++i << "个事件中，当前事件类型：" << event->getDescription() << std::endl;
		//printf("处理第%d个事件中，当前事件类型：%s\n", ++i,event->getDescription());
		/*
		*	事件类型
			EVENT_NONE = 0,				//无事件(占位符)
			EVENT_SUNSHINE_COLLECTED,	//阳光收集事件
			EVENT_ZOMBIE_SPAWN,			//僵尸生成事件
			EVENT_ZOMBIE_KILLED,		//僵尸死亡事件
			EVENT_PLANT_PLANTED,		//植物种植事件
			EVENT_BULLET_FIRED,			//发射子弹事件
			EVENT_BULLET_HIT,			//子弹命中事件
			EVENT_GAME_START,			//游戏开始事件
			EVENT_GAME_OVER,			//游戏失败事件
			EVENT_GAME_WIN,				//游戏胜利事件
			EVENT_PLANT_DESTROYED,		//植物死亡事件
			EVENT_ZOMBIE_EATING,		//僵尸进食事件
			EVENT_COUNT					//事件总数
		*/
		//根据事件类型执行不同的处理逻辑
		switch (event->getType())//根据事件类型区分
		{
		case GameEventType::EVENT_NONE:break;//无事件
		case GameEventType::EVENT_SUNSHINE_COLLECTED://阳光收集事件
		{
			//播放音效
			//PlaySound("res/audio/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);
			audio->play("res/audio/sunshine.wav", LOOP_NONE, 1, 1600, true);
			break;
		}
		case GameEventType::EVENT_ZOMBIE_SPAWN:break;//僵尸生成事件
		case GameEventType::EVENT_ZOMBIE_KILLED:break;//僵尸死亡事件
		case GameEventType::EVENT_PLANT_PLANTED://植物种植事件
		{
			//PlaySound("res/audio/plantdown.wav", NULL, SND_FILENAME | SND_ASYNC);
			//mciSendString("open res/audio/plantdown.mp3 alias pd", 0, 0, 0);//设置背景音乐
			//mciSendString("setaudio pd volume to 200", 0, 0, 0);//修改音量
			audio->play("res/audio/plantdown.wav", LOOP_NONE, 1, 400, true);
			break;
		}
		case GameEventType::EVENT_BULLET_FIRED://发射子弹事件
		{
			//PlaySound("res/audio/shootpea1.wav", NULL, SND_FILENAME | SND_ASYNC);
			//audio->play("res/audio/shootpea1.wav", LOOP_NONE, 1, 600, true);
			break;
		}	
		case GameEventType::EVENT_BULLET_HIT://子弹命中事件
		{
			//PlaySound("res/audio/peacrush3.wav", NULL, SND_FILENAME | SND_ASYNC);
			//audio->play("res/audio/peacrush3.wav", LOOP_NONE, 1, 600, true);
			break;
		}
		case GameEventType::EVENT_GAME_GOING://游戏进行事件
		{
			gameStatus = GOING;		//当前游戏状态
			printf("游戏继续！\n");
			break;
		}
		case GameEventType::EVENT_GAME_OVER:
		{
			break;//游戏失败事件
		}
		case GameEventType::EVENT_GAME_WIN:
		{
			//audio->play("res/audio/win.mp3", LOOP_NONE, 1, 400, true);
			break;//游戏胜利事件
		}
		case GameEventType::EVENT_PLANT_DESTROYED:break;//植物死亡事件
		case GameEventType::EVENT_ZOMBIE_EATING://僵尸进食事件
		{
			//PlaySound("res/audio/zmeat.wav", NULL, SND_FILENAME | SND_ASYNC);
			//audio->play("res/audio/win.mp3", LOOP_NONE, 1, 400, true);
			break;
		}
		case GameEventType::EVENT_ZOMBIE_WALKING://僵尸行走事件
		{
			break;
		}
		case GameEventType::EVENT_GAME_START://游戏开始事件
		{
			/*audio->play("res/audio/readysetplant.mp3", LOOP_NONE, 1, 1000, true); break;
			Sleep(5000);*/
			//audio->play("res/audio/zombiescoming.mp3", LOOP_NONE, 1, 1000, true); break;
		}
		case GameEventType::EVENT_GAME_BREAK://游戏退出事件
		{
			break;
		}
		case GameEventType::EVENT_PLANTS_REMOVE://铲除植物事件
		{
			audio->play("res/audio/shovel.mp3", LOOP_NONE, 1, 1000, true); break;
			break;
		}
		case GameEventType::EVENT_GAME_PAUSE://游戏暂停事件
		{
			printf("游戏暂停！\n");
			break;
		}
		default:break;//其他事件
		}
		m_pool.freeNode(event);//将处理完的事件节点放回对象池中(放入空闲链表中)(更新空闲链表)
	}
	//更新音频管理器
	audio->update();
}

