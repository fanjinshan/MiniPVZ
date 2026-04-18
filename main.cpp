/***************************************************************************
 * 项目名称：植物大战僵尸(学习版)
 *
 * 项目描述：
 * 基于EasyX图形库实现的植物大战僵尸简化版游戏，包含植物种植、僵尸攻击、
 * 阳光收集、子弹射击等核心玩法。
 * 总代码量：2200行左右
 * 
 * 开发环境：
 * - 操作系统：Windows 11
 * - 开发工具：Visual Studio 2022
 * - 图形库：EasyX 2024
 * - 语言：C++
 * - 编译标准：C++11
 *
 * 主要功能：
 * 实现三种基本植物：豌豆射手、向日葵、坚果墙(其余待实现)
 * 2. 实现三种僵尸：普通僵尸、路障僵尸、铁桶僵尸(其余待实现)
 * 3. 实现阳光系统：随机生成 + 向日葵生产
 * 4. 实现碰撞系统：子弹-僵尸、僵尸-植物
 * 5. 实现游戏状态管理：开始菜单、游戏进行、游戏结束
 * 
 * 文件结构：
 * main.cpp     - 主程序文件
 * event.cpp/h  - 事件系统文件
 * tools.h/cpp  - 工具函数（PNG图片处理）
 * vector2.h/cpp    - 向量计算类
 * AudioManager.h/cpp - 音频处理文件
 * res/         - 资源文件夹（图片、音频）
 *
 * 开发者信息：
 * - 开发者：樊锦山
 *
 * 时间信息：
 * - 创建时间：2026年1月5日
 * - 最后修改：2026年1月11日
 * - 项目周期：2026年1月5日-至今
 *
 * 版权声明：
 * 本项目仅供学习交流使用，基于原版植物大战僵尸游戏进行简化实现。
 * 所有游戏素材版权归PopCap Games所有，请勿用于商业用途。
 *
 * 文件说明：
 * 此文件为游戏核心功能实现，包含游戏初始化、主循环、渲染更新等核心逻辑。
 *
 * 版本历史：
 * v1.0.0 (2026-1-11) 基础功能实现
 *   - 实现植物种植、僵尸移动、阳光收集等核心功能
 *   - 完成碰撞检测系统
 *   - 添加音效和动画效果
 *
 ***************************************************************************/

/**
* 开发日志
* 核心功能实现：
* 1.创建新项目
* 2.导入素材
* 3.实现游戏场景
* 4.实现游戏顶部的工具栏
* 5.实现工具栏中的植物卡牌
* 6.实现拖动植物卡牌
* 7.实现种植植物卡牌
* 8.实现植物摆动
* 9.制作启动菜单
* 10.随机生成阳光
* 11.显示阳光
* 12.收集阳光
* 13.创建僵尸
* 14.移动僵尸，添加游戏结束条件
* 15.实现僵尸的动作
* 16.创建豌豆子弹
* 17.移动,渲染豌豆子弹
* 18.实现豌豆子弹和僵尸的碰撞检测
* 19.实现子弹爆炸效果
* 20.实现僵尸死亡动画
* 21.实现僵尸的进食动作
* 22.阳光球收集动画
* 23.向日葵生产阳光
* 24.开始过场动画
* 25.开场卡槽向下移动动画
* 26.判断游戏结束
* 27.游戏结束动画，背景音乐
* 28.添加铲子铲除植物功能
* 29.添加主菜单退出游戏功能
* 30.添加开场动画，音效
* 31.实现土豆雷完整功能
* 进阶功能：
* 1.实现事件队列系统(统一管理游戏事件)⬅
* 2.开始菜单->冒险模式、联机模式
* 冒险模式->选关->进入游戏
* 联机模式->选择植物方、僵尸方->进入游戏
*/ 

/*
* 待解决bug：
* 1.收集阳光时会卡顿√
* 2.帧率过低√
* 3.豌豆子弹异常消失√
* 4.当阳光在回收的过程中产生了另一个阳光时，正在回收的阳光会消失√
* 5.有时铁桶僵尸吃完植物后会异常停留在原地持续进食动作，直到种个新植物给它吃√
* 6.阳光掉落位置，植物种植位置，卡槽位置，游戏结束判断位置√
* 7.有时子弹会穿过前一个僵尸攻击后面的僵尸√
* 8.音效播放异常√
* 9.偶尔会有僵尸在没有植物的地方原地进食√
* 10.GameEventQueue::enqueue中未设置m_head = node;
* //!!!!!!!!!!!!!!!!!!!!!!//bug所在
* //忘记设置头结点，导致dequeue方法中,if (m_head == nullptr) ,队列明明不为空，
* // 但由于未设置m_head，m_head一直为nullptr，导致dequeue方法一直返回空指针，出队一直失败！！！
* //但m_queue.isEmpty()，队列实际上又不是空的，故陷入死循环，卡在了GameEventSystem::processEvents函数中无限循环！√
* 11.植物会种植到第10列，并在第10列显示，而实际上植物的位置在该行的下一行的第一列，子弹也从下一行第一列的位置发出，
* 僵尸也会在下一行的第一列攻击植物
*/

/*
* 可优化：
* 1.帧率过低
* 2.分离植物血量√
* 3.添加音效
* 4.添加关卡
* 5.添加植物√
* 6.添加僵尸√
* 7.添加场景
* 8.添加道具
* 9.添加铲除植物√
* 10.添加退出游戏√重开游戏
* 11.背景音乐等√
* 12.植物，僵尸，子弹速度
*/

#include <stdio.h>
#include <graphics.h>	//easyx图形库头文件，需要安装easyx图形库
#include "tools.h"//处理图片黑色背景
#include <time.h>//作为随机数种子
#include "vector2.h"//向量计算
#include <math.h>//atan，cos，sin函数

#include "event.h"//事件队列系统

//音效头文件
#include <mmsystem.h>
#include <windows.h>
#pragma comment(lib,"winmm.lib")
#include "AudioManager.h"//音效处理头文件

#define WIN_WIDTH  900	//游戏窗口大小
#define WIN_HEIGHT 600
#define GROUND_TO_WIN 135 //背景图到游戏窗口左边界的距离(越接近0，离窗口越近)
#define ZM_MAX 300	//僵尸的最大数量

enum {WAN_DOU,XIANG_RI_KUI,JIAN_GUO,TU_DOU_LEI,DOUBLE_WAN_DOU,ICE_WAN_DOU,DA_ZUI_HUA,ZHI_WU_COUNT};//植物种类
enum { SUNSHINE_DOWN, SUNSHINE_GROUND, SUNSHINE_COLLECT, SUNSHINE_PRODUCT };//阳光的状态
enum {RegularZM,RoadBlockZM,BucketZM,SmallZM,RugbyZM,IronGateZM,ZMType};//僵尸种类
enum {TU_DOU_LEI_Under,TU_DOU_LEI_Ready,DA_ZUI_HUA_EAT,Plants_Status};//土豆雷状态
enum GameAction	{ACTION_START_NEW_GAME,ACTION_RESTART_GAME,ACTION_BACK_TO_MENU,ACTION_EXIT};//游戏的状态(2)
enum {NORMAL_PEA,ICE_PEA,PEA_TYPE};//豌豆子弹状态
GameAction nextAction = ACTION_START_NEW_GAME;//游戏状态枚举变量
IMAGE imgBg;	//表示背景图片
IMAGE imgBar;	//工具栏图片
IMAGE imgShovel;//铲子图片
IMAGE imgShovelBg;//铲子背景图
IMAGE imgPause;	//暂停按钮
IMAGE imgPauseMenu;//暂停菜单按钮
IMAGE imgCards[ZHI_WU_COUNT];	//植物数组（存放植物图片）
IMAGE* imgZhiWu[ZHI_WU_COUNT][20];	//存储植物在拖动过程中的图片以及种植后植物的动图，
									//因为每个植物动图所含静图的数量不等，但不超过20张，所以使用指针数组
									//共ZHI_WU_COUNT行，每行20个指向图片的指针，多余的指针置为空即可
IMAGE* imgPlantsStatus[Plants_Status][17];//植物独特状态的图片数组
IMAGE imgBoom[2];//土豆雷爆炸图
IMAGE imgReady, imgSet, imgPlant;	//开始，安放，植物
bool showStartSequence = false;		//是否正在显示开始动画
int startSequenceIndex = 0;			//当前显示哪张图片
DWORD startSequenceStartTime = 0;	//开始显示开始动画的时间

int curX, curY;	//当前选中植物，在移动过程中的实时坐标
int curZhiWu; //0:没有选中，1：选中第一种植物，2：选中第二种植物……



/*
* 植物
*/
struct zhiwu//先初始化
{
	int type;//0:没有植物 1:第一种植物
	int frameIndex;//序列帧的序号
	int deadTime;//死亡计数器(植物血条)

	int timer;
	int x, y;

	int shootTime;//发射的间隔时间(每隔一段时间发射)
	int width;
	int status;	//植物状态(土豆雷)
	int prepareTime;//准备时间(土豆雷)
	int boom;//土豆雷
	bool isEating;//大嘴花
};

struct zhiwu plants[5][9];		//五行九列的植物

/*
* 阳光
*/
struct sunshineBall//阳光对象
{
	int x, y;//阳光坐标,阳光球在飘落过程中x不变，只有y变化
	int frameIndex;//阳光是旋转的，该变量表示当前显示图片帧的序号
	int destY;//飘落的目标位置的y坐标
	bool used;//是否在使用，0：可用，1：已被使用，不可用
	int timer;//定时器

	float xoff;//偏移量
	float yoff;

	float t;//贝塞尔曲线的时间点 0..1
	vector2 p1, p2, p3, p4;
	vector2 pCur;//当前时刻阳光球的位置
	float speed;//速度
	int status;//当前状态（共四种状态） 
};

struct sunshineBall balls[20];//需要频繁使用阳光，使用池的概念(数组)
IMAGE imgSunShineBall[29];//阳光图片数组，存储加载图片
int sunshine;//阳光值

/*
* 僵尸
*/
struct zm	//僵尸结构体
{
	int type;//僵尸的种类
	float x, y;//位置
	int frameIndex;//图片帧
	bool used;//是否已经登场
	float speed;//速度
	int row;//所在行
	int blood;//血量
	bool dead;//是否死亡
	bool eating; // 是否正在吃植物
	int width;//僵尸的宽度
	int eatingRow;//正在吃的植物的行
	int eatingCol;//正在吃的植物的列
};
struct zm zms[50];	//50个僵尸
IMAGE* imgZM[ZMType][23];//僵尸走路动画
IMAGE imgZMDead[20];//僵尸死亡动画
IMAGE* imgZMEat[ZMType][21];//僵尸进食动画
IMAGE imgZMStand[11];//僵尸站立动画
//struct zm zmsS[20]; //20个小鬼僵尸

//IMAGE imgBlockZM[21];//路障僵尸
//IMAGE imgSmallZM[12];//小鬼僵尸
int zmMax = sizeof(zms) / sizeof(zms[0]);				//“池”中所有僵尸的数量
int killCount;//当前击杀的僵尸数量
int zmCount;//已经出现的僵尸数量


/*
* 豌豆子弹
*/
struct bullet
{
	int x, y;
	int row;
	bool used;
	int speed;
	bool blast;//是否发生爆炸
	int frameIndex;//帧序号
	int damage;//子弹伤害
	int type;//子弹类型
};
struct bullet bullets[100];//子弹池
IMAGE imgBulletNormal[PEA_TYPE];//子弹的一般状态
IMAGE imgBulletBlast[4];//子弹爆炸图 imgBallBlast

/*
* 铲子
*/
struct 
{
	int x;
	int y;
	bool isSelected;//是否被选中
}shovel = {753,-5,false};//铲子结构体对象

//渲染字体（阳光数量）
char scoreText[8];
/*
* @brief:判断文件是否存在
* @argument：name :文件名
* @return：存在返回true
*		   不存在返回false
*/
bool fileExist(const char* name)	//文件流
{
	FILE* fp = fopen(name, "r");
	if (fp)
	{
		fclose(fp);//判断完文件存在就关闭文件
		return true;
	}
	else
		return false;
}

void gameInit()
{
	//加载游戏背景图片
	//把字符集修改为多字节字符集
	loadimage(&imgBg, "res/map1.jpg");//初始化图片（每张都需要）
	loadimage(&imgBar, "res/bar5.png");
	loadimage(&imgShovelBg, "res/shovelSlot.png");
	loadimage(&imgShovel, "res/shovel.png");
	loadimage(&imgPause, "res/pause.png");
	loadimage(&imgPauseMenu, "res/pauseMenu.png");
	memset(imgZhiWu, 0, sizeof(imgZhiWu));	//将imgZhiWu数组中的指针全部置零（初始化）
	memset(plants, 0, sizeof(plants));//初始化zhiwu数组（种植植物）

	killCount = 0;			//当前击杀数
	zmCount = 0;			//当前已经出现的僵尸数量
	gameStatus = GOING;		//初始化游戏状态
	Event_SEND_SIMPLE(GameEventType::EVENT_GAME_GOING);
	//初始化植物卡牌
	char name[64];
	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		//生成植物卡牌文件名
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);
		for (int j = 0; j < 20; j++)
		{
			//生成移动过程中的图片以及植物动图的文件名
			sprintf_s(name, sizeof(name), "res/Plants/%d/%d.png", i , j + 1);//每个植物文件夹中(i)，有多张图片(j)
			//首先判断文件是否存在
			if (fileExist(name))
			{
				imgZhiWu[i][j] = new IMAGE;//此时imgZhiWu中的指针都是空，要先分配内存
				//IMAGE是类，要用new分配动态内存而不是malloc
				loadimage(imgZhiWu[i][j], name);	//把文件名为name的文件依次存到imgZhiWu中指针指向的IMAGE变量中
			}
			else
				break;
		}
	}
	curZhiWu = 0;//当前未选中植物
	sunshine = 1000;//阳光值

	//阳光
	memset(balls, 0, sizeof(balls));//初始化阳光球数组（阳光池）
	for (int i = 0; i < 29; i++)
	{
		//生成阳光球每一帧的文件名
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunShineBall[i], name);//加载每个图片帧
	}

	//配置随机种子
	srand(time(0));

	//创建游戏图形窗口
	initgraph(WIN_WIDTH, WIN_HEIGHT);//加个1会保留控制台界面

	//设置字体
	LOGFONT f;//字体对象
	gettextstyle(&f);
	f.lfHeight = 30;
	f.lfWeight = 15;
	strcpy(f.lfFaceName, "Segoe UI Black");
	f.lfQuality = ANTIALIASED_QUALITY;//抗锯齿效果
	settextstyle(&f);//设置字体
	setbkmode(TRANSPARENT);//设置背景模式（背景透明）
	setcolor(BLACK);//设置字体颜色

	//初始化普通僵尸数组
	memset(zms, 0, sizeof(zms));
	//初始化僵尸吃和走的动画数组
	memset(imgZM, 0, sizeof(imgZM));
	memset(imgZMEat, 0, sizeof(imgZMEat));
	
	for (int i = 0; i < ZMType; i++)//加载三种僵尸正常走路的帧画面
	{
		for(int j = 0; j < 23; j++)
		{
			sprintf_s(name, sizeof(name), "res/zm/%d/%d.png", i ,j + 1);
			if (fileExist(name))//如果图片存在，则加载到imgZM数组中
			{
				imgZM[i][j] = new IMAGE;
				loadimage(imgZM[i][j], name);
				//printf("加载imgZM[%d][%d]张图片成功\n", i, j + 1);
			}
			else
				break;
		}
	}

	//初始化僵尸进食动画数组
	for (int i = 0; i < ZMType; i++)
	{
		for (int j = 0; j < 22; j++)
		{
			sprintf_s(name, sizeof(name), "res/zm_eat/%d/%d.png", i, j + 1);
			if (fileExist(name))//如果图片存在，则加载到imgZMEat数组中
			{
				imgZMEat[i][j] = new IMAGE;
				loadimage(imgZMEat[i][j], name);
				//printf("加载imgZMEat[%d][%d]张图片成功\n", i, j + 1);
			}
			else
				break;
		}
	}
	

	//初始化僵尸死亡动画数组
	for (int i = 0; i < 20; i++)
	{
		sprintf_s(name, sizeof(name), "res/zm_dead/ZombieDust/%d.png", i + 1);
		loadimage(&imgZMDead[i], name);
	}
	
	//初始化小鬼僵尸数组
	//memset(zmsS, 0, sizeof(zmsS));
	//for (int i = 0; i < 12; i++)//加载小鬼僵尸图片帧
	//{
	//	sprintf_s(name, sizeof(name), "res/zm/3/%d.png", i + 1);
	//	loadimage(&imgSmallZM[i], name);
	//}

	//加载豌豆类型
	for (int i = 0; i < PEA_TYPE; i++)
	{
		sprintf_s(name, sizeof(name), "res/bullets/PeaNormal/PeaNormal_%d.png", i);
		loadimage(&imgBulletNormal[i], name);//加载子弹图片
	}
	memset(bullets, 0, sizeof(bullets));//初始化子弹池

	//初始化豌豆子弹帧图片数组(爆炸)
	loadimage(&imgBulletBlast[3], "res/bullets/PeaNormalExplode/PeaNormalExplode_0.png");
	for (int i = 0; i < 3; i++)
	{
		float k = (i + 1) * 0.25;//比例
		//由于只有一张爆炸图片，所以分四次，每次等比例放大图片，0.25，0.5，0.75，1.0
		loadimage(&imgBulletBlast[i], "res/bullets/PeaNormalExplode/PeaNormalExplode_0.png",
			imgBulletBlast[3].getwidth()*k,imgBulletBlast[3].getheight()*k,true);
	}

	for (int i = 0; i < 11; i++)//加载僵尸站立动画
	{
		sprintf_s(name, sizeof(name), "res/zm_stand/0/%d.png", i + 1);
		loadimage(&imgZMStand[i], name);
	}

	/*shovel.x = 753;
	shovel.y = -5;
	shovel.isSelected = 0;*/

	for (int i = 0; i < Plants_Status; i++)//加载土豆雷状态
	{
		for (int j = 0; j < 17; j++)
		{
			sprintf_s(name, sizeof(name), "res/Plants/status/%d/%d.png", i, j + 1);
			if (fileExist(name))
			{
				imgPlantsStatus[i][j] = new IMAGE;
				loadimage(imgPlantsStatus[i][j], name);
			}
			else
				break;
		}
	}



	loadimage(&imgBoom[0], "res/Plants/status/boom/boom1.png");
	loadimage(&imgBoom[1], "res/Plants/status/boom/boom2.png");
}

void getZombieRenderPos(int type, int x, int y, int& renderX, int& renderY)
{
	renderX = x;
	renderY = y;

	switch (type)
	{
	case RegularZM:
	case BucketZM:
	case IronGateZM:
		renderY = y - 144;
		break;
	case RoadBlockZM:
		renderX = x - 20;
		renderY = y - 144;
		break;
	case SmallZM:
		renderX = x + 100;
		renderY = y - 80;
		break;
	case RugbyZM:
		renderY = y - 150;
		break;
	default:
		renderY = y - 144;
		break;
	}
}

void drawZM()//渲染僵尸
{
	//渲染僵尸
	for (int i = 0; i < zmMax; i++)
	{
		if (zms[i].used)
		{
			int zmsType = zms[i].type;
			int index = zms[i].frameIndex;

			int renderX, renderY;
			getZombieRenderPos(zmsType, zms[i].x, zms[i].y, renderX, renderY);

			if (zms[i].dead)
			{
				putimagePNG(zms[i].x, zms[i].y - 144, &imgZMDead[index]);
			}
			else if (zms[i].eating)
			{
				putimagePNG(renderX, renderY, imgZMEat[zmsType][index]);
			}
			else
			{
				putimagePNG(renderX, renderY, imgZM[zmsType][index]);
			}

			//IMAGE* img = &imgZM[zms[i].frameIndex];
			//IMAGE* img = (zms->dead) ? imgZMDead : imgZM;//如果僵尸已死，则img指向死亡动画数组，否则指向行走动画的数组
			//IMAGE* img = NULL;
			//if (zms[i].dead) img = imgZMDead;//如果僵尸已死，则img指向死亡动画数组
			//else if (zms[i].eating) img = imgZMEat;//如果在进食，img指向进食动画数组
			//else img = imgZM;//否则正常行走动画
			//
			//img += zms[i].frameIndex;//指针偏移，指向相应的对象
			//putimagePNG(zms[i].x, zms[i].y - img->getheight(), img);
		}
	}
}

void drawSunshines()//绘制阳光
{
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		if (balls[i].used || balls[i].xoff)
		{
			IMAGE* img = &imgSunShineBall[balls[i].frameIndex];
			//putimagePNG(balls[i].x, balls[i].y, img);
			putimagePNG(balls[i].pCur.x, balls[i].pCur.y, img);
		}
	}
}

void drawSunshineNum()//渲染阳光数量
{
	sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
	outtextxy(291 - GROUND_TO_WIN, 68, scoreText);//设置阳光数量的位置
}

void drawCards()//渲染植物卡牌
{
	for (int i = 0; i < ZHI_WU_COUNT; i++)
	{
		int x = 350 - GROUND_TO_WIN + i * 70;	//各卡牌的x坐标，图片为64*90像素
		int y = 5;				//y坐标
		putimagePNG(x, y, &imgCards[i]);	//依次渲染植物卡牌图片
	}
}

void drawZhiWu()//渲染植物
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (plants[i][j].type > 0)
			{
				/*int x = 256 + j * 80;
				int y = 75 + i * 100 + 20;*/
				int zhiWuType = plants[i][j].type - 1;//获取当前选中的植物
				int index = plants[i][j].frameIndex;//获取当前植物的序列帧
				//putimagePNG(x, y, imgZhiWu[zhiWuType][index]);
				if (plants[i][j].type == TU_DOU_LEI + 1)
				{
					if (plants[i][j].status == TU_DOU_LEI_Under)
					{
						zhiWuType = TU_DOU_LEI_Under;
					}
					else if (plants[i][j].status == TU_DOU_LEI_Ready)
					{
						zhiWuType = TU_DOU_LEI_Ready;
					}
					putimagePNG(plants[i][j].x, plants[i][j].y + 20, imgPlantsStatus[zhiWuType][index]);
				}
				else if (plants[i][j].type == DA_ZUI_HUA + 1 && plants[i][j].isEating)
				{
					//不能用zhiWuType，数组会越界
					putimagePNG(plants[i][j].x, plants[i][j].y - 30, imgPlantsStatus[DA_ZUI_HUA_EAT][index]);
				}
				else
				{
					if (plants[i][j].type == DA_ZUI_HUA + 1)	//解决大嘴花偏移的问题
					{
						putimagePNG(plants[i][j].x, plants[i][j].y - 20, imgZhiWu[zhiWuType][index]);
					}
					else
					{
						putimagePNG(plants[i][j].x, plants[i][j].y, imgZhiWu[zhiWuType][index]);
					}
				}


				if (plants[i][j].type == TU_DOU_LEI + 1 && plants[i][j].boom)
				{
					plants[i][j].prepareTime++;
					if (plants[i][j].prepareTime < 70)//土豆雷爆炸图，持续一段时间
					{
						putimagePNG(plants[i][j].x - 25, plants[i][j].y - 25, &imgBoom[0]);
						putimagePNG(plants[i][j].x - 25, plants[i][j].y - 15, &imgBoom[1]);
					}
					else//爆炸后更新草坪状态
					{
						plants[i][j].boom = false;
						plants[i][j].prepareTime = 0;
						plants[i][j].type = 0;
					}
				}
			}
		}
	}

	//渲染拖动过程中的植物
	if (curZhiWu)
	{
		IMAGE* img = imgZhiWu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);//每种植物的第一张图片
		//除2是让鼠标位于图片正中间，否则在左上角
	}//把拖动植物放到最后面渲染，让拖动植物位于图层最上层
}

void drawBullets()//渲染子弹
{
	//渲染豌豆子弹
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	for (int i = 0; i < bulletMax; i++)
	{
		if (bullets[i].used)
		{
			if (bullets[i].blast)//查看子弹状态,爆炸时，切换爆炸的图片帧
			{
				IMAGE* img = &imgBulletBlast[bullets[i].frameIndex];//子弹爆炸帧
				putimagePNG(bullets[i].x, bullets[i].y, img);
			}
			else//没爆炸，显示正常的子弹
			{
				if (bullets[i].type == NORMAL_PEA)
				{
					putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal[NORMAL_PEA]);
				}
				else if (bullets[i].type == ICE_PEA)
				{
					putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal[ICE_PEA]);
				}
				else
				{
					putimagePNG(bullets[i].x, bullets[i].y, &imgBulletNormal[NORMAL_PEA]);
				}
			}
		}
	}
}

void drawShovel()
{
	if (!shovel.isSelected)
	{
		putimagePNG(753, -5, &imgShovel);
	}
	else
	{
		putimagePNG(shovel.x, shovel.y, &imgShovel);
	}
}

void drawStartSequence()//在播放准备安放植物的音乐的同时，渲染对应三张图片
{
	if (!showStartSequence)//不允许在此时渲染这三张图
	{
		return;
	}

	DWORD currentTime = GetTickCount();//获取当前时间
	DWORD elapsedTime = currentTime - startSequenceStartTime;//距离播放音乐经过的时间
	if (elapsedTime < 1500)//每张图片显示一秒
	{
		putimagePNG((WIN_WIDTH - imgReady.getwidth()) / 2, (WIN_HEIGHT - imgReady.getheight()) / 2, &imgReady);
		startSequenceIndex = 0;
	}
	else if (elapsedTime >= 1500 && elapsedTime < 2100)//每张图片显示一秒
	{
		putimagePNG((WIN_WIDTH - imgSet.getwidth()) / 2, (WIN_HEIGHT - imgSet.getheight()) / 2, &imgSet);
		startSequenceIndex = 1;
	}
	else if (elapsedTime >= 2100 && elapsedTime < 3800)
	{
		putimagePNG((WIN_WIDTH - imgPlant.getwidth()) / 2, (WIN_HEIGHT - imgPlant.getheight()) / 2, &imgPlant);
		startSequenceIndex = 2;
	}
}

void updateWindow()//渲染图片
{
	BeginBatchDraw();//开始双缓冲，先把图片显示到缓冲区中，再打印

	putimage(-GROUND_TO_WIN, 0, &imgBg);		//渲染背景图片，坐标(0,0)
	putimagePNG(260 - GROUND_TO_WIN, 0, &imgBar);	//渲染工具栏图片，PNG版本（扣图片）
	putimagePNG(0, 0, &imgPause);//渲染暂停背景图
	putimagePNG(760, 0, &imgShovelBg);//渲染铲子背景图
	//渲染顺序：铲子>阳光>子弹>僵尸>植物>字体>卡牌
	
	
	drawCards();//渲染植物卡牌
	drawSunshineNum();//渲染阳光数量字体
	
	
	drawZhiWu();//渲染植物
	drawZM();//渲染僵尸
	drawBullets();//渲染子弹
	drawSunshines();//绘制阳光
	drawShovel();//绘制铲子
	drawStartSequence();//绘制开始动画

	EndBatchDraw();//结束双缓冲
}

void show_Start_Sequence()//播放开始音乐（准备安放植物，僵尸要来了）
{
	loadimage(&imgReady, "res/ready.png");
	loadimage(&imgSet, "res/set.png");
	loadimage(&imgPlant, "res/plants.png");

	showStartSequence = true;
	startSequenceIndex = 0;
	startSequenceStartTime = GetTickCount(); //获取开始时间

	Sleep(1000);
	AudioManager::getInstance()->play("res/audio/readysetplant.mp3", LOOP_NONE, 1, 800, true);
	DWORD startTime = GetTickCount();
	while (GetTickCount() - startTime < 3500)//等待音乐播放完成
	{
		updateWindow();
		Sleep(10);
	}
	//隐藏开始序列
	showStartSequence = false;

	/*startTime = GetTickCount();
	while (GetTickCount() - startTime < 2000)
	{
		updateWindow();
		Sleep(10);
	}*/
	AudioManager::getInstance()->play("res/audio/zombiescoming.mp3", LOOP_NONE, 1, 800, true);
}


/*
* @brief:收集阳光
*/
void collectSunshine(ExMessage* msg)
{
	int count = sizeof(balls) / sizeof(balls[0]);//计算当前阳光个数
	int w = imgSunShineBall[0].getwidth();//阳光球的宽度
	int h = imgSunShineBall[0].getheight();//阳光球的高度


	for (int i = 0; i < count; i++)
	{
		if (balls[i].used)
		{
			/*int x = balls[i].x;
			int y = balls[i].y;*/
			int x = balls[i].pCur.x;
			int y = balls[i].pCur.y;//贝塞尔函数求得的实时坐标

			if (msg->x > x && msg->x < x + w &&
				msg->y > y && msg->y < y + h)	//判断点击鼠标时，鼠标是否在当前阳光球图片的范围内
			{
				//GameEventSystem::getInstance()->sendEvent(GameEventType::EVENT_SUNSHINE_COLLECTED,x,y,25,"收集阳光");
				EVENT_SEND(GameEventType::EVENT_SUNSHINE_COLLECTED, x, y, 25, "收集阳光");//便捷宏
				//balls[i].used = false;
				balls[i].status = SUNSHINE_COLLECT;//设置阳光当前状态为收集状态
				//不支持MP3格式
				//PlaySound("res/audio/sunshine.wav", NULL, SND_FILENAME | SND_ASYNC);//优化收集阳光时卡顿的问题
				//mciSendString("play res/audio/sunshine.mp3 alias bgm", NULL, 0, NULL);//添加收集阳光的音效
				//mciSendString("play bgm", NULL, 0, NULL);
				
				//设置阳光球的偏移量
				//float destY = 0;
				//float destX = 260;
				//float angle = atan((y - destY) / (x - destX));//反正切函数，求阳光球到工具栏左上角与水平线的夹角
				//balls[i].xoff = 4 * cos(angle);
				//balls[i].yoff = 4 * sin(angle);

				balls[i].p1 = balls[i].pCur;//收集阳光球当前点（pCur）的位置
				balls[i].p4 = vector2(262 - GROUND_TO_WIN, 0);//构造函数
				balls[i].t = 0;
				float distance = dis(balls[i].p1 - balls[i].p4);//运算符重载（operator -）
				float off = 16;//斜边(每次移动的距离 单位：像素)
				balls[i].speed = 1.0 / (distance / off);

				break;//找到鼠标所在的阳光后就退出
			}
		}
	}
}

void createPlants(int row, int col, int plantsType)//plantsType:1:豌豆 2：向日葵 ……
{
	plants[row][col].type = plantsType;//在对应的草坪处更新为当前选中的植物
	plants[row][col].frameIndex = 0;
	plants[row][col].shootTime = 0;
	switch (plantsType)
	{
	case WAN_DOU + 1:sunshine -= 100; break;
	case XIANG_RI_KUI + 1:sunshine -= 50; break;
	case JIAN_GUO + 1:sunshine -= 50; break;
	case TU_DOU_LEI + 1:sunshine -= 25; break;
	case DOUBLE_WAN_DOU + 1:sunshine -= 200; break;
	case ICE_WAN_DOU + 1:sunshine -= 175; break;
	case DA_ZUI_HUA + 1:sunshine -= 150; break;
	default:break;
	}
	/*int x = 256 + j * 80;
	int y = 75 + i * 100 + 20;*/
	plants[row][col].x = 256 - GROUND_TO_WIN + col * 80;
	plants[row][col].y = 75 + row * 100 + 20;//更新植物的坐标
	plants[row][col].deadTime = 300;//植物初始血量统一为300
	plants[row][col].width = 60;//植物的默认宽度
	if (plantsType == JIAN_GUO + 1)
	{
		plants[row][col].deadTime = 3000;//血量为3000(3000帧后死亡)
	}
	else if (plantsType == TU_DOU_LEI + 1)
	{
		plants[row][col].status = TU_DOU_LEI_Under;
		plants[row][col].prepareTime = 0;
		plants[row][col].boom = false;
	}
	else if (plantsType == DA_ZUI_HUA + 1)
	{
		plants[row][col].isEating = false;
		plants[row][col].deadTime = 1200;
	}
	
	
}

void userClick()
{
	ExMessage msg;	//消息变量（存放消息）
	static int status = 0;	//判断左键有没有按下
	//int zhiWuType[7] = { WAN_DOU,XIANG_RI_KUI,JIAN_GUO,TU_DOU_LEI,DOUBLE_WAN_DOU,ICE_WAN_DOU,DA_ZUI_HUA };
	if (peekmessage(&msg))//判断有没有消息，有消息返回真，无消息返回假，不会阻塞，鼠标点击也是消息
	{
		if (msg.message == WM_LBUTTONDOWN)	//如果消息类型为鼠标左键按下
		{
			if (msg.x > 350 - GROUND_TO_WIN && msg.x<350 - GROUND_TO_WIN + 70 * ZHI_WU_COUNT && msg.y < 95)	//确保鼠标在植物图片内时才生效,x从工具栏左侧到最后一张卡牌的右侧
			{
				int index = (msg.x -(350 - GROUND_TO_WIN)) / 70 ;//0代表第一张卡牌，1代表第二张卡牌
				curZhiWu = index + 1;
				if (index == WAN_DOU && sunshine >= 100)
				{
					status = 1;		//左键已经按下
					curX = msg.x;//重置curX和curY，否则会错误的在上一次种植位置显示当前选中植物
					curY = msg.y;
				}
				else if (index == XIANG_RI_KUI && sunshine >= 50)
				{
					status = 1;		//左键已经按下
					curX = msg.x;//重置curX和curY，否则会错误的在上一次种植位置显示当前选中植物
					curY = msg.y;
				}
				else if (index == JIAN_GUO && sunshine >= 50)
				{
					status = 1;		//左键已经按下
					curX = msg.x;//重置curX和curY，否则会错误的在上一次种植位置显示当前选中植物
					curY = msg.y;
				}
				else if (index == TU_DOU_LEI && sunshine >= 25)
				{
					status = 1;		//左键已经按下
					curX = msg.x;//重置curX和curY，否则会错误的在上一次种植位置显示当前选中植物
					curY = msg.y;
				}
				else if (index == DOUBLE_WAN_DOU && sunshine >= 200)
				{
					status = 1;		//左键已经按下
					curX = msg.x;//重置curX和curY，否则会错误的在上一次种植位置显示当前选中植物
					curY = msg.y;
				}
				else if(index == ICE_WAN_DOU && sunshine >= 175)
				{
					status = 1;		//左键已经按下
					curX = msg.x;//重置curX和curY，否则会错误的在上一次种植位置显示当前选中植物
					curY = msg.y;
				}
				else if (index == DA_ZUI_HUA && sunshine >= 150)
				{
					status = 1;		//左键已经按下
					curX = msg.x;//重置curX和curY，否则会错误的在上一次种植位置显示当前选中植物
					curY = msg.y;
				}
				else {
					curZhiWu = 0;
				}
			}
			else if (msg.x > 755 && msg.x < 755 + 75 && msg.y < 80)//如果点击的是铲子
			{
				shovel.isSelected = true;//此时选中铲子
				shovel.x = msg.x - imgShovel.getwidth() / 2;//确保鼠标位于铲子中间
				shovel.y = msg.y - imgShovel.getheight() / 2;
			}
			else if (msg.x > 0 && msg.x < 109 && msg.y < 39)//如果点击的是暂停按钮
			{
				if (gameStatus == GOING)
				{
					gameStatus = PAUSE;
					Event_SEND_SIMPLE(GameEventType::EVENT_GAME_PAUSE);
					printf("游戏进入暂停！\n");
				}
				else if (gameStatus == PAUSE)
				{
					gameStatus = GOING;
					Event_SEND_SIMPLE(GameEventType::EVENT_GAME_GOING);
					printf("继续游戏！\n");
				}
			}
			else //如果没有在卡牌工具栏区域内点击，就要判断是否在做其他操作(比如收集阳光)
			{
				collectSunshine(&msg);//收集阳光
			}
		}
		else if (msg.message == WM_MOUSEMOVE )	//如果消息类型为鼠标移动（植物跟着鼠标闪动）
		{	
			if (status == 1)//左键按下的同时，移动才算//添加铲子跟随鼠标移动
			{
				curX = msg.x;		//声明全局变量，让植物图片跟随鼠标
				curY = msg.y;
			}
			else if (shovel.isSelected)
			{
				shovel.x = msg.x - imgShovel.getwidth() / 2;//确保鼠标位于铲子中间
				shovel.y = msg.y - imgShovel.getheight() / 2;
			}
		}
		else if (msg.message == WM_LBUTTONUP )	//如果鼠标抬起来，则种下植物
		{
			if (msg.x > 256 - GROUND_TO_WIN && msg.x < 850 &&  msg.y > 75 && msg.y < 575)
			{
				int row = (msg.y - 75) / 100;		//获取草坪的坐标
				int col = (msg.x - (256 - GROUND_TO_WIN)) / 81;//记得加括号，否则变成+GROUND_TO_WIN
				if (shovel.isSelected)//如果选中铲子
				{
					if (plants[row][col].type > 0)//如果当前格子有植物
					{
						Event_SEND_SIMPLE(GameEventType::EVENT_PLANTS_REMOVE);
						plants[row][col].type = 0;//选中铲子的话清除植物
						plants[row][col].frameIndex = 0;
						plants[row][col].deadTime = 0;
					}
					shovel.isSelected = false;//不管有没有清除植物，只要松开右键，就放下铲子
					shovel.x = 753;
					shovel.y = -5;
				}
				else if (status == 1)//种植植物
				{
					//发送事件
					EVENT_SEND(GameEventType::EVENT_PLANT_PLANTED, msg.x, msg.y, 0, "种植植物");
					printf("%d,%d\n", row, col);

					if (plants[row][col].type == 0)//如果当前草坪没有选中的植物，才种植物，防止重复种植
					{
						createPlants(row, col, curZhiWu);//创建植物
					}
				}
			}
			else
			{
				if (shovel.isSelected)//如果在草坪外，也要放下铲子
				{
					shovel.isSelected = false;
					shovel.x = 753;
					shovel.y = -5;
				}
			}
			
			//重置植物选择状态
			curZhiWu = 0;//取消选中
			status = 0;//取消图片跟随
			curX = msg.x;
			curY = msg.y;
		}
	}
}

void createSunshine()
{
	static int count = 0;
	static int fre = 300;//初始300帧
	count++;
	if (count >= fre)
	{
		fre = rand() % 300 + 400;//每300到700帧之间随机的生成一个阳光
		count = 0;//计数器清零
		//从阳光池中取一个可以使用的
		int ballMax = sizeof(balls) / sizeof(balls[0]);	//阳光池中阳光的数量

		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);//在池中寻找可用的阳光，如果该阳光已被使用，
		//则进行下一轮循环，看下一个阳光是否可用，直到找到可用的阳光或阳光都被使用，退出循环
		if (i >= ballMax)
			return;//如果池中没有可用阳光，则退出函数

		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].timer = 0;

		balls[i].status = SUNSHINE_DOWN;
		balls[i].t = 0;
		//在随机生成阳光时，阳光呈垂直下降状态
		balls[i].p1 = vector2(rand() % (850 - (260 - GROUND_TO_WIN)) + 260 - GROUND_TO_WIN,60);//确定垂直下降的起点（随机在某一列产生）
		balls[i].p4 = vector2(balls[i].p1.x,(rand() % 4) * 90 + 200);//确定垂直下降的终点（随机落到某一行）
		int off = 2;
		float distance = balls[i].p4.y - balls[i].p1.y;
		balls[i].speed = 1.0 / (distance / off);
		//balls[i].x = rand() % (901 - 260) + 260;//阳光在x坐标为260-900之间随机产生
		//balls[i].y = 60;
		//balls[i].destY = (rand() % 4) * 90 + 200;//阳光降落在第二行到第五行之间
		//balls[i].xoff = 0;//偏移量
		//balls[i].yoff = 0;

	}

	//向日葵生产阳光
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (plants[i][j].type == XIANG_RI_KUI + 1)//XIANG_RI_KUI是1
			{
				plants[i][j].timer++;
				if (plants[i][j].timer > 1000)//周期
				{
					plants[i][j].timer = 0;

					int k;
					for (k = 0; k < ballMax && balls[k].used; k++);//遍历所有阳光球，直到找到未使用的阳光球
					if (k >= ballMax) return;//遍历完所有阳光球，仍没有未使用的阳光球

					balls[k].used = true;
					//贝塞尔曲线需要四个点(p1,p2,p3,p4),p1为起点(向日葵位置)，p2为曲线的最高点，p4为落点
					balls[k].p1 = vector2(plants[i][j].x, plants[i][j].y);
					//阳光球落点到向日葵的水平距离（随机分布在向日葵左右两边）
					int w = (100 + rand() % 51) * (rand() % 2 ? 1 : -1);//使用rand生成0或者1的随机数，1：参数为1;0，参数为-1
					balls[k].p4 = vector2(plants[i][j].x + w, plants[i][j].y + imgZhiWu[XIANG_RI_KUI][0]->getheight()
						- imgSunShineBall->getheight());//阳光球落点的y坐标(向日葵的y坐标+向日葵的高度-阳光球的高度)
					balls[k].p2 = vector2(balls[k].p1.x + w * 0.3, balls[k].p1.y - 80);//p2最高，所以向上偏移
					balls[k].p3 = vector2(balls[k].p1.x + w * 0.7, balls[k].p1.y - 50);//p3高于p4，低于p1
					balls[k].status = SUNSHINE_PRODUCT;//设置向日葵状态为生产阳光
					balls[k].speed = 0.05;
					balls[k].t = 0;
				}
			}
		}
	}
}

void updateSunshine()//更新阳光
{
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++)
	{
		if (balls[i].used)
		{
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
			if (balls[i].status == SUNSHINE_DOWN)
			{
				sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);//p1 + t * (p4 - p1)
				if (sun->t >= 1)
				{
					sun->status = SUNSHINE_GROUND;//此时阳光已经到达落点
					sun->timer = 0;//阳光落到地面的保留时间
				}
			}
			else if (balls[i].status == SUNSHINE_GROUND)
			{
				balls[i].timer++;
				if (balls[i].timer == 300)
				{
					balls[i].used = false;
					balls[i].timer = 0;
				}
			}
			else if (balls[i].status == SUNSHINE_COLLECT)//阳光飞回卡槽状态
			{
				sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = sun->p1 + sun->t * (sun->p4 - sun->p1);
				if (sun->t > 1)
				{
					sun->used = false;
					sunshine += 25;
				}
			}
			else if (balls[i].status == SUNSHINE_PRODUCT)//向日葵产出的阳光
			{
				sunshineBall* sun = &balls[i];
				sun->t += sun->speed;
				sun->pCur = calcBezierPoint(sun->t, sun->p1, sun->p2, sun->p3, sun->p4);//使用贝塞尔曲线绘制向日葵产出阳光
				if (sun->t > 1)
				{
					sun->status = SUNSHINE_GROUND;//着陆状态
					sun->timer = 0;
				}
			}
		}
	}
}

void createZM()//创建僵尸
{
	if (zmCount >= ZM_MAX)
	{
		return;	//如果已经出现的僵尸数量大于僵尸最大数量，则不再创建僵尸
	}
	static int zmFre = 300;//每300帧创建一个僵尸
	static int count = 0;//循环次数
	static int timeFactor = 0;//时间因子（控制僵尸生成频率及生成大波僵尸的概率）
	static int timeCount = 0;//次数
	static int zmOutFlag = 0;
	while (++timeCount >= 500)//每500帧增加一次时间因子
	{
		timeCount = 0;
		if (timeFactor <= 100)//每0.5秒增加一次，一共增加100次，50秒后僵尸生成速率达到最快
		{
			timeFactor++;
		}
	}

	int baseFre = 400 - timeFactor * 3;
	if (baseFre < 100) baseFre = 100;//最低不低于100帧
	count++;//循环次数
	if (count > zmFre)
	{
		count = 0;
		int prob = 7 - timeFactor / 20;//prob为2-7//timeFactor/20的值为0-5
		if (prob < 2) prob = 2;
		int zmOut = rand() % prob;//zmOut为1/10 到 1/5

		if (zmOut == 0 && zmOutFlag < 15)//25%的概率生成多个僵尸，次数不超过15次
		{
			zmFre = 50;
			zmOutFlag++;
		}
		else
			zmFre = rand() % 50 + baseFre;//生成僵尸的速率（50 + (150 到 400))

		int i;
		//判断是否是在僵尸池中取的，并判断取得的僵尸是否被使用过了（注意这是空的for循环）
		for (i = 0; i < zmMax && zms[i].used; i++);
		
		////小鬼僵尸数量
		//int zmSMax = sizeof(zmsS) / sizeof(zmsS[0]);

		//创建僵尸
		if (i < zmMax)
		{
			zms[i].used = true;
			zms[i].x = WIN_WIDTH;
			zms[i].row = rand() % 5;//0-4行
			zms[i].y = 80 + (1 + zms[i].row) * 100;//1-5行
			zms[i].speed = 1;
			zms[i].blood = 100;
			zms[i].dead = false;//僵尸默认时存活的
			zms[i].eating = false;
			zms[i].frameIndex = 0;
			zms[i].width = 35;//像素
			zms[i].eatingRow = -1;//初始化为-1，没有吃植物
			zms[i].eatingCol = -1;
			zmCount++;//僵尸数量加一
			//memset(&zms[i], 0, sizeof(zms[i]));//创建僵尸时，将其数据全部初始化为0
			int random = rand() % 20;
			if (random < 5)//0-4：普通僵尸
			{
				zms[i].blood = 100;
				zms[i].type = RegularZM;
			}
			else if (random >= 5 && random < 7)//5-6:小鬼僵尸
			{
				zms[i].speed = 2;
				zms[i].blood = 50;
				zms[i].type = SmallZM;
			}
			else if (random >= 7 && random < 10)//7-9：路障僵尸
			{
				zms[i].blood = 300;
				zms[i].type = RoadBlockZM;
			}
			else if(random >= 10 && random < 13)//10-12 :铁桶僵尸
			{
				zms[i].blood = 400;
				zms[i].type = BucketZM;
			}
			else if (random >= 13 && random < 16)//13-15橄榄球僵尸
			{
				zms[i].blood = 500;
				zms[i].type = RugbyZM;
				zms[i].speed = 2;
			}
			else if (random >= 16 && random < 20)//16-19铁门僵尸
			{
				zms[i].blood = 600;
				zms[i].type = IronGateZM;
			}
			
		}
		

		////小鬼僵尸
		//for (i = 0; i < zmSMax && zmsS[i].used; i++);//循环，直到找到能用的僵尸
		//if (i < zmSMax)
		//{
		//	zmsS[i].used = true;
		//	zmsS[i].x = WIN_WIDTH;
		//	zmsS[i].y = 80 + (1 + rand() % 5) * 100;
		//	zmsS[i].speed = 2;
		//}
	}
}

void updateZM()//更新僵尸状态
{
	//int zmSMax = sizeof(zmsS) / sizeof(zmsS[0]);//小鬼僵尸
	static int count = 0;
	count++;
	//动作状态
	if (count > 6)//每2次循环更新僵尸动作
	{
		count = 0;
		//更新普通僵尸的位置
		for (int i = 0; i < zmMax; i++)
		{
			if (zms[i].used && !zms[i].dead)//只有或者的僵尸才移动(修改1)
			{
				zms[i].x -= zms[i].speed;
				if (zms[i].x < 100 - GROUND_TO_WIN)
				{
					//printf("GAME OVER\n");
					//MessageBox(NULL, "over", "over", 0);//待优化
					//exit(0);//待优化
					Event_SEND_SIMPLE(GameEventType::EVENT_GAME_OVER);
					gameStatus = FAIL;
					Sleep(2000);
					IMAGE img;
					loadimage(&img, "res/fail.png");
					putimagePNG(200, 80, &img);
					AudioManager::getInstance()->play("res/audio/lose.mp3", LOOP_NONE, 1, 800, true);
				}
			}
		}
	}

	//帧序号状态（动画状态）
	static int count2 = 0;
	count2++;
	if (count2 > 6)
	{
		count2 = 0;
		for (int i = 0; i < zmMax; i++)
		{
			if (zms[i].used)//是否被使用
			{
				if (zms[i].dead)
				{
					zms[i].frameIndex++;
					if (zms[i].frameIndex >= 20)
					{
						zms[i].used = false;	//僵尸死亡，回收僵尸
						killCount++;
						if (killCount >= ZM_MAX)
						{
							Event_SEND_SIMPLE(GameEventType::EVENT_GAME_WIN);
							gameStatus = WIN;//当击杀数量大于僵尸总数，胜利！ 游戏结束
							Sleep(2000);//停两秒
							loadimage(0, "res/win.png");
							AudioManager::getInstance()->play("res/audio/win.mp3", LOOP_NONE, 1, 800, true);
						}
					}
				}
				else if (zms[i].eating)//是否在进食
				{
					if (zms[i].type == RegularZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;//1-21帧率
					}
					else if (zms[i].type == RoadBlockZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 11;
					}
					else if (zms[i].type == BucketZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 11;
					}
					else if (zms[i].type == SmallZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 7;
					}
					else if (zms[i].type == RugbyZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 10;
					}
					else if (zms[i].type == IronGateZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 12;
					}

				}
				else//存活状态，进行各自的行走动画
				{
					if (zms[i].type == RegularZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 21;//1-21帧率
					}
					else if (zms[i].type == RoadBlockZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 11;
					}
					else if (zms[i].type == BucketZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 11;
					}
					else if (zms[i].type == SmallZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 12;
					}
					else if (zms[i].type == RugbyZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 11;
					}
					else if (zms[i].type == IronGateZM)
					{
						zms[i].frameIndex = (zms[i].frameIndex + 1) % 23;
					}
				}
			}
		}
	}
		

	////更新小鬼僵尸的位置
	//for (int i = 0; i < zmSMax; i++)
	//{
	//	if (zmsS[i].used)
	//	{
	//		zmsS[i].x -= zmsS[i].speed;
	//		if (zmsS[i].x < 200)
	//		{
	//			printf("GAME OVER\n");
	//			MessageBox(NULL, "over", "over", 0);//待优化
	//			exit(0);//待优化
	//		}
	//	}
	//}
}

void createPeaBullet(int row, int col, int plantType, int bulletIndex,int type)//创建子弹//type:子弹类型
{
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);

	int k;
	for (k = 0; k < bulletMax && bullets[k].used; k++);//遍历子弹池，直到找到未使用的子弹
	if (k < bulletMax)
	{
		bullets[k].used = true;//标记该子弹已使用
		bullets[k].row = row;//子弹所在行
		bullets[k].speed = 7;//子弹速度
		bullets[k].damage = 10;//子弹伤害
		bullets[k].blast = false;//子弹状态
		bullets[k].frameIndex = 0;//帧序列

		//int x = 256 + j * 80;
		//int y = 75 + i * 100 + 20;

		int zwX = 256 - GROUND_TO_WIN + col * 80;
		int zwY = 75 + row * 100 + 20;//发射子弹植物的坐标
		//设置子弹坐标，让子弹从植物嘴巴发出
		bullets[k].x = zwX + imgZhiWu[plantType - 1][0]->getwidth() - 30;//子弹x坐标

		if (plantType == DOUBLE_WAN_DOU + 1)//如果是双发豌豆射手，就让两发子弹错开
		{
			bullets[k].type = NORMAL_PEA;
			if (bulletIndex == 0)
			{
				bullets[k].y = zwY;
			}
			else
			{
				bullets[k].y = zwY + 7;
			}
		}
		else if (plantType == WAN_DOU + 1)//普通豌豆射手
		{
			bullets[k].type = NORMAL_PEA;
			bullets[k].y = zwY + 4;
		}
		else if (plantType == ICE_WAN_DOU + 1)
		{
			bullets[k].damage = 15;
			bullets[k].type = ICE_PEA;
			bullets[k].y = zwY + 4;
		}

		//射击音效
		Event_SEND_SIMPLE(GameEventType::EVENT_BULLET_FIRED);
		/*else
		{
			PlaySound("res/audio/shootpea2.wav", NULL, SND_FILENAME | SND_ASYNC);
		}*/
	}
	else
	{
		printf("子弹池已满，无法发射更多子弹！\n");
	}
}

void shoot()//发射豌豆子弹
{
	int lines[5] = { 0 };
	int zmCount = sizeof(zms) / sizeof(zms[0]);
	int bulletMax = sizeof(bullets) / sizeof(bullets[0]);
	int dangetX = WIN_WIDTH - 100;	//当僵尸越过这个位置时，豌豆开始攻击

	for (int i = 0; i < zmCount; i++)
	{
		if (zms[i].used && zms[i].x < dangetX)//当僵尸越过这个位置时，豌豆开始攻击
		{
			lines[zms[i].row] = 1;	//表示该行有僵尸，准备发射子弹
		}
	}
	for (int i = 0; i < 5; i++)//遍历草坪块
	{
		for (int j = 0; j < 9; j++)
		{
			if (lines[i])//该行有僵尸才发射子弹
			{
				//植物类型为豌豆类型才发射豌豆子弹
				if (plants[i][j].type == WAN_DOU + 1 || plants[i][j].type == DOUBLE_WAN_DOU + 1)
				{
					plants[i][j].shootTime++;//每个豌豆自己的计数器自增

					int shootRate = (plants[i][j].type == DOUBLE_WAN_DOU + 1) ? 100 : 70;

					if (plants[i][j].shootTime > shootRate)//豌豆发射速率
					{

						if (plants[i][j].type == DOUBLE_WAN_DOU + 1)
						{
							//发射第一颗子弹
							createPeaBullet(i, j, plants[i][j].type, 0, NORMAL_PEA);

							if (plants[i][j].timer == 0)
							{
								plants[i][j].timer = 20;//20帧后发射第二颗子弹
							}
						}
						else//普通豌豆射手
						{
							createPeaBullet(i, j, plants[i][j].type, 0, NORMAL_PEA);
						}
						plants[i][j].shootTime = 0;
					}

					if (plants[i][j].type == DOUBLE_WAN_DOU + 1 && plants[i][j].timer > 0)
					{
						plants[i][j].timer--;
						if (plants[i][j].timer == 0)
						{
							createPeaBullet(i, j, plants[i][j].type, 1, NORMAL_PEA);
						}
					}
				}
				else if (plants[i][j].type == ICE_WAN_DOU + 1)
				{
					plants[i][j].shootTime++;//每个豌豆自己的计数器自增

					if (plants[i][j].shootTime > 70)//豌豆发射速率
					{
						plants[i][j].shootTime = 0;
						createPeaBullet(i, j, plants[i][j].type, 0, ICE_PEA);
					}
				}
			}
		}
	}
}

void updateBullets()//更新豌豆子弹
{
	int countMax = sizeof(bullets) / sizeof(bullets[0]);//当前豌豆池中豌豆数量
	//static int count = 0;
	//if (++count < 6)return; //降频
	//count = 0;

	for (int i = 0; i < countMax; i++)
	{
		if (bullets[i].used)
		{
			bullets[i].x += bullets[i].speed;//更新豌豆位置
			if (bullets[i].x > WIN_WIDTH)
			{
				bullets[i].used = false;//超出屏幕，回收子弹
			}

			if (bullets[i].blast)//若子弹发生碰撞，改变帧序号
			{
				bullets[i].frameIndex++;
				if (bullets[i].frameIndex > 3)//爆炸效果结束，回收子弹
				{
					bullets[i].used = false;
				}
			}
		}
	}
}

void checkBullet2Zm()//子弹跟僵尸的碰撞检测
{
	int bCount = sizeof(bullets) / sizeof(bullets[0]);//子弹数量
	int zCount = sizeof(zms) / sizeof(zms[0]);//僵尸数量
	for (int i = 0; i < bCount; i++)
	{
		if (bullets[i].used == false || bullets[i].blast) continue;//如果子弹还没被使用或者已经爆炸了，就不参与检测
		{
			for (int k = 0; k < zCount; k++)
			{
				if (zms[k].used == false || bullets[i].row != zms[k].row) 
					continue;//如果生成的僵尸还没被使用，则不参与检测,或者当前子弹跟僵尸不在同一行，则跳过本次循环
				int lx = zms[k].x + 80;//子弹跟僵尸碰撞检测的左侧临界点
				int rx = zms[k].x + 115;//右侧临界点
				int x = bullets[i].x;//子弹的横坐标

				//检查子弹是否和僵尸在同一行；如果子弹的横坐标在(x1,x2)范围内，则视为发生碰撞
				//如果僵尸没有死，才做碰撞检测，否则即使僵尸死掉，也会做检测，导致重复进行死亡动画
				if (zms[k].dead == false && x >= lx && x <= rx)
				{
					Event_SEND_SIMPLE(GameEventType::EVENT_BULLET_HIT);
					//如果僵尸不在进食状态，被寒冰豌豆射中才会减速，且最低速度为0.5
					//否则僵尸会在进食的时候仍保持最低速度（0.5）行走，而不是停下来吃
					if (bullets[i].type == ICE_PEA && !zms[k].eating)
					{
						zms[k].speed /= 2;
						if (zms[k].speed < 0.5)
						{
							zms[k].speed = 0.5;
						}
					}
					zms[k].blood -= bullets[i].damage;
					bullets[i].blast = true;
					bullets[i].speed = 0;//碰撞后子弹停止

					//僵尸低血量状态
					if (zms[k].blood <= 0)
					{
						zms[k].dead = true;
						zms[k].speed = 0;
						zms[k].frameIndex = 0;//死亡动画的动画帧
					}
					break;//当子弹找到与其碰撞的僵尸后，退出循环
				}
				
			}
		}
	}
}

void checkZm2ZhiWu()//僵尸跟植物的碰撞检测(僵尸吃植物)
{
	int zCount = sizeof(zms) / sizeof(zms[0]);
	for (int i = 0; i < zCount; i++)
	{
		if (zms[i].dead) continue;//如果僵尸已经死亡，就不再判断了

		int row = zms[i].row;
		bool foundPlant = false;//标记是否找到可以吃的植物
		for (int k = 0; k < 9; k++)//检查当前行所有植物
		{
			if (plants[row][k].type == 0)continue;//如果这一行都没有植物，则跳过
			int zhiWuX = 256 - GROUND_TO_WIN + k * 80;
			int x1 = zhiWuX + 10;//植物的左侧边界
			int x2 = zhiWuX + plants[row][k].width;//植物的右侧边界
			int x3 = zms[i].x + 80;//僵尸(人物)的左侧边界
			if (zms[i].type == RoadBlockZM)
			{
				x3 += 20;
			}

			//此时植物位于僵尸攻击范围内
			//但这里只判断了僵尸的x坐标是否在植物范围内！！！(也就是说整列植物都在僵尸的攻击范围内)
			//所以要加(zms[i].eating && k != zms[i].eatingCol))这个条件，让该行的僵尸吃该行的植物，而不是其他行的植物！！！
			if (x3 > x1 && x3 < x2)
			{
				foundPlant = true;//找到可以吃的植物了
				//已解决有一个僵尸正在吃一个植物时，其他僵尸直接忽视该植物的bug
				//已解决有僵尸吃完植物，继续原地进食的bug
				//如果僵尸不在进食状态或者正在吃其他植物(僵尸会选择吃该行离其更近的植物)
				if (!zms[i].eating || (zms[i].eating && k != zms[i].eatingCol))
				{
					Event_SEND_SIMPLE(GameEventType::EVENT_ZOMBIE_EATING);
					//开始吃这个植物
					zms[i].eating = true;
					zms[i].speed = 0;
					zms[i].frameIndex = 0;
					zms[i].eatingRow = row;
					zms[i].eatingCol = k;
					//PlaySound("res/audio/zmeat.wav", NULL, SND_FILENAME | SND_ASYNC);
				}

				//减少植物血量
				plants[row][k].deadTime--;
				if (plants[row][k].deadTime < 0)
				{
					//植物死亡，清除植物
					plants[row][k].type = 0;
					plants[row][k].deadTime = 0;

					//如果僵尸正在吃这个植物，需要恢复到行走状态
					if (zms[i].eatingRow == row && zms[i].eatingCol == k)
					{
						zms[i].eating = false;
						zms[i].speed = 1;
						zms[i].frameIndex = 0;
						zms[i].eatingRow = -1;
						zms[i].eatingCol = -1;
					}
				}
				break;//找到植物后就退出循环？？？？？？？？？？
				}
			}
		//如果没有找到植物，且僵尸正在进食状态，则恢复行走
		if(!foundPlant && zms[i].eating)
		{
			zms[i].eating = false;
			zms[i].speed = 1;
			zms[i].frameIndex = 0;
			zms[i].eatingRow = -1;
			zms[i].eatingCol = -1;
		}
	}
}

void checkPotatoMine2ZM()//土豆雷对僵尸的碰撞检测
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (plants[i][j].type == TU_DOU_LEI + 1 && plants[i][j].status == TU_DOU_LEI_Ready)
			{
				for (int k = 0; k < zmMax; k++)//对每个僵尸进行检测
				{
					if (zms[k].used == false || zms[k].dead || zms[k].row != i) continue;
					int plantX = 256 - 120 + j * 80;
					int x1 = plantX + 10;
					int x2 = plantX + 60;
					int x3 = zms[k].x + 80; //僵尸的左边界
					if (x3 > x1 && x3 < x2)//认为此时发生碰撞
					{
						zms[k].dead = true;
						zms[k].speed = 0;
						zms[k].frameIndex = 0;

						plants[i][j].boom = true;
						AudioManager::getInstance()->play("res/audio/potato_mine.mp3", LOOP_NONE, 1, 800, true);
					}
				}
			}
		}
	}
}

void checkDA_ZUI_HUA2ZM()
{
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (plants[i][j].type == DA_ZUI_HUA + 1 && plants[i][j].isEating == false)
			{
				for (int k = 0; k < zmMax; k++)
				{
					if (zms[k].used == false || zms[k].dead || zms[k].row != i) continue;
					int plantX = plants[i][j].x;
					int x1 = plantX + 43;
					int x2 = plantX + 43 + 80;//（攻击距离为1格）
					int x3 = zms[k].x + 80; //僵尸的左边界
					//// 调试信息
					// printf("大嘴花[%d,%d] 坐标:%d, 攻击范围:[%d-%d], 僵尸[%d]左边界:%d\n", 
					//        i, j, plantX, x1,x2, k, x3);
					if (x3 > x1 && x3 < x2)//认为此时发生碰撞
					{
						zms[k].used = false;
						killCount++;
						plants[i][j].isEating = true;
						plants[i][j].frameIndex = 0;
						plants[i][j].prepareTime = 0;
						break;
					}
				}
			}
		}
	}
}


void collisionCheck()//碰撞检测函数
{
	checkBullet2Zm();//子弹跟僵尸的碰撞检测
	checkZm2ZhiWu();//僵尸跟植物的碰撞检测(僵尸吃植物)
	checkDA_ZUI_HUA2ZM();//大嘴花跟僵尸的碰撞检测
}

void updateZhiWu()//更新植物数据
{
	//更改植物的摆动速度
	static int count = 0;
	if (++count < 4)return;//当count为1时，不执行循环;为2时不返回，执行下面的循环
	count = 0;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (plants[i][j].type > 0)
			{
				plants[i][j].frameIndex++;//切换到下一帧
				int zhiWuType = plants[i][j].type - 1;
				int index = plants[i][j].frameIndex;
				if (plants[i][j].type == TU_DOU_LEI + 1)
				{
					if (plants[i][j].status == TU_DOU_LEI_Under)
					{
						plants[i][j].prepareTime++;
						if (plants[i][j].prepareTime > 300)	//土豆雷的准备时间
						{
							plants[i][j].status = TU_DOU_LEI_Ready;
							plants[i][j].prepareTime = 0;
						}
					}
					zhiWuType = plants[i][j].status;
					if (imgPlantsStatus[zhiWuType][index] == NULL)//重置土豆雷动画状态
					{
						plants[i][j].frameIndex = 0;
					}
				}
				else if (plants[i][j].type == DA_ZUI_HUA + 1 )
				{
					if (plants[i][j].isEating == true)
					{
						plants[i][j].prepareTime++;
						if (imgPlantsStatus[DA_ZUI_HUA_EAT][plants[i][j].frameIndex] == NULL)
						{
							plants[i][j].frameIndex = 0;
						}
						if (plants[i][j].prepareTime > 300)
						{
							plants[i][j].isEating = false;
							plants[i][j].prepareTime = 0;
							plants[i][j].frameIndex = 0;//重置为待机状态第一帧
						}
					}
					else
					{
						if (imgZhiWu[DA_ZUI_HUA][plants[i][j].frameIndex] == NULL)
						{
							plants[i][j].frameIndex = 0;
						}
					}
				}

				if (imgZhiWu[zhiWuType][index] == NULL)//如果图片帧数组中的指针为空时，重置plants[i][j].frameIndex
				{
					plants[i][j].frameIndex = 0;
				}
			}
		}
	}
}

void updateGame()//更新游戏数据
{
	updateZhiWu();//更新植物数据

	createSunshine();//创建阳光
	updateSunshine();//更新阳光的状态

	createZM();//创建僵尸
	updateZM();//更新僵尸的状态

	shoot();//发射豌豆子弹
	updateBullets();//更新豌豆子弹
	collisionCheck();//实现豌豆子弹和僵尸的碰撞检测
	checkPotatoMine2ZM();//检查土豆雷与僵尸碰撞
}

void startUI()//开始菜单
{
	IMAGE imgBg, imgMenu1, imgMenu2;//进入游戏按钮
	IMAGE imgBreak1, imgBreak2;//退出游戏按钮
	loadimage(&imgBg, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");
	loadimage(&imgBreak1, "res/menu_quit1.png");
	loadimage(&imgBreak2, "res/menu_quit2.png");

	int flagStart = 0;
	int flagBreak = 0;
	mciSendString("open res/audio/bg.MP3 alias bg", 0, 0, 0);//设置背景音乐
	mciSendString("play bg repeat", 0, 0, 0);
	mciSendString("setaudio bg volume to 200", 0, 0, 0);//修改音量

	while (1)
	{
		BeginBatchDraw();//开启双缓冲
		putimage(0, 0, &imgBg);
		putimagePNG(475, 75, flagStart?&imgMenu2:&imgMenu1);
		putimagePNG(811, 505, flagBreak?&imgBreak2: &imgBreak1);
		ExMessage msg;
		if (peekmessage(&msg))//获取消息
		{
			if (msg.message == WM_LBUTTONDOWN )//如果鼠标左键按下
			{
				if (msg.x > 475 && msg.x < 475 + 331
					&& msg.y>75 && msg.y < 75 + 140)//确保鼠标在选项内才能选中，鼠标按下
				{
					flagStart = 1;	//选项切换为高亮版本
					EndBatchDraw();//结束双缓冲
				}
				else if (msg.x > 811 && msg.x < 811 + 47
						&& msg.y>505 && msg.y < 505 + 27)//按下退出按钮，变色
				{
					flagBreak = 1;
					EndBatchDraw();//结束双缓冲
				}
			}
			else if (msg.message == WM_LBUTTONUP)	//鼠标抬起
			{
				if (flagStart)//进入游戏
				{
					Sleep(600);//延迟一段时间再进入游戏
					gameStatus = START;
					Event_SEND_SIMPLE(GameEventType::EVENT_GAME_START);
					
					EndBatchDraw();
					break;//退出开始菜单，进入游戏
				}
				else if (flagBreak)
				{
					Event_SEND_SIMPLE(GameEventType::EVENT_GAME_BREAK);
					EndBatchDraw();
					exit(0);
				}
			}
		}
		EndBatchDraw();//结束双缓冲
	}
}

void viewScene()//过场动画
{
	int xMin = WIN_WIDTH - imgBg.getwidth();//900-1400 = -500
	vector2 points[9] =
	{
		{550,80},{650,160},{570,170},{630,200},{515,250},
		{565,400},{700,340},{525,300},{590,320}
	};//开场显示九个僵尸的位置 
	int index[9];
	for (int i = 0; i < 9; i++)
	{
		index[i] = rand() % 11;//九个僵尸的起始帧不一样(站立动画随机播放)
	}
	int count = 0;
	for (int x = 0; x >= xMin; x -= 2)//移动背景图，展示僵尸
	{
		BeginBatchDraw();
		putimage(x, 0, &imgBg);

		count++;
		for (int k = 0; k < 9; k++)//绘制站立状态僵尸
		{
			putimagePNG(points[k].x - xMin + x,points[k].y,&imgZMStand[index[k]]);
			if (count >= 10)
			{
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 10)count = 0;//重置count，切换站立动画为下一帧

		EndBatchDraw();
		Sleep(5);//停5帧
	}

	AudioManager* audio = AudioManager::getInstance();
	audio->play("res/audio/evillaugh.mp3", LOOP_NONE, 1, 400, true);

	//停留时间
	for (int i = 0; i < 100; i++)
	{
		BeginBatchDraw();
		
		putimage(xMin, 0, &imgBg);
		for (int k = 0; k < 9; k++)
		{
			putimagePNG(points[k].x, points[k].y, &imgZMStand[index[k]]);
			index[k] = (index[k] + 1) % 11;//切换到下一帧
		}

		EndBatchDraw();
		Sleep(30);
	}

	//移动回主场景
	for (int x = xMin; x <= -GROUND_TO_WIN; x += 2)//移动背景图
	{
		BeginBatchDraw();
		putimage(x, 0, &imgBg);
		
		count++;
		for (int k = 0; k < 9; k++)//绘制站立状态僵尸
		{
			putimagePNG(points[k].x - xMin + x, points[k].y, &imgZMStand[index[k]]);
			if (count >= 10)
			{
				index[k] = (index[k] + 1) % 11;
			}
		}
		if (count >= 10)count = 0;//重置count，切换站立动画为下一帧

		EndBatchDraw();
		Sleep(5);//停5帧
	}
}

void barsDown()//卡槽下降动画
{
	int height = imgBar.getheight();
	for(int y = -height; y <= 0; y++)
	{
		BeginBatchDraw();

		putimage(-GROUND_TO_WIN, 0, &imgBg);
		putimagePNG(260 - GROUND_TO_WIN, y, &imgBar);
		for (int i = 0; i < ZHI_WU_COUNT; i++)
		{
			int Cards_x = 350 - GROUND_TO_WIN + i * 70;	//各植物卡牌的x坐标
			int Cards_y = 5 + y;						//y坐标
			putimagePNG(Cards_x, Cards_y, &imgCards[i]);
		}
		
		sprintf_s(scoreText, sizeof(scoreText), "%d", sunshine);
		outtextxy(291 - GROUND_TO_WIN, 68 + y, scoreText);//设置阳光数量的位置
		EndBatchDraw();
		Sleep(5);
	}
}

void loopMain()
{

	AudioManager* audio = AudioManager::getInstance();

	//预加载音效
	audio->preload("res/audio/sunshine.wav");
	audio->preload("res/audio/plantdown.wav");
	audio->preload("res/audio/shootpea1.wav");
	audio->preload("res/audio/peacrush3.wav");
	audio->preload("res/audio/zmeat.wav");

	int timer = 0;
	bool flag = true;
	while (1)
	{
		//处理事件队列
		GameEventSystem::getInstance()->processEvents();
		if (gameStatus != PAUSE)//当暂停时，不处理其他操作
		{
			userClick();//用户处理
		}
		timer += getDelay();//第一次调用返回0，往后每一次调用返回距离上次调用过了多长时间，单位：ms
		if (timer > 10)//延时图片渲染和改变数据，植物摇摆速度
		{
			flag = true;
			timer = 0;//重置timer为0，否则一直大于20
		}

		if (flag)
		{
			flag = false;
			if (gameStatus != PAUSE)
			{
				updateWindow();//图片渲染
			}

			if (gameStatus == PAUSE)
			{
				BeginBatchDraw();

				putimagePNG((WIN_WIDTH - imgPauseMenu.getwidth()) / 2,
					(WIN_HEIGHT - imgPauseMenu.getheight()) / 2, &imgPauseMenu);
				ExMessage msg;
				if (peekmessage(&msg) && msg.message == WM_LBUTTONDOWN)//左键按下
				{
					if (msg.x > 300 && msg.x < 600 && msg.y > 450 && msg.y < 510)//如果鼠标在返回游戏按键内，则返回游戏
					{
						gameStatus = GOING;
						printf("继续游戏！\n");
					}
					else if (msg.x > 350 && msg.x < 550 && msg.y > 380 && msg.y < 410)//返回主菜单
					{
						gameStatus = BACK_TO_MENU;
						nextAction = ACTION_BACK_TO_MENU;
						printf("返回主菜单！\n");
						return;
					}
					else if (msg.x > 350 && msg.x < 550 && msg.y > 330 && msg.y < 360)//重新开始游戏
					{
						gameStatus = RESTART;
						nextAction = ACTION_RESTART_GAME;
						printf("重新开始游戏！\n");
						return;//退出当前游戏循环
					}
				}
				EndBatchDraw();
				Sleep(10);
				continue;
			}
			else if (gameStatus == GOING)
			{
				updateGame();//改变相关数据
				audio->update();
			}

			if (gameStatus == WIN || gameStatus == FAIL || gameStatus == BREAK)
			{
				nextAction = ACTION_EXIT;
				break;//检查游戏是否结束
			}
				
		}
	}
}

void resetGameData()
{
	//清空数据
	memset(plants, 0, sizeof(plants));
	memset(zms, 0, sizeof(plants));
	memset(bullets, 0, sizeof(plants));
	memset(balls, 0, sizeof(plants));
	
	gameStatus = GOING;

	sunshine = 100;

	curZhiWu = 0;

	zmCount = 0;
	killCount = 0;

	shovel.isSelected = false;
	shovel.x = 753;
	shovel.y = -5;

	GameEventSystem::getInstance()->clearQueue();

	printf("游戏数据已重置!\n");
}

int main(void)
{
	while (1)
	{
		switch (nextAction)
		{
		case ACTION_START_NEW_GAME:
		{
			gameInit();//初始化
			startUI();//启动菜单
			break;
		}
		case ACTION_RESTART_GAME://重新开始游戏，但是不退出到主菜单
		{
			resetGameData();
			break;
		}

		case ACTION_BACK_TO_MENU://返回主菜单
		{
			nextAction = ACTION_START_NEW_GAME;
			continue;
		}

		case ACTION_EXIT:
		{
			GameEventSystem::getInstance()->destroyInstance();//销毁事件队列单例实例
			system("pause");
			return 0;
		}
		}
		viewScene();//巡视场景(过场动画)
		barsDown();//卡槽下降动画
		show_Start_Sequence();//绘制开始动画

		loopMain();
	}

	GameEventSystem::getInstance()->destroyInstance();
	return 0;
}