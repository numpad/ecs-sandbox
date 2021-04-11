#pragma once

#include <World.hpp>

class Engine;

class IScene {
public:

	virtual ~IScene() {}

	/**
	 * Called after the engine specific features have been initialized, before
	 * application code is allowed to run. The return value of this function
	 * decides if the engine should continue to execute or stop and clean up.
	 * (F.ex. if required assets couldn't be found)
	 * 
	 * @returns Wether the application initialized successfully.
	 */
	virtual bool onCreate() { return true; }
	/**
	 * Called before the engine cleanup process will begin.
	 */
	virtual void onDestroy() {}

	virtual void onUpdate(float dt) = 0;
	virtual void onRender() = 0;

	Engine *m_engine = nullptr;
private:

};
