#include "EventListener.h"
#include "interfaces.h"
#include "features.h"
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>

#pragma comment(lib, "Winmm.lib")

extern LocalPlayer* localPlayer;
EventListener::EventListener() noexcept
{
    interfaces::GameEventManager->AddListener(this, "player_hurt");
}

void EventListener::Remove() noexcept
{
    interfaces::GameEventManager->RemoveListener(this);
}

void EventListener::FireGameEvent(IGameEvent* event)
{
    if (strncmp(event->GetName(), "player_hurt", sizeof("player_hurt")) == 0)
    {
        int entityId = interfaces::EngineClient->GetPlayerForUserID(event->GetInt("attacker"));
        if (entityId == *features::localPlayerIndex) {
            PlaySound(TEXT("C:\\pepe\\hitsound1.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }
    }
}

EventListener* g_eventListener = nullptr;