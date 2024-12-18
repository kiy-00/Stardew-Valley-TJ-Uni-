#pragma once
#include "GameObject.h"
#include <memory>

class BoundaryAdder : public GameObject {
public:
  explicit BoundaryAdder(std::unique_ptr<GameObject> gameObject)
      : decoratedObject(std::move(gameObject)) {}

  virtual void render() override {
    if (decoratedObject) {
      decoratedObject->render();
      // 添加边界相关的额外渲染逻辑
    }
  }

private:
  std::unique_ptr<GameObject> decoratedObject;
};
