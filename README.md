---

# 🌻 植物大战僵尸 (PVZ) - C++ 学习版
## 📖 项目简介

本项目是基于 Windows 平台，使用 **C++** 语言和 **EasyX** 图形库实现的《植物大战僵尸》简化版游戏。旨在通过复刻经典游戏的核心玩法，深入理解游戏开发中的面向对象设计、资源管理、碰撞检测、动画状态机以及事件驱动架构。

> **注意**：本项目仅供学习和交流使用，所有游戏素材（图片、音频）版权归 PopCap Games / EA 所有，严禁用于商业用途。

## ✨ 主要功能

### 游戏界面
<img width="902" height="632" alt="QQ_1776516759790" src="https://github.com/user-attachments/assets/5d8b2988-5e3c-4827-9edd-b905019522d7" />

<img width="902" height="632" alt="QQ_1776516739558" src="https://github.com/user-attachments/assets/b52f9ef4-5b85-4c5e-a191-d2678be05aa2" />

### 🌱 植物系统
- **豌豆射手 (Peashooter)**：发射普通豌豆攻击僵尸。
- **向日葵 (Sunflower)**：定期生产阳光，提供经济来源。
- **坚果墙 (Wall-nut)**：高血量防御植物，阻挡僵尸前进。
- **土豆雷 (Potato Mine)**：需要准备时间，爆炸后对范围内僵尸造成巨额伤害。
- **双发射手 (Repeater)**：一次发射两颗豌豆。
- **寒冰射手 (Snow Pea)**：发射冰豌豆，减缓僵尸移动速度。
- **大嘴花 (Chomper)**：可以瞬间吞噬靠近的僵尸，但咀嚼期间无法行动。

### 🧟 僵尸系统
- **普通僵尸**：基础敌人。
- **路障僵尸**：拥有路障头盔，血量较高。
- **铁桶僵尸**：拥有铁桶头盔，血量极高。
- **小鬼僵尸**：移动速度快，血量低。
- **橄榄球僵尸**：移动速度快，血量高。
- **铁门僵尸**：拥有铁门护盾，防御力强。
- *包含行走、进食、死亡等多种动画状态。*

### ☀️ 核心机制
- **阳光系统**：自然掉落与向日葵生产，支持贝塞尔曲线动画收集效果。
- **种植系统**：拖拽卡牌种植，铲子移除植物。
- **战斗系统**：
  - 子弹与僵尸的碰撞检测及伤害计算。
  - 僵尸与植物的碰撞检测及进食逻辑。
  - 特殊植物技能判定（如土豆雷爆炸、大嘴花吞噬）。
- **关卡流程**：
  - 动态难度调整（随时间增加僵尸生成频率）。
  - 开场动画与过场演出。
  - 胜利/失败判定及结算画面。

### 🔊 音效与事件
- **音频管理**：基于 MCI (`winmm.lib`) 封装的 [AudioManager](file://\develop\vs2022\demo\PVZ\PVZ\AudioManager.h#L36-L134)，支持多音效并发播放、音量控制及自动释放。
- **事件系统**：基于对象池 ([GameEventPool](file://\develop\vs2022\demo\PVZ\PVZ\event.h#L97-L126)) 和队列 ([GameEventQueue](file://\develop\vs2022\demo\PVZ\PVZ\event.h#L129-L172))实现的高效事件分发机制解耦游戏逻辑与表现层。

## 🛠️ 技术栈

- **编程语言**: C/C++11
- **开发环境**: Visual Studio 2022
- **图形库**: [EasyX](https://easyx.cn/) (2024版本)
- **音频库**: Windows MCI API (`winmm.lib`)
- **操作系统**: Windows 10/11

## 📂 项目结构

```text
PVZ/
├── main.cpp            # 主程序入口，游戏循环，全局状态管理
├── event.h/cpp         # 事件系统（单例模式、对象池、事件队列）
├── tools.h/cpp         # 工具函数（PNG透明绘制、延时计算）
├── vector2.h/cpp       # 二维向量数学库（用于物理计算和贝塞尔曲线）
├── AudioManager.h/cpp  # 音频管理器（MCI封装，单例模式）
├── res/                # 资源文件夹
│   ├── audio/          # 音效文件 (.wav, .mp3)
│   ├── Cards/          # 植物卡牌图片
│   ├── Plants/         # 植物动画序列帧
│   ├── zm/             # 僵尸动画序列帧
│   ├── bullets/        # 子弹图片
│   └── ...             # 其他UI资源
└── README.md           # 项目说明文档
```

## 🚀 快速开始

### 前置要求
1. 安装 **Visual Studio 2022** (或更高版本)。
2. 安装 **EasyX 图形库**。
   - 访问 [EasyX 官网](https://easyx.cn/) 下载并安装最新版本。
   - 确保在安装时选择了对应的 VS 版本进行集成。

### 编译运行
1. 克隆或下载本项目代码。
2. 使用 Visual Studio 打开 `.sln` 解决方案文件或创建新项目并添加所有 [.cpp](file://\develop\vs2022\demo\PVZ\PVZ\main.cpp) 和 [.h](file://\develop\vs2022\demo\PVZ\PVZ\event.h) 文件。
3. 确保 `res` 文件夹与可执行文件在同一目录下，或者在代码中修正资源路径。
4. 点击“生成” -> “生成解决方案”。
5. 点击“调试” -> “开始执行(不调试)”运行游戏。

## 🎮 操作指南

- **鼠标左键**：
  - 点击顶部卡牌选中植物。
  - 在草坪网格内点击种植。
  - 点击掉落的阳光进行收集。
  - 点击铲子图标，再点击植物可铲除。
- **暂停按钮**：点击左上角暂停/继续游戏。


## 📝 核心模块说明

### 1. 事件系统 (`event.h/cpp`)
采用**对象池模式**管理 [GameEventNode](file://\develop\vs2022\demo\PVZ\PVZ\event.h#L42-L93)，避免频繁内存分配带来的开销。[GameEventSystem](file://\develop\vs2022\demo\PVZ\PVZ\event.h#L175-L222) 作为单例，负责事件的发送 ([sendEvent](file://\develop\vs2022\demo\PVZ\PVZ\event.h#L200-L200)) 和处理 ([processEvents](file://\develop\vs2022\demo\PVZ\PVZ\event.h#L203-L203))。这种设计将逻辑触发（如种植、击杀）与具体表现（如播放音效、更新分数）解耦。

### 2. 音频管理 (`AudioManager.h/cpp`)
封装了 Windows MCI 命令，实现了简单的音频生命周期管理。支持预加载 ([preload](file://\develop\vs2022\demo\PVZ\PVZ\AudioManager.h#L115-L115))、播放、停止、暂停及自动清理 ([autoRelease](file://\develop\vs2022\demo\PVZ\PVZ\AudioManager.h#L33-L33))，解决了传统 `PlaySound` 无法同时播放多个音效的问题。

### 3. 图形渲染 (`tools.h/cpp`)
提供了 [putimagePNG](file://\develop\vs2022\demo\PVZ\PVZ\tools.h#L3-L3) 函数，通过逐像素 Alpha 混合算法实现 PNG 图片的透明背景绘制，使得游戏画面更加精致。

### 4. 向量数学 (`vector2.h/cpp`)
实现了基本的二维向量运算及贝塞尔曲线计算 ([calcBezierPoint](file://\develop\vs2022\demo\PVZ\PVZ\vector2.h#L47-L47))，用于处理阳光收集的平滑轨迹动画。


## 📄 许可证

本项目代码遵循 MIT 许可证。
游戏素材版权归原版权方所有。

---

*Happy Coding! 🌱🧟‍♂️*
