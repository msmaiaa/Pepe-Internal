#include "config.h"
#include <iostream>
#include "simdjson/simdjson.h"
#include "simdjson/json.hpp"
#include <fstream>
#include <string>

using json = nlohmann::json;
using namespace simdjson;
namespace config {
	bool isESPActivated = false;
	bool esp_allies = false;
	bool isGlowActivated = false;
	bool isRCSActivated = false;
	bool glow_allies = false;
	bool isRCSCrosshairActivated = false;
	//
	bool isFovActivated = false;
	int fovAmount = 90;
	bool noFlashActivated = false;
	bool isRadarActivated = false;
	bool isBhopActivated = false;
	//
	bool isTbotActivated = false;
	int tBotKey = 67;
	int tBotDelay = 0;
	//
	bool isAimbotActivated = false;
	int aimbotKey = 1;
	int aimbotFov = 20;
	int aimbotSpeed = 80;
	bool drawAimbotFov = false;
	bool smoothAimbot = false;

	bool loadConfig() {
		try {
			ondemand::parser parser;
			padded_string json = padded_string::load("c:\\pepe\\pepe_csgo.json");
			ondemand::document configs = parser.iterate(json);
			isGlowActivated = bool(configs["glow"]);
			glow_allies = bool(configs["glow_allies"]);
			isESPActivated = bool(configs["esp"]);
			esp_allies = bool(configs["esp_allies"]);
			isRadarActivated = bool(configs["radar"]);
			isRCSCrosshairActivated = bool(configs["rcs_crosshair"]);
			//
			isBhopActivated = bool(configs["bhop"]);
			noFlashActivated = bool(configs["no_flash"]);
			isRCSActivated = bool(configs["rcs"]);
			isFovActivated = bool(configs["fov_activated"]);
			fovAmount = int64_t(configs["fov_amount"]);
			//
			isTbotActivated = bool(configs["triggerbot"]);
			tBotDelay = int64_t(configs["triggerbot_delay"]);
			tBotKey = int64_t(configs["triggerbot_key"]);
			//
			isAimbotActivated = bool(configs["aimbot"]);
			aimbotFov = int64_t(configs["aimbot_fov"]);
			aimbotKey = int64_t(configs["aimbot_key"]);
			aimbotSpeed = int64_t(configs["aimbot_speed"]);
			drawAimbotFov = bool(configs["draw_aimbot_fov"]);
			smoothAimbot = bool(configs["smooth_aimbot"]);
			//
			return 0;
		}
		catch (simdjson_error e) {
			if (e.error() == IO_ERROR) {
				saveConfig();
				loadConfig();
			}
		}
	};
	void saveConfig() {
		json j = {
		  { "glow", isGlowActivated },
		  { "glow_allies", glow_allies },
		  { "esp", isESPActivated },
		  { "esp_allies", esp_allies},
		  { "radar", isRadarActivated },
		  { "rcs_crosshair", isRCSCrosshairActivated },
		  { "bhop", isBhopActivated },
		  { "no_flash", noFlashActivated },
		  { "rcs", isRCSActivated },
		  { "triggerbot", isTbotActivated },
		  { "triggerbot_delay", tBotDelay },
		  { "triggerbot_key", tBotKey },
		  { "aimbot", isAimbotActivated },
		  { "aimbot_fov", aimbotFov },
		  { "aimbot_key", aimbotKey },
		  { "aimbot_speed", aimbotSpeed },
		  { "draw_aimbot_fov", drawAimbotFov},
		  { "smooth_aimbot", smoothAimbot},
		};
		std::ofstream output("c:\\pepe\\pepe_csgo.json");
		output << std::setw(4) << j << std::endl;
	};
}