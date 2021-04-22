#pragma once

#include <Engine/IScene.hpp>

class TowerScene : public IScene {
public:

	bool onCreate() override;
	void onDestroy() override;
	void onUpdate(float dt) override;
	void onRender() override;

private:

};
