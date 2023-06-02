#pragma once
#include "kmath.h"
#include "Mesh.h"
#include <vector>
#define OCEAN_SIZE 128
const float pi = 3.1416f;
const float A = 1.0f;
const kmath::vec2f wind(10.0f, 10.0f);
const float velocity = kmath::module(wind);
const float g = 9.8f;
const float L = (velocity * velocity) / g;

std::vector<std::vector<float> > getGaussianRand(int width, int height);

float getPhilipsSpectrum(const kmath::vec2f& k);
float donelanBanner(const kmath::vec2f& k);
kmath::vec2f getHeightSpectrum(kmath::vec2f coor, float t, std::vector<std::vector<float> >& gaussx, std::vector<std::vector<float> >& gaussy);
void ifft(std::vector<kmath::vec2f>& vec, int size);
void ifft2(std::vector<std::vector<kmath::vec2f> >& mat);
Mesh doFFTOcean();