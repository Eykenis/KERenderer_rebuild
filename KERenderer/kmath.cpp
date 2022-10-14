#include "kmath.h"
namespace kmath {
	float module(const vec3f& v) {
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	}
	vec3f normalize(const vec3f& v) {
		vec3f ret(v);
		float mod = module(v);
		ret.x /= mod, ret.y /= mod, ret.z /= mod;
		return ret;
	}
	vec3f prod(const vec3f& v1, const vec3f& v2) {
		return vec3f(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}
	float module(const vec4f& v) {
		return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
	}
	vec4f normalize(const vec4f& v) {
		vec4f ret(v);
		float mod = module(v);
		ret.x /= mod, ret.y /= mod, ret.z /= mod, ret.w /= mod;
		return ret;
	}

	mat4f scale(float sx, float sy, float sz) {
		mat4f ret = mat4f::identical();
		ret.m[0][0] = sx;
		ret.m[1][1] = sy;
		ret.m[2][2] = sz;
		return ret;
	}

	mat4f pan(float px, float py, float pz) {
		mat4f ret = mat4f::identical();
		ret.m[0][3] = px, ret.m[1][3] = py, ret.m[2][3] = pz;
		return ret;
	}

	mat4f rotate(float rx, float ry, float rz) {
		rx = (rx / 180.f) * 3.14;
		ry = (ry / 180.f) * 3.14;
		rz = (rz / 180.f) * 3.14;
		mat4f ret = mat4f::identical();
		mat4f tmp = mat4f::identical();
		tmp.m[1][1] = cos(rx), tmp.m[1][2] = -sin(rx), tmp.m[2][1] = sin(rx), tmp.m[2][2] = cos(rx);
		ret = tmp * ret;
		tmp = mat4f::identical();
		tmp.m[0][0] = cos(ry), tmp.m[0][2] = sin(ry), tmp.m[2][0] = -sin(ry), tmp.m[2][2] = cos(ry);
		ret = tmp * ret;
		tmp = mat4f::identical();
		tmp.m[0][0] = cos(rz), tmp.m[0][1] = -sin(rz), tmp.m[1][0] = sin(rz), tmp.m[1][1] = cos(rz);
		ret = tmp * ret;
		return ret;
	}

	mat4f model(vec3f _rotate, vec3f _scale, vec3f _pan) {
		return rotate(_rotate.x, _rotate.y, _rotate.z) * scale(_scale.x, _scale.y, _scale.z) * pan(_pan.x, _pan.y, _pan.z);
	}

	mat4f ortho(float l, float r, float b, float t, float n, float f) {
		mat4f scale = mat4f::identical();
		scale.m[0][0] = 2. / (r - l);
		scale.m[1][1] = 2. / (t - b);
		scale.m[2][2] = 2. / (n - f);
		scale.m[3][3] = 1.;
		mat4f pann = pan(-(r + l) / 2., -(t + b) / 2., -(n + f) / 2.);
		return scale * pann;
	}

	mat4f perspective(float fov, float aspect, float n, float f) {
		fov = (fov / 180.) * 3.14;
		mat4f ret = mat4f::zero();
		n = -n, f = -f;
		ret.m[0][0] = n;
		ret.m[1][1] = n;
		ret.m[2][2] = n + f;
		ret.m[2][3] = -n * f;
		ret.m[3][2] = 1.;

		float t = tan(fov / 2.) * abs(n);
		float b = -t;
		float r = aspect * t;
		float l = -r;

		return ortho(l, r, b, t, n, f) * ret;
	}

	mat4f lookat(vec3f cameraPos, vec3f cameraDir, vec3f cameraUp) {
		mat4f ret = mat4f::identical();
		cameraDir = normalize(cameraDir);
		cameraUp = normalize(cameraUp);
		vec3f cameraRight = cross(cameraDir, cameraUp);
		ret.m[0][0] = cameraRight.x, ret.m[0][1] = cameraRight.y, ret.m[0][2] = cameraRight.z;
		ret.m[1][0] = cameraUp.x, ret.m[1][1] = cameraUp.y, ret.m[1][2] = cameraUp.z;
		ret.m[2][0] = -cameraDir.x, ret.m[2][1] = -cameraDir.y, ret.m[2][2] = -cameraDir.z;
		ret = ret * pan(-cameraPos.x, -cameraPos.y, -cameraPos.z);
		return ret;
	}

	mat4f translate(float rotate, vec3f pan) {
		return mat4f::zero();
	}

	mat4f viewport(float xbegin, float xend, float ybegin, float yend) {
		mat4f ret = pan(-1., -1., 0.);
		mat4f scale = mat4f::identical();
		scale.m[0][0] *= (xbegin - xend) / 2.;
		scale.m[1][1] *= (ybegin - yend) / 2.;
		return scale * ret;
	}
};