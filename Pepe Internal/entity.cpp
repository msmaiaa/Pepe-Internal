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