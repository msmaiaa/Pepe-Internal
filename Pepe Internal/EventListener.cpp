#include "EventListener.h"
#include "interfaces.h"
#include <iostream>

EventListener::EventListener() noexcept
{
    interfaces::GameEventManager->AddListener(this, "player_hurt");
}

void EventListener::Remove() noexcept
{
    interfaces::GameEventManager->RemoveListener(this);
}

/*void EventListener::start() noexcept
{
    interfaces::GameEventManager->AddListener(this, "player_hurt");
    //interfaces::GameEventManager->RemoveListener(this);
}*/

void EventListener::FireGameEvent(IGameEvent* event)
{
    if (strncmp(event->GetName(), "player_hurt", sizeof("player_hurt")) == 0)
    {
        // Find out how much damage was done
        int damage = event->GetInt("dmg_health");
        std::cout << "Player hurt for: " << damage << std::endl;
    }
}

EventListener* g_eventListener = nullptr;