#pragma once
struct vec3 {
	float x, y, z;
	vec3 operator*(float b) {
		return { x * b, y * b, z * b };
	}
	vec3 operator+(vec3 b) {
		return { x + b.x, y + b.y, z + b.y };
	}
	vec3 operator-(vec3 b) {
		return { x - b.x, y - b.y, z - b.y };
	}

	bool isMoving() {
		return ((x + y + z) != 0.0f);
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
};