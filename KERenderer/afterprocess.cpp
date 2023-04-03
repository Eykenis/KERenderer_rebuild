#include "afterprocess.h"

void avr2x2(unsigned char* buffer, int x, int y) {
	int idx = (y * WINDOW_WIDTH + x) * 4;
	kmath::vec3<int> v(0, 0, 0);
	for (int i = 0; i < 4; ++i) {
		for (int k = 0; k < 3; ++k) {
			v.v[k] += buffer[idx + k];
		}
		if (i & 1) idx += 4 * WINDOW_WIDTH;
		else idx += 4;
	}
	v = v / 4;
	idx = (y * WINDOW_WIDTH + x) * 4;
	for (int i = 0; i < 4; ++i) {
		for (int k = 0; k < 3; ++k) {
			buffer[idx + k] = v.v[k];
		}
		if (i & 1) idx += 4 * WINDOW_WIDTH;
		else idx += 4;
	}
}

void filter(unsigned char* buffer) {
	for (int i = 0; i + 1 < WINDOW_WIDTH; ++i) {
		for (int j = 0; j + 1 < WINDOW_HEIGHT; ++j) {
			avr2x2(buffer, i, j);
		}
	}
}