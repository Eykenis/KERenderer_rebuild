#include "fftocean.h"
#include <random>
#include <iostream>

std::vector<std::vector<float> > getGaussianRand(int width, int height) {
	std::vector<std::vector<float> > ret;
	ret.resize(width);
	std::normal_distribution<float> distrx(0.0, 1.0);
	std::default_random_engine gen;
	for (int i = 0; i < width; ++i) {
		ret[i].resize(height);
		for (int j = 0; j < height; ++j) {
			float s = distrx(gen);
			ret[i][j] = s;
		}
		std::cout << std::endl;
	}
	return ret;
}

float getPhilipsSpectrum(const kmath::vec2f& k) {
	float klen = max(kmath::module(k), 0.0001f);
	float klen2 = klen * klen;
	float klen4 = klen2 * klen2;
	return A * exp(-1.0f / (klen2 * (L * L))) / klen4;
}

float donelanBanner(const kmath::vec2f& k) {
	float betaS = 0;
	float omegap = 0.855f * g / velocity;
	float ratio = kmath::module(k) / omegap;
	if (ratio < 0.95f)
		betaS = 2.61f * pow(ratio, 1.3f);
	else if (ratio >= 0.95f && ratio < 1.6f)
		betaS = 2.28f * pow(ratio, -1.3f);
	else if (ratio >= 1.6f) {
		float eps = -0.4f + 0.8393f * exp(-0.567f * log(ratio * ratio));
		betaS = pow(10, eps);
	}
	float theta = atan2(k.y, k.x) - atan2(wind.y, wind.x);
	return betaS / max(0.00001, 2.0f * tanh(betaS * pi) * pow(cosh(betaS * theta), 2));
}

kmath::vec2f getHeightSpectrum(kmath::vec2f coor, float t, std::vector<std::vector<float> >& gaussx, std::vector<std::vector<float> >& gaussy) {
	kmath::vec2f k(2 * pi * coor.x / OCEAN_SIZE - pi, 2 * pi * coor.y / OCEAN_SIZE - pi);
	kmath::vec2f gauss(gaussx[coor.x][coor.y], gaussy[coor.x][coor.y]);
	kmath::vec2f h0 =  gauss * sqrt(getPhilipsSpectrum(k) * donelanBanner(k) / 2);
	kmath::vec2f h0c = gauss * sqrt(getPhilipsSpectrum(-k) * donelanBanner(-k) / 2);
	h0c = h0c * -1;
	float klen = kmath::module(k);
	float omegat = sqrt(klen * g) * t;
	float c = cos(omegat);
	float s = sin(omegat);
	kmath::vec2f euler(c, s);
	kmath::vec2f eulerc(c, -s);
	return kmath::prod(h0, euler) + kmath::prod(h0c, eulerc);
}

void ifft(std::vector<kmath::vec2f>& vec, int size) {
	if (!size) return;
	std::vector<kmath::vec2f> a, b;
	a.resize(size); b.resize(size);
	for (int i = 0; i < size; ++i) {
		a[i] = vec[i << 1];
		b[i] = vec[i << 1 | 1];
	}
	ifft(a, size >> 1);
	ifft(b, size >> 1);
	kmath::vec2f wn(cos(pi / size), -sin(pi / size));
	kmath::vec2f w(1, 0);
	for (int i = 0; i < size; ++i, w = kmath::complexm(w, wn)) {
		vec[i] = a[i] + kmath::complexm(w, b[i]);
		vec[i + size] = a[i] - kmath::complexm(w, b[i]);
	}
}

void ifft2(std::vector<std::vector<kmath::vec2f> >& mat) {
	int size = mat.size();
	int f2 = 1; while (f2 < size) f2 <<= 1;
	if (f2 != size) {
		std::cout << "FFT Size Incorrect!" << std::endl;
	}

	// D 1
	for (int i = 0; i < mat.size(); ++i) {
		ifft(mat[i], size >> 1);
	}

	// D 2
	for (int i = 0; i < size; ++i) {
		std::vector<kmath::vec2f> tmp;
		for (int j = 0; j < size; ++j) {
			tmp.emplace_back(mat[j][i]);
		}
		ifft(tmp, size >> 1);
		for (int j = 0; j < size; ++j) {
			mat[j][i] = tmp[j];
		}
	}
}

Mesh doFFTOcean() {
	std::vector<std::vector<float> > gaussx = getGaussianRand(OCEAN_SIZE, OCEAN_SIZE);
	std::vector<std::vector<float> > gaussy = getGaussianRand(OCEAN_SIZE, OCEAN_SIZE);
	std::vector<std::vector<kmath::vec2f> > heightspectrogram;
	heightspectrogram.resize(OCEAN_SIZE);
	for (int i = 0; i < OCEAN_SIZE; ++i) {
		heightspectrogram[i].resize(OCEAN_SIZE);
		for (int j = 0; j < OCEAN_SIZE; ++j) {
			heightspectrogram[i][j] = getHeightSpectrum(kmath::vec2f(i, j), 0, gaussx, gaussy);
		}
	}

	// IDFT
	ifft2(heightspectrogram);

	// translate to model
	Mesh fftMesh;
	fftMesh.normal.push_back({ 0, 0, 1 });
	fftMesh.tex_coord.push_back({ 0, 0 });
	fftMesh.submesh.push_back(SubMesh());
	for (int i = 0; i < OCEAN_SIZE; ++i) {
		for (int j = 0; j < OCEAN_SIZE; ++j) {
			float y = kmath::module(heightspectrogram[i][j]) / 50;
			fftMesh.vert.push_back(kmath::vec3f(i, y, j));
		}
	}
	for (int i = 0; i + 1 < OCEAN_SIZE; ++i) {
		for (int j = 0; j + 1 < OCEAN_SIZE; ++j) {
			int idx = i * OCEAN_SIZE + j;
			fftMesh.submesh[0].face.push_back({ {idx, 0, 0}, {idx + 1, 0, 0}, {idx + OCEAN_SIZE, 0, 0} });
		}
	}
	for (int i = 1; i < OCEAN_SIZE; ++i) {
		for (int j = 0; j + 1 < OCEAN_SIZE; ++j) {
			int idx = i * OCEAN_SIZE + j;
			fftMesh.submesh[0].face.push_back({ {idx, 0, 0}, {idx + 1 - OCEAN_SIZE, 0, 0}, {idx + 1, 0, 0} });
		}
	}
	return fftMesh;
}