#pragma once
#include <math.h>
struct vec3 {
	float x, y, z;
	vec3 operator*(float b) {
		return { x * b, y * b, z * b };
	}
	vec3 operator+(vec3 b) {
		return { x + b.x, y + b.y, z + b.z };
	}
	vec3 operator-(vec3 b) {
		return { x - b.x, y - b.y, z - b.z };
	}

	bool isMoving() {
		return ((x + y + z) != 0.0f);
	}

	void normalize() {
		while (y < -180) { y += 360; }
		while (y > 180) { y -= 360; }
		if (x > 89) { x = 89; }
		if (x < -89) { x = -89; }
	}
	float distance(const vec3& other) {
		return sqrtf((this->x - other.x) * (this->x - other.x) + (this->y - other.y) * (this->y - other.y) + (this->z - other.z) * (this->z - other.z));
	}
};


struct vec2 {
	float x, y;
	vec2 operator*(float b) {
		return { x * b, y * b };
	}
	vec2 operator+(vec2 b) {
		return { x + b.x, y + b.y };
	}
	vec2 operator-(vec2 b) {
		return { x - b.x, y - b.y };
	}

	void normalize() {
		while (y < -180) { y += 360; }
		while (y > 180) { y -= 360; }
		if (x > 89) { x = 89; }
		if (x < -89) { x = -89; }
	}
};

struct vec4 {
	float x, y, z, w;
};