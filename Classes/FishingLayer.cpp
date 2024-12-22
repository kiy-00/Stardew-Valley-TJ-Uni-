#include "cocostudio/CocoStudio.h"
#include "extensions/cocos-ext.h"
#include "ui/CocosGUI.h"
#include "platform/CCFileUtils.h"
#include "FishingLayer.h"

using namespace cocos2d;
using namespace cocostudio;
using namespace ui;

////////////////////////////////////////////////////////////////
// FishingLayerProperty结构体是用于存储本层所需的参数和状态信息。
// 包含时间计时、鱼与滑块的运动范围和速度、进度条移动速度等属性。
////////////////////////////////////////////////////////////////
FishingLayerProperty::FishingLayerProperty()
{
	this->timeCounter = 0.0f;                // 时间计时,单位秒
	this->fishYMin = 0;                      // 鱼最低高度
	this->fishYMax = 0;                      // 鱼最高高度
	this->fishRandomY = 0;                   // 鱼随机高度(用于鱼在滑条范围内的随机位置)
	this->fishMoveSpeed = 200;               // 鱼移动速度(像素/秒)
	this->sliderYMin = 0;                    // 滑块最低高度
	this->sliderYMax = 0;                    // 滑块最高高度
	this->sliderOriginMoveSpeed = 200;       // 滑块原始移动速度(像素/秒)
	this->sliderMoveSpeed = 0;               // 滑块当前移动速度(像素/秒)
	this->loadingBarOriginMoveSpeed = 0.3f;  // 进度条原始增长速度(1~100)
	this->loadingBarMoveSpeed = 0;           // 进度条当前增长速度(1~100)
}

////////////////////////////////////////////////////////////
// 创建FishingLayer场景的方法。
// @return Scene* 返回一个包含FishingLayer的场景对象。
////////////////////////////////////////////////////////////
Scene* FishingLayer::createScene()
{
	auto scene = Scene::create();
	auto layer = FishingLayer::create();
	scene->addChild(layer);
	return scene;
}

////////////////////////////////////////////////////////////
// 初始化FishingLayer，完成UI初始化、事件监听、数据初始化与定时调度。
////////////////////////////////////////////////////////////
bool FishingLayer::init()
{
	baseNode = nullptr;
	this->initUI();
	this->initKeyListener();
	this->initSchedule();
	this->initData();
	return true;
}

////////////////////////////////////////////////////////////
// 初始化UI，包括加载csb文件与设置进度条初始值等。
////////////////////////////////////////////////////////////
void FishingLayer::initUI()
{
	// 从CSB文件中创建节点树
	Node* pRoot = CSLoader::getInstance()->createNodeWithFlatBuffersFile("FishingLayer.csb");
	auto bg = pRoot->getChildByName("bg");
	addChild(pRoot);
	this->baseNode = pRoot;

	// 初始化LoadingBar的初始进度
	Node* LoadingBar = baseNode->getChildByName("LoadingBar");
	this->LoadingBarUI = (ui::LoadingBar*)LoadingBar;
	this->LoadingBarUI->setPercent(0);
}

////////////////////////////////////////////////////////////
// 初始化输入监听，包括键盘事件与触摸事件。
// 键盘事件：按下空格让滑块向上移动，松开空格让滑块向下移动。
// 触摸事件：触摸开始向上移动，触摸结束向下移动。
////////////////////////////////////////////////////////////
void FishingLayer::initKeyListener()
{
	// 创建键盘事件监听器
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(FishingLayer::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(FishingLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithFixedPriority(keyboardListener, -11);
	_keyboardListener = keyboardListener;

	// 创建单点触摸事件监听器
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(FishingLayer::onTouchBegan, this);
	listener->onTouchEnded = CC_CALLBACK_2(FishingLayer::onTouchEnded, this);
	_touchListener = listener;
	_eventDispatcher->addEventListenerWithFixedPriority(listener, 1);
}

////////////////////////////////////////////////////////////
// 初始化本层数据，包括计算鱼与滑块的上下限位置、鱼的随机目标位置等。
// 此函数在init()中调用。
////////////////////////////////////////////////////////////
void FishingLayer::initData()
{
	if (baseNode == nullptr) {
		return;
	}

	Node* bg = baseNode->getChildByName("bg");
	Node* shutiao = baseNode->getChildByName("shutiao");
	Node* yu = baseNode->getChildByName("yu");
	Node* huakuai = baseNode->getChildByName("huakuai");
	Node* LoadingBar = baseNode->getChildByName("LoadingBar");

	// 计算鱼上下移动的限制位置
	int fishYMin = shutiao->getPosition().y
		- shutiao->getAnchorPoint().y * shutiao->getContentSize().height
		+ yu->getContentSize().height * yu->getAnchorPoint().y;

	int fishYMax = shutiao->getPosition().y
		+ (1 - shutiao->getAnchorPoint().y) * shutiao->getContentSize().height
		+ yu->getContentSize().height * (yu->getAnchorPoint().y - 1);

	_fishingLayerProperty.fishYMin = fishYMin;
	_fishingLayerProperty.fishYMax = fishYMax;

	// 设定鱼的初始位置和随机目标位置
	int currentPositionY = yu->getPositionY();
	int fishMoveSpeed = _fishingLayerProperty.fishMoveSpeed;
	double randomNumber = static_cast<double>(rand()) / RAND_MAX;
	int fishRandomY = (int)(randomNumber * (fishYMax - fishYMin)) + fishYMin;
	int sign = ((fishRandomY - currentPositionY) >= 0) ? 1 : -1;
	fishMoveSpeed = sign * abs(fishMoveSpeed);

	_fishingLayerProperty.fishRandomY = fishRandomY;
	_fishingLayerProperty.fishMoveSpeed = fishMoveSpeed;
	yu->setPositionY(fishYMin); // 初始化鱼的位置在下限

	// 计算滑块上下移动的限制位置
	int sliderYMin = shutiao->getPosition().y
		- shutiao->getAnchorPoint().y * shutiao->getContentSize().height
		+ huakuai->getContentSize().height * huakuai->getAnchorPoint().y;

	int sliderYMax = shutiao->getPosition().y
		+ (1 - shutiao->getAnchorPoint().y) * shutiao->getContentSize().height
		+ huakuai->getContentSize().height * (huakuai->getAnchorPoint().y - 1);

	_fishingLayerProperty.sliderYMin = sliderYMin;
	_fishingLayerProperty.sliderYMax = sliderYMax;
}

////////////////////////////////////////////////////////////
// 初始化定时调度器，scheduleUpdate()会在每帧调用update函数。
////////////////////////////////////////////////////////////
void FishingLayer::initSchedule()
{
	scheduleUpdate();
}

////////////////////////////////////////////////////////////
// 获取本层参数的函数，当前实现为空，可根据需要返回参数。
// @return 返回一个std::unordered_map<std::string, int>的参数表
////////////////////////////////////////////////////////////
std::unordered_map<std::string, int> FishingLayer::getParams()
{
	std::unordered_map<std::string, int> hashMap;
	return hashMap;
}

////////////////////////////////////////////////////////////
// 保存当前鱼的信息
// @param 传入鱼的相关参数
////////////////////////////////////////////////////////////
void FishingLayer::setTargetFish(const FishInfo& fish)
{
	_targetFish = fish; // 保存目标鱼信息
}

////////////////////////////////////////////////////////////
// onExit回调，在退出层时清理资源与监听器等。
////////////////////////////////////////////////////////////
void FishingLayer::onExit()
{
	Layer::onExit();

	// 确保移除键盘监听器
	if (_keyboardListener) {
		_eventDispatcher->removeEventListener(_keyboardListener);
		_keyboardListener = nullptr;
	}

	// 确保移除触摸监听器
	if (_touchListener) {
		_eventDispatcher->removeEventListener(_touchListener);
		_touchListener = nullptr;
	}

	// 停止定时器调度
	this->unscheduleUpdate();
	this->baseNode = nullptr;
}


////////////////////////////////////////////////////////////
// 每帧调用的更新函数，根据dt(帧时间)更新鱼、滑块、进度条等元素的状态。
// @param dt 每帧所用时间，单位秒
////////////////////////////////////////////////////////////
void FishingLayer::update(float dt)
{
	this->updateFish(dt);
	this->updateSlider(dt);
	this->updateLoadingBar(dt);
}

////////////////////////////////////////////////////////////
// 更新鱼的状态，鱼会在上下限之间随机游动。
// @param dt 每帧所用时间，单位秒
////////////////////////////////////////////////////////////
void FishingLayer::updateFish(float dt)
{
	Node* bg = baseNode->getChildByName("bg");
	Node* shutiao = baseNode->getChildByName("shutiao");
	Node* yu = baseNode->getChildByName("yu");
	Node* huakuai = baseNode->getChildByName("huakuai");
	Node* LoadingBar = baseNode->getChildByName("LoadingBar");

	float timeCounter = _fishingLayerProperty.timeCounter;
	int fishYMin = _fishingLayerProperty.fishYMin;
	int fishYMax = _fishingLayerProperty.fishYMax;
	int fishRandomY = _fishingLayerProperty.fishRandomY;
	int fishMoveSpeed = _fishingLayerProperty.fishMoveSpeed;

	int currentPositionY = yu->getPositionY();

	// 如果鱼已经达到随机目标位置(或即将超过)，则重新生成随机目标位置
	if ((currentPositionY >= fishRandomY + dt * fishMoveSpeed && fishMoveSpeed >= 0)
		|| (currentPositionY <= fishRandomY + dt * fishMoveSpeed && fishMoveSpeed < 0))
	{
		currentPositionY = yu->getPositionY();
		fishMoveSpeed = _fishingLayerProperty.fishMoveSpeed;
		double randomNumber = static_cast<double>(rand()) / RAND_MAX;
		fishRandomY = (int)(randomNumber * (fishYMax - fishYMin)) + fishYMin;
		int sign = ((fishRandomY - currentPositionY) >= 0) ? 1 : -1;
		fishMoveSpeed = sign * abs(fishMoveSpeed);
		_fishingLayerProperty.fishRandomY = fishRandomY;
		_fishingLayerProperty.fishMoveSpeed = fishMoveSpeed;
	}

	// 根据速度更新鱼的Y坐标
	currentPositionY += dt * fishMoveSpeed;
	yu->setPositionY(currentPositionY);
}

////////////////////////////////////////////////////////////
// 更新滑块的位置，根据sliderMoveSpeed移动，且保持在限定范围内。
// @param dt 每帧所用时间，单位秒
////////////////////////////////////////////////////////////
void FishingLayer::updateSlider(float dt)
{
	Node* huakuai = baseNode->getChildByName("huakuai");
	int currentPositionY = huakuai->getPositionY();

	int sliderYMin = _fishingLayerProperty.sliderYMin;
	int sliderYMax = _fishingLayerProperty.sliderYMax;
	int sliderMoveSpeed = _fishingLayerProperty.sliderMoveSpeed;

	// 计算滑块的新位置并限制在上下限之间
	currentPositionY = currentPositionY + sliderMoveSpeed * dt;
	currentPositionY = MIN(currentPositionY, sliderYMax);
	currentPositionY = MAX(currentPositionY, sliderYMin);
	huakuai->setPositionY(currentPositionY);
}

////////////////////////////////////////////////////////////
// 更新进度条的状态。当滑块与鱼重合时，进度条增长，否则减少。
// 如果进度达到100%则表示胜利。
// @param dt 每帧所用时间，单位秒
////////////////////////////////////////////////////////////
void FishingLayer::updateLoadingBar(float dt)
{
	Node* LoadingBar = baseNode->getChildByName("LoadingBar");
	ui::LoadingBar* LoadingBarUI = static_cast<ui::LoadingBar*>(LoadingBar);

	float percent = LoadingBarUI->getPercent();

	// 获取鱼和滑块的包围盒，用来判断两者是否相交
	Node* yu = baseNode->getChildByName("yu");
	Rect yuBoundingBox = yu->getBoundingBox();
	yuBoundingBox.origin = yu->convertToWorldSpaceAR(Vec2());

	Node* huakuai = baseNode->getChildByName("huakuai");
	Rect huakuaiBoundingBox = huakuai->getBoundingBox();
	huakuaiBoundingBox.origin = huakuai->convertToWorldSpaceAR(Vec2());

	// 如果滑块与鱼相交，进度条增加，否则减少
	if (huakuaiBoundingBox.intersectsRect(yuBoundingBox)) {
		_fishingLayerProperty.loadingBarMoveSpeed = _fishingLayerProperty.loadingBarOriginMoveSpeed;
	}
	else {
		_fishingLayerProperty.loadingBarMoveSpeed = -_fishingLayerProperty.loadingBarOriginMoveSpeed;
	}

	// 更新进度条的百分比，并限制在 0 到 100 之间
	percent = percent + _fishingLayerProperty.loadingBarMoveSpeed;
	percent = MAX(percent, 0);
	percent = MIN(percent, 100);
	LoadingBarUI->setPercent(percent);

	// 平滑过渡颜色：小于 30% 时为红色，大于等于 30% 时逐渐过渡到绿色
	Color3B color = interpolateColor(percent);
	LoadingBarUI->setColor(color);

	printf("%.3f\n", percent);

	// 游戏结束逻辑
	if (percent >= 100.0f) {
		// 成功结束钓鱼游戏
		this->unscheduleUpdate();
		this->stopAllActions();

		if (onGameEnd) onGameEnd(true, _targetFish); // 成功
		
		// 延迟销毁，确保回调完成后
		this->runAction(Sequence::create(
			DelayTime::create(0.1f), 
			CallFunc::create([this]() { this->removeFromParent(); }),
			nullptr
		));
		return;
	}
	else if (percent <= 0.0f) {
		// 失败结束钓鱼游戏
		this->unscheduleUpdate();
		this->stopAllActions();

		if (onGameEnd) onGameEnd(false, _targetFish); // 失败
		
		// 延迟销毁，确保回调完成后
		this->runAction(Sequence::create(
			DelayTime::create(0.1f),
			CallFunc::create([this]() { this->removeFromParent(); }),
			nullptr
		));
		return;
	}
}

////////////////////////////////////////////////////////////
// 根据进度百分比平滑插值颜色。
// @param percent 当前进度百分比（0~100）
// @return Color3B 平滑插值后的颜色
////////////////////////////////////////////////////////////
Color3B FishingLayer::interpolateColor(float percent)
{
	// 定义起始颜色（红色）和目标颜色（绿色）
	Color3B startColor = Color3B::RED;
	Color3B endColor = Color3B::GREEN;

	// 计算插值比例，确保在 0 到 1 之间
	float t = percent / 100.0f;
	t = MAX(0.0f, MIN(t, 1.0f));

	// 线性插值计算 R、G、B 分量
	GLubyte r = startColor.r + t * (endColor.r - startColor.r);
	GLubyte g = startColor.g + t * (endColor.g - startColor.g);
	GLubyte b = startColor.b + t * (endColor.b - startColor.b);

	return Color3B(r, g, b);
}

////////////////////////////////////////////////////////////
// 滑块操作函数，根据传入的direction(1或-1)来设定滑块的移动速度。
// 滑块的移动速度被设置为鱼移动速度的两倍，并根据方向进行正负设置。
// @param direction 移动方向(1表示正向，-1表示反向)
////////////////////////////////////////////////////////////
void FishingLayer::handleSliderAction(int direction)
{
	float timeCounter = _fishingLayerProperty.timeCounter;
	int fishYMin = _fishingLayerProperty.fishYMin;
	int fishYMax = _fishingLayerProperty.fishYMax;
	int fishRandomY = _fishingLayerProperty.fishRandomY;
	int fishMoveSpeed = _fishingLayerProperty.fishMoveSpeed;
	int sliderYMin = _fishingLayerProperty.sliderYMin;
	int sliderYMax = _fishingLayerProperty.sliderYMax;
	int sliderOriginMoveSpeed = _fishingLayerProperty.sliderOriginMoveSpeed;
	int sliderMoveSpeed = _fishingLayerProperty.sliderMoveSpeed;

	// 设置滑块移动速度为鱼移动速度的两倍，并根据direction确定方向
	_fishingLayerProperty.sliderMoveSpeed = direction * 2 * abs(fishMoveSpeed);
}

////////////////////////////////////////////////////////////
// 键盘按下事件回调函数，当空格键按下时，使滑块向正方向移动。
// @param code 键盘按键码
// @param event 事件对象
////////////////////////////////////////////////////////////
void FishingLayer::onKeyPressed(EventKeyboard::KeyCode code, Event* event)
{
	log("onKeyPressed, keycode: %d", code);
	if (code == EventKeyboard::KeyCode::KEY_SPACE) {
		handleSliderAction(1);  // 正向移动
	}
}

////////////////////////////////////////////////////////////
// 键盘松开事件回调函数，当空格键松开时，使滑块向反方向移动。
// @param code 键盘按键码
// @param event 事件对象
////////////////////////////////////////////////////////////
void FishingLayer::onKeyReleased(EventKeyboard::KeyCode code, Event* event)
{
	log("onKeyReleased, keycode: %d", code);
	if (code == EventKeyboard::KeyCode::KEY_SPACE) {
		handleSliderAction(-1);  // 反向移动
	}
}

////////////////////////////////////////////////////////////
// 触摸开始事件回调函数，当用户触摸屏幕时，使滑块向正方向移动。
// @param touch 触摸对象
// @param event 事件对象
// @return true 表示事件被处理
////////////////////////////////////////////////////////////
bool FishingLayer::onTouchBegan(Touch* touch, Event* event)
{
	handleSliderAction(1);  // 正向移动
	return true;
}

////////////////////////////////////////////////////////////
// 触摸结束事件回调函数，当用户结束触摸屏幕时，使滑块向反方向移动。
// @param touch 触摸对象
// @param event 事件对象
////////////////////////////////////////////////////////////
void FishingLayer::onTouchEnded(Touch* touch, Event* event)
{
	handleSliderAction(-1);  // 反向移动
}
