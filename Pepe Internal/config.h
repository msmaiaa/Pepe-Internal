#pragma once
namespace config {
	//
	extern bool noFlashActivated;
	extern bool isRadarActivated;
	extern bool isBhopActivated;
	extern bool isFovActivated;
	extern int fovAmount;
	//
	extern bool isGlowActivated;
	extern bool glow_allies;
	extern bool isRCSActivated;
	extern bool isRCSCrosshairActivated;
	//esp
	extern bool isESPActivated;
	extern bool esp_allies;
	//
	extern bool isTbotActivated;
	extern int tBotDelay;
	extern int tBotKey;
	//
	extern bool isAimbotActivated;
	extern int aimbotKey;
	extern int aimbotFov;
	extern bool drawAimbotFov;
	extern int aimbotSpeed;
	extern bool smoothAimbot;
	//
	extern bool showKDA;
	extern bool showPing;

	bool loadConfig();
	void saveConfig();
}