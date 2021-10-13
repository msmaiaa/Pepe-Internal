#pragma once
namespace config {
	extern bool noFlashActivated;
	extern bool isRadarActivated;
	extern bool isBhopActivated;
	extern bool isGlowActivated;
	extern bool glow_allies;
	extern bool isTbotActivated;
	extern bool isRCSActivated;
	extern bool isRCSCrosshairActivated;
	//esp
	extern bool isESPActivated;
	extern bool esp_allies;
	//
	extern int tBotDelay;
	extern int tBotKey;

	bool loadConfig();
	void saveConfig();
}