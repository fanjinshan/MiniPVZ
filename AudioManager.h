#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <string>
#include <map>
#include <vector>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// 音频循环模式
enum AudioLoopMode {
    LOOP_NONE = 0,      // 不循环
    LOOP_COUNT,         // 有限次数循环
    LOOP_INFINITE       // 无限循环
};

// 音频状态
enum AudioStatus {
    STATUS_STOPPED,
    STATUS_PLAYING,
    STATUS_PAUSED
};

// 音频信息结构
struct AudioInfo {
    std::string alias;           // MCI别名
    AudioStatus status;          // 当前状态
    AudioLoopMode loopMode;      // 循环模式
    int loopCount;               // 剩余循环次数
    int originalLoopCount;       // 原始循环次数
    bool is3D;                   // 是否3D音效
    int volume;                  // 音量 (0-1000)
    bool autoRelease;            // 播放完成后自动释放
};

class AudioManager {
private:
    // 单例模式
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    static AudioManager* instance;

    std::map<std::string, AudioInfo> audioMap;      // 音频映射
    std::vector<std::string> cleanupList;           // 待清理列表
    int instanceCounter;                            // 实例计数器

    // 生成唯一别名
    std::string generateAlias(const std::string& filename);

    // 执行MCI命令
    bool execMCI(const std::string& command, std::string* error = nullptr);

    // 获取音频状态
    AudioStatus getActualStatus(const std::string& alias);

    // 检查并更新音频状态
    void updateAudioStatus(const std::string& filename, AudioInfo& info);

public:
    // 获取单例实例
    static AudioManager* getInstance();

    // 释放单例实例
    static void releaseInstance();

    // 播放音频
    // filename: 音频文件路径
    // loopMode: 循环模式 (LOOP_NONE, LOOP_COUNT, LOOP_INFINITE)
    // loopCount: 循环次数 (当loopMode为LOOP_COUNT时有效)
    // volume: 音量 (0-1000)
    // autoRelease: 播放完成后自动释放资源
    bool play(const std::string& filename,
        AudioLoopMode loopMode = LOOP_NONE,
        int loopCount = 1,
        int volume = 800,
        bool autoRelease = true);

    // 停止播放
    bool stop(const std::string& filename);

    // 暂停播放
    bool pause(const std::string& filename);

    // 恢复播放
    bool resume(const std::string& filename);

    // 停止所有音频
    void stopAll();

    // 设置音量
    bool setVolume(const std::string& filename, int volume);

    // 获取当前音量
    int getVolume(const std::string& filename);

    // 设置播放位置 (毫秒)
    bool setPosition(const std::string& filename, int milliseconds);

    // 获取播放位置 (毫秒)
    int getPosition(const std::string& filename);

    // 获取音频长度 (毫秒)
    int getLength(const std::string& filename);

    // 是否正在播放
    bool isPlaying(const std::string& filename);

    // 设置循环次数 (仅对LOOP_COUNT模式有效)
    bool setLoopCount(const std::string& filename, int count);

    // 预加载音频 (提前打开但不播放)
    bool preload(const std::string& filename);

    // 卸载音频 (释放资源)
    bool unload(const std::string& filename);

    // 更新音频管理器 (必须每帧调用)
    void update();

    // 清理所有已完成的音频
    void cleanup();

    // 获取已加载音频数量
    int getLoadedCount() const { return (int)audioMap.size(); }

    // 获取正在播放的音频数量
    int getPlayingCount();

    // 设置最大同时播放数量 (软限制)
    void setMaxSimultaneous(int max) { /* 可选实现 */ }
};

#endif // AUDIO_MANAGER_H