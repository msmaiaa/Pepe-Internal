#include "config.h"
#include <iostream>
#include "simdjson/simdjson.h"
#include "simdjson/json.hpp"
#include <fstream>
#include <string>

using json = nlohmann::json;
using namespace simdjson;
namespace config {
	bool noFlashActivated;
	bool isRadarActivated;
	bool isBhopActivated;
	bool isGlowActivated;
	bool isTbotActivated;
	bool isRCSActivated;
	bool isRCSCrosshairActivated;
	bool isESPActivated;
	int tBotKey;
	int tBotDelay;

	bool loadConfig() {
		try {
			ondemand::parser parser;
			padded_string json = padded_string::load("c:\\pepe\\pepe_csgo.json");
			ondemand::document configs = parser.iterate(json);
			isGlowActivated = bool(configs["glowhack"]);
			isESPActivated = bool(configs["esp"]);
			isRadarActivated = bool(configs["radar"]);
			isRCSCrosshairActivated = bool(configs["rcs_crosshair"]);
			isBhopActivated = bool(configs["bhop"]);
			noFlashActivated = bool(configs["no_flash"]);
			isRCSActivated = bool(configs["rcs"]);
			isTbotActivated = bool(configs["triggerbot"]);
			tBotDelay = int64_t(configs["triggerbot_delay"]);
			tBotKey = int64_t(configs["triggerbot_key"]);
			return 0;
		}
		catch (simdjson_error e) {
			std::cout << e.error() << std::endl;
		}
	};
	void saveConfig() {
		json j = {
		  { "glowhack", isGlowActivated },
		  { "esp", isESPActivated },
		  { "radar", isRadarActivated },
		  { "rcs_crosshair", isRCSCrosshairActivated },
		  { "bhop", isBhopActivated },
		  { "no_flash", noFlashActivated },
		  { "rcs", isRCSActivated },
		  { "triggerbot", isTbotActivated },
		  { "triggerbot_delay", tBotDelay }
		};
		std::ofstream output("c:\\pepe\\pepe_csgo.json");
		output << std::setw(4) << j << std::endl;
	};
}