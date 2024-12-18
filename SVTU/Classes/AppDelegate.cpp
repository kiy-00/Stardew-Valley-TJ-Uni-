/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 http://www.cocos2d-x.org

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"
#include "GameScene.h"
#include "HelloWorldScene.h"
#include "MyScene.h"
#include "PhysicsTestScene.h"

// #define USE_AUDIO_ENGINE 1
// #define USE_SIMPLE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE && USE_SIMPLE_AUDIO_ENGINE
#error                                                                         \
    "Don't use AudioEngine and SimpleAudioEngine at the same time. Please just select one in your game!"
#endif

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#elif USE_SIMPLE_AUDIO_ENGINE
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
#endif

USING_NS_CC;

// 修改设计分辨率为地图尺寸
static cocos2d::Size designResolutionSize =
    cocos2d::Size(1600, 1600); // 改为地图实际大小
static cocos2d::Size smallResolutionSize =
    cocos2d::Size(1280, 720); // 小屏幕（备用）
static cocos2d::Size mediumResolutionSize =
    cocos2d::Size(1920, 1080); // 中等屏幕（备用）
static cocos2d::Size largeResolutionSize =
    cocos2d::Size(2560, 1440); // 大屏幕（备用）

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {
#if USE_AUDIO_ENGINE
  AudioEngine::end();
#elif USE_SIMPLE_AUDIO_ENGINE
  SimpleAudioEngine::end();
#endif
}

// 如果需要不同的上下文，修改 glContextAttrs 的值
void AppDelegate::initGLContextAttrs() {
  // 设置 OpenGL 上下文属性：
  // red, green, blue, alpha, depth, stencil, multisamplesCount
  GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

  GLView::setGLContextAttrs(glContextAttrs);
}

// 如果使用包管理器安装更多包，不要修改或删除此函数
static int register_all_packages() {
  return 0; // 包管理器的标志
}

bool AppDelegate::applicationDidFinishLaunching() {
  // 初始化 director
  auto director = Director::getInstance();
  auto glview = director->getOpenGLView();
  if (!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) ||                               \
    (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) ||                                 \
    (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)

    glview =
        GLViewImpl::createWithRect("SVTU", cocos2d::Rect(0, 0, 1600, 1600));
#else
    glview = GLViewImpl::create("SVTU");
#endif
    director->setOpenGLView(glview);
  }

  // 清除纹理缓存（如果需要）
  // director->getTextureCache()->removeAllTextures();

  // 清除精灵帧缓存（如果需要）
  // SpriteFrameCache::getInstance()->removeSpriteFrames();

  // 打开显示 FPS
  director->setDisplayStats(true);

  // 设置 FPS。默认值是 1.0/60，如果不调用此函数
  director->setAnimationInterval(1.0f / 60);

  // 设置设计分辨率,使用 SHOW_ALL 策略确保地图完整显示且不变形
  glview->setDesignResolutionSize(1600, 1600, ResolutionPolicy::SHOW_ALL);
  auto frameSize = glview->getFrameSize();

  // 设置内容缩放因子（可选，根据需要调整）
  // 由于设计分辨率与地图大小一致，这里可以将 contentScaleFactor 设置为 1
  director->setContentScaleFactor(1.0f);

  register_all_packages();

  // 创建并运行初始场景
  // auto scene = HelloWorldScene::createScene();

  auto scene = GameScene::createScene();
  // auto scene = MyScene::createScene();
  // auto scene = PhysicsTestScene::createScene();

  // 运行场景
  director->runWithScene(scene);

  return true;
}

// 当应用程序进入后台时调用。注意，当接收到电话时会调用此函数。
void AppDelegate::applicationDidEnterBackground() {
  Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
  AudioEngine::pauseAll();
#elif USE_SIMPLE_AUDIO_ENGINE
  SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
  SimpleAudioEngine::getInstance()->pauseAllEffects();
#endif
}

// 当应用程序重新进入前台时调用
void AppDelegate::applicationWillEnterForeground() {
  Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
  AudioEngine::resumeAll();
#elif USE_SIMPLE_AUDIO_ENGINE
  SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
  SimpleAudioEngine::getInstance()->resumeAllEffects();
#endif
}
