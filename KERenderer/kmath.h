#pragma once
#include <cstring>
#include <cmath>
#define vec2i vec2<int>
#define vec2f vec2<float>
#define vec3i vec3<int>
#define vec3f vec3<float>
#define vec4i vec4<int>
#define vec4f vec4<float>
#define mat3i mat3<int>
#define mat3f mat3<float>
#define mat4i mat4<int>
#define mat4f mat4<float>

namespace kmath {
	template<typename T> struct vec2 {
		union {
			struct {
				T x, y;
			};
			struct {
				T tex_u, tex_v;
			};
			T v[2];
		};

		vec2<T>() {
			x = y = 0;
		}
		~vec2<T>() { }
		vec2<T>(T _x, T _y) {
			x = _x;
			y = _y;
		}
		vec2<T>(const vec2<T>& v) {
			x = v.x;
			y = v.y;
		}

		T operator * (const vec2<T>& v) {
			return this->x * v.x + this->y * v.y;
		}

		vec2<T> operator + (const vec2<T>& v) {
			return vec2(this->x + v.x, this->y + v.y);
		}

		vec2<T> operator - (const vec2<T>& v) {
			return vec2(this->x - v.x, this->y - v.y);
		}

		vec2<T> operator * (const T t) {
			return vec2(this->x * t, this->y * t);
		}

		vec2<T>& operator = (const vec2<T>& v) {
			this->x = v.x; this->y = v.y;
			return *this;
		}
	};

	template<typename T> struct vec3 {
		union {
			struct {
				T x, y, z;
			};
			struct {
				T r, g, b;
			};
			T v[3];
		};

		vec3<T>() {
			x = y = z = 0;
		}
		~vec3<T>() { }
		vec3<T>(T _x, T _y, T _z) {
			x = _x;
			y = _y;
			z = _z;
		}
		vec3<T>(const vec3<T>& v) {
			x = v.x;
			y = v.y;
			z = v.z;
		}

		T operator * (const vec3<T>& v) {
			return this->x * v.x + this->y * v.y + this->z * v.z;
		}

		vec3<T> operator + (const vec3<T>& v) {
			return vec3<T>(this->x + v.x, this->y + v.y, this->z + v.z);
		}

		vec3<T> operator - (const vec3<T>& v) {
			return vec3<T>(this->x - v.x, this->y - v.y, this->z - v.z);
		}

		vec3<T> operator * (const T t) {
			return vec3<T>(this->x * t, this->y * t, this->z * t);
		}

		vec3<T>& operator = (const vec3<T>& v) {
			this->x = v.x; this->y = v.y; this->z = v.z;
			return *this;
		}
	};

	template<typename T> struct vec4 {
		union {
			struct {
				T x, y, z, w;
			};
			struct {
				T r, g, b, a;
			};
			T v[4];
			vec3<T> xyz;
		};

		vec4<T>() {
			x = y = z = w = 0;
		}
		~vec4<T>() { }
		vec4<T>(T _x, T _y, T _z, T _w) {
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
		vec4<T>(const vec4<T>& v) {
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;
		}
		vec4<T>(const vec3<T>& v, T t) {
			x = v.x;
			y = v.y;
			z = v.z;
			w = t;
		}

		T operator * (const vec4<T>& v) {
			return this->x * v.x + this->y * v.y + this->z * v.z + this->w * v.w;
		}

		vec4<T> operator + (const vec4<T>& v) {
			return vec4<T>(this->x + v.x, this->y + v.y, this->z + v.z, this->w + v.w);
		}

		vec4<T> operator - (const vec4<T>& v) {
			return vec4<T>(this->x - v.x, this->y - v.y, this->z - v.z, this->w - v.w);
		}

		vec4<T> operator * (const T t) {
			return vec3<T>(this->x * t, this->y * t, this->z * t, this->w * t);
		}

		vec4<T>& operator = (const vec4<T>& v) {
			this->x = v.x; this->y = v.y; this->z = v.z, this->w = v.w;
			return *this;
		}
	};

	template<typename T> struct mat3 {
		T m[3][3];

		mat3<T>() {
			memset(m, 0, sizeof(m));
		}
		~mat3<T>() { }
		mat3<T>(const mat3<T>& mat) {
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					m[i][j] = mat.m[i][j];
				}
			}
		}
		mat3<T>(int _m[9]) {
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					m[i][j] = _m[i * 3 + j];
				}
			}
		}

		static mat3f identical() {
			static mat3f ident;
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					if (i == j) ident.m[i][j] = 1;
					else ident.m[i][j] = 0;
				}
			}
			return ident;
		}

		static mat3f zero() {
			static mat3f z;
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) z.m[i][j] = 0;
			}
			return z;
		}

		mat3<T> operator * (const mat3<T>& mat) {
			mat3<T> ret;
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					for (int k = 0; k < 3; ++k) {
						ret.m[i][j] += m[i][k] * mat.m[k][j];
					}
				}
			}
			return ret;
		}

		vec3<T> operator * (const vec3<T>& vec) {
			vec3<T> ret;
			for (int i = 0; i < 3; ++i) {
				for (int k = 0; k < 3; ++k) {
					ret.v[i] += m[i][k] * vec.v[k];
				}
			}
			return ret;
		}

		mat3<T>& operator = (const mat3<T>& mat) {
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					m[i][j] = mat.m[i][j];
				}
			}
			return *this;
		}

		T det() {
			return m[0][0] * m[1][1] * m[2][2] + m[0][2] * m[1][0] * m[2][1] + m[0][1] * m[1][2] * m[2][0] -
				(m[0][2] * m[1][1] * m[2][0] + m[0][1] * m[1][0] * m[2][2] + m[0][0] * m[1][2] * m[2][1]);
		}
	};

	template<typename T> struct mat4 {
		T m[4][4];

		mat4<T>() {
			memset(m, 0, sizeof(m));
		}
		~mat4<T>(){ }
		mat4<T>(const mat4<T>& mat) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					m[i][j] = mat.m[i][j];
				}
			}
		}
		mat4<T>(int _m[16]) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					m[i][j] = _m[i * 4 + j];
				}
			}
		}
		
		static mat4f identical() {
			static mat4f ident;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					if (i == j) ident.m[i][j] = 1;
					else ident.m[i][j] = 0;
				}
			}
			return ident;
		}
		
		static mat4f zero() {
			static mat4f z;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) z.m[i][j] = 0;
			}
			return z;
		}
		
		mat4<T> operator * (const mat4<T>& mat) {
			mat4<T> ret = mat4f::zero();
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					for (int k = 0; k < 4; ++k) {
						ret.m[i][j] += m[i][k] * mat.m[k][j];
					}
				}
			}
			return ret;
		}

		vec4<T> operator * (const vec4<T>& vec) {
			vec4<T> ret(0., 0., 0., 0.);
			for (int i = 0; i < 4; ++i) {
				for (int k = 0; k < 4; ++k) {
					ret.v[i] += m[i][k] * vec.v[k];
				}
			}
			return ret;
		}

		mat4<T>& operator = (const mat4<T>& mat) {
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					m[i][j] = mat.m[i][j];
				}
			}
			return *this;
		}

		mat3<T> cofactor(int row, int col) {
			mat3<T> ret = mat3f::zero();
			for (int i = 2; i >= 0; --i) {
				for (int j = 2; j >= 0; --j) {
					ret.m[i][j] = m[i < row ? i : i + 1][j < col ? j : j + 1];
				}
			}
			return ret;
		}

		T det() {

		}

		mat4<T> adjoint() {
			mat4<T> inv = mat4f::zero();
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					inv.m[i][j] = cofactor(i, j).det() * ((i + j) % 2 ? -1 : 1);
				}
			}
			return inv;
		}

		mat4<T> transpose() {
			mat4<T> ret;
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					ret.m[i][j] = m[j][i];
				}
			}
			return ret;
		}

		mat4<T> inverse() {
			mat4<T> ret = adjoint();
			T tmp = ret.m[0][0] * m[0][0] + ret.m[0][1] * m[0][1] + ret.m[0][2] * m[0][2] + ret.m[0][3] * m[0][3];
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					ret.m[i][j] /= tmp;
				}
			}
			return ret.transpose();
		}
	};

	template<typename T> T cross(const vec2<T>& u, const vec2<T>& v) {
		return u.x * v.y - u.y * v.x;
	}
	template<typename T> vec3<T> cross(const vec3<T>& u, const vec3<T>& v) {
		return vec3<T>(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
	}
	float module(const vec3f&);
	vec3f normalize(const vec3f&);
	float module(const vec4f&);
	vec4f normalize(const vec4f&);
	mat4f pan(float px, float py, float pz);
	mat4f rotate(float rx, float fy, float rz);
	mat4f model(vec3f _rotate, vec3f _scale, vec3f _pan);
	mat4f ortho(float l, float r, float b, float t, float n, float f);
	mat4f perspective(float fov, float aspect, float n, float f);
	mat4f lookat(vec3f cameraPos, vec3f cameraDir, vec3f cameraUp);
	mat4f viewport(float xbegin, float xend, float ybegin, float yend);
};