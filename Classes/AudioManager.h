// AudioManager.h
#ifndef __AUDIO_MANAGER_H__
#define __AUDIO_MANAGER_H__

#include "cocos2d.h"
#include "audio/include/AudioEngine.h"

class AudioManager {
public:
    static AudioManager* getInstance();
    bool init();

    // 背景音乐控制
    void playBackgroundMusic(const std::string& filename, bool loop = true);
    void stopBackgroundMusic();
    void pauseBackgroundMusic();
    void resumeBackgroundMusic();

    // 音量控制
    void setBackgroundMusicVolume(float volume);
    float getBackgroundMusicVolume() const;

    // 状态查询
    bool isBackgroundMusicPlaying() const;

    // 调试信息
    void logAudioStatus();

private:
    AudioManager() : isInitialized(false), bgmId(cocos2d::experimental::AudioEngine::INVALID_AUDIO_ID),
        bgmVolume(0.8f), currentBGM("") {
    }
    virtual ~AudioManager();

    static AudioManager* instance;
    bool isInitialized;
    int bgmId;  // 当前背景音乐的ID
    float bgmVolume;  // 当前音量
    std::string currentBGM;  // 当前播放的音乐文件名
};

#endif // __AUDIO_MANAGER_H__