// AudioManager.cpp
#include "AudioManager.h"

USING_NS_CC;

AudioManager* AudioManager::instance = nullptr;

AudioManager* AudioManager::getInstance() {
    if (!instance) {
        instance = new AudioManager();
        instance->init();
    }
    return instance;
}

bool AudioManager::init() {
    if (isInitialized) {
        return true;
    }

    // experimental::AudioEngine 不需要显式初始化
    isInitialized = true;
    bgmId = experimental::AudioEngine::INVALID_AUDIO_ID;
    bgmVolume = 0.8f;

    CCLOG("AudioManager initialized successfully");
    return true;
}

AudioManager::~AudioManager() {
    stopBackgroundMusic();
    experimental::AudioEngine::end();
}

void AudioManager::playBackgroundMusic(const std::string& filename, bool loop) {
    if (!isInitialized) {
        CCLOG("AudioManager not initialized");
        return;
    }

    // 检查文件是否存在
    if (!FileUtils::getInstance()->isFileExist(filename)) {
        CCLOG("Music file not found: %s", filename.c_str());
        return;
    }

    // 如果当前有音乐在播放，先停止它
    stopBackgroundMusic();

    // 播放新的音乐
    bgmId = experimental::AudioEngine::play2d(filename, loop, bgmVolume);
    if (bgmId != experimental::AudioEngine::INVALID_AUDIO_ID) {
        currentBGM = filename;
        CCLOG("Started playing background music: %s", filename.c_str());

        // 设置播放完成回调
        experimental::AudioEngine::setFinishCallback(bgmId, [this](int id, const std::string& filePath) {
            if (id == bgmId) {
                bgmId = experimental::AudioEngine::INVALID_AUDIO_ID;
                CCLOG("Background music finished playing: %s", filePath.c_str());
            }
            });
    }
    else {
        CCLOG("Failed to play background music");
    }
}

void AudioManager::stopBackgroundMusic() {
    if (bgmId != experimental::AudioEngine::INVALID_AUDIO_ID) {
        experimental::AudioEngine::stop(bgmId);
        bgmId = experimental::AudioEngine::INVALID_AUDIO_ID;
        currentBGM = "";
        CCLOG("Stopped background music");
    }
}

void AudioManager::pauseBackgroundMusic() {
    if (bgmId != experimental::AudioEngine::INVALID_AUDIO_ID) {
        experimental::AudioEngine::pause(bgmId);
        CCLOG("Paused background music");
    }
}

void AudioManager::resumeBackgroundMusic() {
    if (bgmId != experimental::AudioEngine::INVALID_AUDIO_ID) {
        experimental::AudioEngine::resume(bgmId);
        CCLOG("Resumed background music");
    }
}

void AudioManager::setBackgroundMusicVolume(float volume) {
    bgmVolume = std::max(0.0f, std::min(1.0f, volume));
    if (bgmId != experimental::AudioEngine::INVALID_AUDIO_ID) {
        experimental::AudioEngine::setVolume(bgmId, bgmVolume);
        CCLOG("Set background music volume to: %.2f", bgmVolume);
    }
}

float AudioManager::getBackgroundMusicVolume() const {
    return bgmVolume;
}

bool AudioManager::isBackgroundMusicPlaying() const {
    if (bgmId != experimental::AudioEngine::INVALID_AUDIO_ID) {
        return experimental::AudioEngine::getState(bgmId) == experimental::AudioEngine::AudioState::PLAYING;
    }
    return false;
}

void AudioManager::logAudioStatus() {
    CCLOG("=== Audio Manager Status ===");
    CCLOG("Is Initialized: %d", isInitialized);
    CCLOG("Current BGM: %s", currentBGM.empty() ? "None" : currentBGM.c_str());
    CCLOG("BGM ID: %d", bgmId);
    CCLOG("Is Playing: %d", isBackgroundMusicPlaying());
    CCLOG("Volume: %.2f", bgmVolume);
    CCLOG("=========================");
}