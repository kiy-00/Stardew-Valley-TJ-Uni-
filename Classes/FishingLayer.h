#ifndef __FHSING_SCENE_H__
#define __FHSING_SCENE_H__

#include <FishInfo.h>
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "platform/CCFileUtils.h"
USING_NS_CC;

/**
 * @class FishingLayerProperty
 * @brief 存储钓鱼场景的属性配置
 */
class FishingLayerProperty
{
public:
    float timeCounter;                // 时间计时器（单位：秒）
    int fishYMin;                     // 鱼的最低高度
    int fishYMax;                     // 鱼的最高高度
    int fishRandomY;                  // 鱼的随机高度
    int fishMoveSpeed;                // 鱼的移动速度
    int sliderYMin;                   // 滑块的最低高度
    int sliderYMax;                   // 滑块的最高高度
    int sliderOriginMoveSpeed;        // 滑块的初始移动速度
    int sliderMoveSpeed;              // 滑块的动态移动速度
    float loadingBarOriginMoveSpeed;  // 进度条的初始增长速度（范围：1-100）
    float loadingBarMoveSpeed;        // 进度条的动态增长速度（范围：1-100）

    /**
     * @brief 构造函数
     * 初始化所有属性为默认值
     */
    FishingLayerProperty();
};


/**
 * @class FishingLayer
 * @brief 钓鱼场景的主要层类
 */
class FishingLayer : public Layer
{
public:
    /**
     * @brief 创建并返回一个钓鱼场景
     * @return 指向创建的 Scene 对象的指针
     */
    static Scene* createScene();

    /**
     * @brief 初始化层
     * @return 初始化成功返回 true，失败返回 false
     */
    virtual bool init();

    /**
     * @brief 获取当前钓鱼场景的参数
     * @return 包含参数的键值对
     */
    std::unordered_map<std::string, int> getParams();


    /**
     * @brief 设置目标鱼的信息。
     * @param fish 目标鱼的信息结构体。
     */
    void setTargetFish(const FishInfo& fish);

    /**
     * @brief 设置小游戏结束时的回调函数。
     * @param callback 回调函数，参数为成功/失败和鱼的信息。
     */
    std::function<void(bool success, const FishInfo& fish)> onGameEnd;

	CREATE_FUNC(FishingLayer);  // 宏定义用于创建实例

	void initUI();           // 初始化用户界面（UI）
	void initKeyListener();  // 初始化键盘输入监听器
	void initSchedule();     // 初始化定时器调度
	void initData();         // 初始化数据

    void handleSliderAction(int direction);                         // 处理滑块的移动操作
    void onKeyPressed(EventKeyboard::KeyCode code, Event* event);   // 处理按键按下事件
    void onKeyReleased(EventKeyboard::KeyCode code, Event* event);  // 处理按键释放事件
    bool onTouchBegan(Touch* touch, Event* event);                  // 处理触摸开始事件
    void onTouchEnded(Touch* touch, Event* event);                  // 处理触摸结束事件

    virtual void onExit() override;           // 覆盖 Layer 的 onExit 方法
    void update(float dt) override;           // 更新方法，用于定时器调度
    void updateFish(float dt);                // 更新鱼的状态
    void updateSlider(float dt);              // 更新滑块的状态
    void updateLoadingBar(float dt);          // 更新进度条的状态
    Color3B interpolateColor(float percent);  // 进度条颜色平滑处理

private:
	Node* baseNode;               // 根节点
	ui::LoadingBar* LoadingBarUI; // 进度条 UI
	FishInfo _targetFish;         // 目标鱼的信息
    FishingLayerProperty _fishingLayerProperty;  // 用于在整个钓鱼场景中共享和管理动态状态信息
};

#endif // __FHSING_SCENE_H__