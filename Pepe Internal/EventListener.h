#pragma once
#include "IGameEvents.h"

class EventListener : public IGameEventListener2 {
public:
	EventListener() noexcept;
	void Remove() noexcept;
	void start() noexcept;
	void FireGameEvent(IGameEvent* event);
	int GetEventDebugID() override
	{
		return 42;
	}
};
extern EventListener* g_eventListener;
