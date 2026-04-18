#include "AudioManager.h"
#include <cstdio>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

// 初始化静态成员
AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() : instanceCounter(0) {
    // 确保MCI音频设备可用
    MCIDEVICEID deviceId;
    MCI_OPEN_PARMS openParms = { 0 };
    openParms.lpstrDeviceType = "waveaudio";

    if (mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_TYPE_ID,
        (DWORD_PTR)&openParms) == 0) {
        deviceId = openParms.wDeviceID;
        mciSendCommand(deviceId, MCI_CLOSE, 0, 0);
    }
}

AudioManager::~AudioManager() {
    stopAll();
    cleanup();
}

AudioManager* AudioManager::getInstance() {
    if (!instance) {
        instance = new AudioManager();
    }
    return instance;
}

void AudioManager::releaseInstance() {
    if (instance) {
        delete instance;
        instance = nullptr;
    }
}

std::string AudioManager::generateAlias(const std::string& filename) {
    std::stringstream ss;
    ss << "audio_" << instanceCounter++ << "_" << time(NULL);
    return ss.str();
}

bool AudioManager::execMCI(const std::string& command, std::string* error) {
    char buffer[256] = { 0 };
    MCIERROR mciError = mciSendString(command.c_str(), buffer, sizeof(buffer), NULL);

    if (mciError != 0) {
        if (error) {
            *error = buffer;
        }
        return false;
    }

    return true;
}

AudioStatus AudioManager::getActualStatus(const std::string& alias) {
    char status[256] = { 0 };
    std::string command = "status " + alias + " mode";

    if (mciSendString(command.c_str(), status, sizeof(status), NULL) != 0) {
        return STATUS_STOPPED;
    }

    std::string statusStr(status);
    if (statusStr == "playing") return STATUS_PLAYING;
    if (statusStr == "paused") return STATUS_PAUSED;
    return STATUS_STOPPED;
}

void AudioManager::updateAudioStatus(const std::string& filename, AudioInfo& info) {
    AudioStatus actualStatus = getActualStatus(info.alias);

    // 如果状态不一致，更新状态
    if (actualStatus != info.status) {
        info.status = actualStatus;

        // 如果播放完成，处理循环逻辑
        if (actualStatus == STATUS_STOPPED && info.status == STATUS_PLAYING) {
            if (info.loopMode == LOOP_COUNT && info.loopCount > 0) {
                info.loopCount--;
                if (info.loopCount > 0) {
                    // 重新开始播放
                    std::string command = "play " + info.alias + " from 0";
                    if (execMCI(command)) {
                        info.status = STATUS_PLAYING;
                    }
                }
                else {
                    // 循环结束，标记为需要清理
                    if (info.autoRelease) {
                        cleanupList.push_back(filename);
                    }
                }
            }
            else if (info.loopMode == LOOP_INFINITE) {
                // 无限循环，重新开始播放
                std::string command = "play " + info.alias + " from 0";
                if (execMCI(command)) {
                    info.status = STATUS_PLAYING;
                }
            }
            else {
                // 非循环音频播放完成
                if (info.autoRelease) {
                    cleanupList.push_back(filename);
                }
            }
        }
    }
}

bool AudioManager::play(const std::string& filename, AudioLoopMode loopMode,
    int loopCount, int volume, bool autoRelease) {

    // 如果音频已加载，先停止并关闭
    auto it = audioMap.find(filename);
    if (it != audioMap.end()) {
        stop(filename);
        unload(filename);
    }

    // 生成唯一别名
    std::string alias = generateAlias(filename);

    // 确定设备类型
    std::string openCmd;
    size_t dotPos = filename.find_last_of('.');
    std::string ext = (dotPos != std::string::npos) ?
        filename.substr(dotPos + 1) : "";

    // 转换为小写
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "mp3") {
        openCmd = "open \"" + filename + "\" type MPEGVideo alias " + alias;
    }
    else {
        openCmd = "open \"" + filename + "\" type waveaudio alias " + alias;
    }

    // 打开音频文件
    if (!execMCI(openCmd)) {
        return false;
    }

    // 设置音量
    std::string volumeCmd = "setaudio " + alias + " volume to " + std::to_string(volume);
    execMCI(volumeCmd);

    // 构建播放命令
    std::string playCmd;
    if (loopMode == LOOP_INFINITE) {
        playCmd = "play " + alias + " repeat";
    }
    else {
        playCmd = "play " + alias;
    }

    // 播放音频
    if (!execMCI(playCmd)) {
        execMCI("close " + alias);
        return false;
    }

    // 保存音频信息
    AudioInfo info;
    info.alias = alias;
    info.status = STATUS_PLAYING;
    info.loopMode = loopMode;
    info.loopCount = (loopMode == LOOP_COUNT) ? loopCount : 0;
    info.originalLoopCount = info.loopCount;
    info.volume = volume;
    info.autoRelease = autoRelease;
    info.is3D = false;

    audioMap[filename] = info;
    return true;
}

bool AudioManager::stop(const std::string& filename) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return false;
    }

    AudioInfo& info = it->second;
    std::string command = "stop " + info.alias;

    if (execMCI(command)) {
        info.status = STATUS_STOPPED;
        return true;
    }

    return false;
}

bool AudioManager::pause(const std::string& filename) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return false;
    }

    AudioInfo& info = it->second;
    if (info.status != STATUS_PLAYING) {
        return false;
    }

    std::string command = "pause " + info.alias;
    if (execMCI(command)) {
        info.status = STATUS_PAUSED;
        return true;
    }

    return false;
}

bool AudioManager::resume(const std::string& filename) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return false;
    }

    AudioInfo& info = it->second;
    if (info.status != STATUS_PAUSED) {
        return false;
    }

    std::string command = "play " + info.alias;
    if (info.loopMode == LOOP_INFINITE) {
        command += " repeat";
    }

    if (execMCI(command)) {
        info.status = STATUS_PLAYING;
        return true;
    }

    return false;
}

void AudioManager::stopAll() {
    for (auto& pair : audioMap) {
        AudioInfo& info = pair.second;
        if (info.status == STATUS_PLAYING || info.status == STATUS_PAUSED) {
            execMCI("stop " + info.alias);
            info.status = STATUS_STOPPED;
        }
    }
}

bool AudioManager::setVolume(const std::string& filename, int volume) {
    if (volume < 0) volume = 0;
    if (volume > 1000) volume = 1000;

    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return false;
    }

    AudioInfo& info = it->second;
    info.volume = volume;

    std::string command = "setaudio " + info.alias + " volume to " + std::to_string(volume);
    return execMCI(command);
}

int AudioManager::getVolume(const std::string& filename) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return 0;
    }

    return it->second.volume;
}

bool AudioManager::setPosition(const std::string& filename, int milliseconds) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return false;
    }

    AudioInfo& info = it->second;
    std::string command = "play " + info.alias + " from " + std::to_string(milliseconds);

    if (execMCI(command)) {
        info.status = STATUS_PLAYING;
        return true;
    }

    return false;
}

int AudioManager::getPosition(const std::string& filename) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return 0;
    }

    AudioInfo& info = it->second;
    char position[256] = { 0 };
    std::string command = "status " + info.alias + " position";

    if (mciSendString(command.c_str(), position, sizeof(position), NULL) != 0) {
        return 0;
    }

    return atoi(position);
}

int AudioManager::getLength(const std::string& filename) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return 0;
    }

    AudioInfo& info = it->second;
    char length[256] = { 0 };
    std::string command = "status " + info.alias + " length";

    if (mciSendString(command.c_str(), length, sizeof(length), NULL) != 0) {
        return 0;
    }

    return atoi(length);
}

bool AudioManager::isPlaying(const std::string& filename) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return false;
    }

    return it->second.status == STATUS_PLAYING;
}

bool AudioManager::setLoopCount(const std::string& filename, int count) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return false;
    }

    AudioInfo& info = it->second;
    if (info.loopMode != LOOP_COUNT) {
        return false;
    }

    info.loopCount = count;
    info.originalLoopCount = count;
    return true;
}

bool AudioManager::preload(const std::string& filename) {
    // 如果已加载，直接返回成功
    if (audioMap.find(filename) != audioMap.end()) {
        return true;
    }

    std::string alias = generateAlias(filename);

    // 确定设备类型
    std::string openCmd;
    size_t dotPos = filename.find_last_of('.');
    std::string ext = (dotPos != std::string::npos) ?
        filename.substr(dotPos + 1) : "";

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "mp3") {
        openCmd = "open \"" + filename + "\" type MPEGVideo alias " + alias;
    }
    else {
        openCmd = "open \"" + filename + "\" type waveaudio alias " + alias;
    }

    if (!execMCI(openCmd)) {
        return false;
    }

    // 保存音频信息但不播放
    AudioInfo info;
    info.alias = alias;
    info.status = STATUS_STOPPED;
    info.loopMode = LOOP_NONE;
    info.loopCount = 0;
    info.originalLoopCount = 0;
    info.volume = 800;
    info.autoRelease = false;
    info.is3D = false;

    audioMap[filename] = info;
    return true;
}

bool AudioManager::unload(const std::string& filename) {
    auto it = audioMap.find(filename);
    if (it == audioMap.end()) {
        return true;
    }

    AudioInfo& info = it->second;
    std::string command = "close " + info.alias;

    if (execMCI(command)) {
        audioMap.erase(it);
        return true;
    }

    return false;
}

void AudioManager::update() {
    // 更新所有音频状态
    for (auto& pair : audioMap) {
        updateAudioStatus(pair.first, pair.second);
    }

    // 清理已完成的音频
    cleanup();
}

void AudioManager::cleanup() {
    for (const std::string& filename : cleanupList) {
        unload(filename);
    }
    cleanupList.clear();
}

int AudioManager::getPlayingCount() {
    int count = 0;
    for (auto& pair : audioMap) {
        if (pair.second.status == STATUS_PLAYING) {
            count++;
        }
    }
    return count;
}