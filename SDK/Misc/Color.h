#pragma once
#include <cmath>
#include <Windows.h>

class Color {
public:
	unsigned char r, g, b, a;

	Color() {
		r = g = b = a = 255;
	}

	explicit Color(int _r) {
		r = g = b = _r;
		a = 255;
	}

	Color(int _r, int _g) {
		r = g = b = _r;
		a = _g;
	}

	Color(int _r, int _g, int _b) {
		r = (unsigned char)_r;
		g = (unsigned char)_g;
		b = (unsigned char)_b;
		a = (unsigned char)255;
	}

	Color(int _r, int _g, int _b, int _a) {
		r = (unsigned char)_r;
		g = (unsigned char)_g;
		b = (unsigned char)_b;
		a = (unsigned char)_a;
	}

	//Color(float _r, float _g, float _b) {
	//	r = static_cast<unsigned char>(_r);
	//	g = static_cast<unsigned char>(_g);
	//	b = static_cast<unsigned char>(_b);
	//	a = (unsigned char)255;
	//}

	//Color(float _r, float _g, float _b, float _a) {
	//	r = static_cast<unsigned char>(_r);
	//	g = static_cast<unsigned char>(_g);
	//	b = static_cast<unsigned char>(_b);
	//	a = static_cast<unsigned char>(_a);
	//}

	void FromHSV(int _h, int _s = 100, int _v = 100, int A = 255) {
		float H = (float)_h;
		float S = (float)_s;
		float V = (float)_v;

		float s = S / 100.f;
		float v = V / 100.f;
		float C = s * v;
		float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
		float m = v - C;
		float _r, _g, _b;
		if (H >= 0 && H < 60) {
			_r = C, _g = X, _b = 0;
		}
		else if (H >= 60 && H < 120) {
			_r = X, _g = C, _b = 0;
		}
		else if (H >= 120 && H < 180) {
			_r = 0, _g = C, _b = X;
		}
		else if (H >= 180 && H < 240) {
			_r = 0, _g = X, _b = C;
		}
		else if (H >= 240 && H < 300) {
			_r = X, _g = 0, _b = C;
		}
		else {
			_r = C, _g = 0, _b = X;
		}
		int R = (_r + m) * 255;
		int G = (_g + m) * 255;
		int B = (_b + m) * 255;

		r = (unsigned char)R;
		g = (unsigned char)G;
		b = (unsigned char)B;
		a = (unsigned char)A;
	}

	void ToHSV(int& h, int& s, int& v) {
		float fR = r / 255.f, fG = g / 255.f, fB = b / 255.f;
		int fH;
		float fS, fV;

		float fCMax = max(max(fR, fG), fB);
		float fCMin = min(min(fR, fG), fB);
		float fDelta = fCMax - fCMin;

		if (fDelta > 0) {
			if (fCMax == fR) {
				fH = 60 * (fmod(((fG - fB) / fDelta), 6));
			}
			else if (fCMax == fG) {
				fH = 60 * (((fB - fR) / fDelta) + 2);
			}
			else if (fCMax == fB) {
				fH = 60 * (((fR - fG) / fDelta) + 4);
			}

			if (fCMax > 0) {
				fS = fDelta / fCMax;
			}
			else {
				fS = 0;
			}

			fV = fCMax;
		}
		else {
			fH = 0;
			fS = 0;
			fV = fCMax;
		}

		if (fH < 0) {
			fH = 360 + fH;
		}

		h = fH;
		s = fS * 100;
		v = fV * 100;
	}

	bool operator==(const Color &o) {
		return *(int*)this == *(int*)&o;
	}

	bool operator!=(const Color& o) {
		return *(int*)this != *(int*)&o;
	}

	Color operator*(float other) const {
		return Color((int)(r * other), (int)(g * other), (int)(b * other), (int)a);
	}

	Color operator*(const Color& other) const {
		return Color().as_fraction(r * other.r / 65025.f, g * other.g / 65025.f, b * other.b / 65025.f, a * other.a / 65025.f);
	}

	Color operator+(Color other) const {
		return Color(min(max(r + other.r, 0), 255), min(max(g + other.g, 0), 255), min(max(b + other.b, 0), 255), min(max(a + other.a, 0), 255));
	}

	Color operator-(Color other) const {
		return Color(min(max(r - other.r, 0), 255), min(max(g - other.g, 0), 255), min(max(b - other.b, 0), 255), min(max(a - other.a, 0), 255));
	}

	void as_int32(int col32) {
		*(int*)this = col32;
	}

	Color& as_fraction(float r_, float g_, float b_, float a_ = 255.f) {
		r = int(r_ * 255.f + 0.5f);
		g = int(g_ * 255.f + 0.5f);
		b = int(b_ * 255.f + 0.5f);
		a = int(a_ * 255.f + 0.5f);
		return *this;
	}

	float* to_fraction() {
		float result[4] = { r / 255.f, g / 255.f, b / 255.f, a / 255.f };
		return result;
	}

	int to_int32() const {
		return *(int*)this;
	}

	unsigned int d3d_color() {
		return ((DWORD)((((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff)));
	}

	Color alpha_modulate(int alpha) const {
		return Color(r, g, b, (unsigned char)alpha);
	}

	Color& alpha_modulatef(float alpha) {
		a *= alpha;
		return *this;
	}

	Color lerp(const Color& other, float perc) {
		return Color(int(r + (other.r - r) * perc), int(g + (other.g - g) * perc), int(b + (other.b - b) * perc), int(a + (other.a - a) * perc));
	}

	Color clone() {
		return Color(r, g, b, a);
	}
};