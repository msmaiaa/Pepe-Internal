#pragma once
#include "CPlayerResource.h"
#include "ISurface.h"
#include "IVEngineClient.h"
#include "IClientEntityList.h"
#include "CInput.h"
#include "internal.h"

namespace interfaces {
	void setupInterfaces();
	extern IClientEntityList* ClientEntityList;
	extern IVEngineClient* EngineClient;
	extern CInput* Input;
	//C_PlayerResource* PlayerResource;
	extern ISurface* Surface;
}
