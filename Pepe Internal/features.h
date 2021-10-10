#pragma once
#include "vector.h"

struct Colors {
	float red[3] = { 1.0f, 0.0f, 0.0f };
	float blue[3] = { 0.0f, 0.0f, 1.0f };
	float green[3] = { 0.0f, 0.0f, 1.0f };
};

void doRadar();
void doBhop();
void doGlow();
void doTbot();