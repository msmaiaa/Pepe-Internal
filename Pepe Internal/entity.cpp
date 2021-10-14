#include "entity.h"
#include <iostream>

bool LocalPlayer::resetFlashDuration() {
	if (flashDuration != NULL) {
		if (this->flashDuration > 0) this->flashDuration = 0;
	}
	else {
		return false;
	}
	return true;
};

float LocalPlayer::getDistance(vec3 other) {
	vec3 myPos = origin;
	vec3 delta = other - myPos;
	return sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z);
}

