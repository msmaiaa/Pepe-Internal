#pragma once
namespace config {
	extern bool noFlashActivated;
	extern bool isRadarActivated;
	extern bool isBhopActivated;
	extern bool isGlowActivated;
	extern bool isTbotActivated;
	extern bool isRCSActivated;
	extern bool isRCSCrosshairActivated;
	extern bool isESPActivated;
	extern int tBotDelay;
	extern int tBotKey;

	bool loadConfig();
	void saveConfig();
}