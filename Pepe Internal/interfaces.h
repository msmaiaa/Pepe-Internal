#pragma once
#include "CPlayerResource.h"
#include "ISurface.h"
#include "IGameEvents.h"
#include "IVEngineClient.h"
#include "IClientEntityList.h"
#include "CInput.h"
#include "internal.h"
#include "IBaseClientDLL.h"
#include "EventListener.h"
#include "IEngineTrace.h"

namespace interfaces {
	void setupInterfaces();
	extern IClientEntityList* ClientEntityList;
	extern IVEngineClient* EngineClient;
	extern CInput* Input;
	extern C_PlayerResource* PlayerResource;
	extern IGameEventManager2* GameEventManager;
	extern ISurface* Surface;
	extern IEngineTrace* EngineTrace;
	extern ClientClass* clientClass;
}
