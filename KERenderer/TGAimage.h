#pragma once
#include "macro.h"
#include <algorithm>
#include <cstring>
#include <fstream>
using namespace std;

struct TGAcolor;
struct TGAHeader;

class TGAimage
{
private:
	unsigned char* data;
	int width;
	int height;
	int bytespp;
public:
	enum Format {
		GRAYSCALE = 1,
		RGB = 3,
		RGBA = 4
	};

	TGAimage(TGAimage& i) {
		width = i.width;
		height = i.height;
		bytespp = i.bytespp;
		unsigned long nbytes = width * height * bytespp;
		data = new unsigned char[nbytes];
		memcpy(data, i.data, nbytes);
	}
	TGAimage(int w = WINDOW_WIDTH, int h = WINDOW_HEIGHT, int bpp = 4) {
		width = w, height = h, bytespp = bpp;
		unsigned long nbytes = width * height * bytespp;
		data = new unsigned char[nbytes];
		memset(data, 0, nbytes);
	}
	~TGAimage() {
		if (data) delete[] data;
	}
	//void load_TGA(char*);
	//void load_PNG(char*);
	//  void release_TGAimage();
	bool setColor(int x, int y, TGAcolor c);
	bool load_rle_data(std::ifstream& in);
	bool unload_rle_data(std::ofstream& out);
	TGAcolor get(int x, int y);
	bool read_TGA(const char* filename);
	bool write_TGA(const char* filename, bool rle = 1);
	bool flip_vertically();
	int getWidth() { return width; }
	int getHeight() { return height; }
	int getBpp() { return bytespp;  }
};
#pragma pack(push, 1)

struct TGA_Header {
	char idLength;
	char colorMapType;
	char dataTypeCode;
	short colorMapOrigin;
	short colorMapLength;
	char colorMapDepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char bitsperpixel;
	char imageDescriptor;
};
#pragma pack(pop)

struct TGAcolor {
	union {
		struct {
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;

	TGAcolor(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255) : b(B), g(G), r(R), a(A), bytespp(4) {

	}

	TGAcolor(int v, int bpp) : val(v), bytespp(bpp) {

	}

	TGAcolor(const TGAcolor& c, int bpp) : val(c.val), bytespp(bpp) {

	}

	TGAcolor(const unsigned char* p, int bpp) {
		bytespp = bpp;
		r = p[0];
		g = p[1];
		if (bpp >= 3) b = p[2];
		if (bpp >= 4) a = p[3];
	}

	TGAcolor& operator = (const TGAcolor& c) {
		if (this != &c) {
			bytespp = c.bytespp;
			val = c.val;
		}
		return *this;
	}

	TGAcolor operator * (const float f) {
		return TGAcolor(min(255.f, r * f), min(255.f, g * f), min(255.f, b * f), a);
	}
	TGAcolor operator + (const TGAcolor& o) {
		return TGAcolor(min(255, r + o.r), min(255, g + o.g), min(255, b + o.b), a);
	}
};
