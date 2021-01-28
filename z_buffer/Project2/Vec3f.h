#pragma once

#include "utils.h"
#include <iostream>
using namespace std;
class Vec3f;
typedef Vec3f Color;
typedef Vec3f Point3d;
class Vec3f {
public:
	float x, y, z;
	Vec3f() :x(0.0), y(0.0), z(0.0) {};
	Vec3f(float a, float b, float c) :x(a), y(b), z(c) {};

	inline Vec3f& operator =(const Vec3f& v) {
		x = v.x, y = v.y, z = v.z;
		return *this;
	}
	inline Vec3f& operator +=(const Vec3f& v) {
		x = x + v.x, y = y + v.y, z = z + v.z;
		return *this;
	}
	inline Vec3f& operator +=(float num) {
		x = x + num, y = y + num, z = z + num;
		return *this;
	}

	inline Vec3f& operator -=(const Vec3f& v) {
		x = x - v.x, y = y - v.y, z = z - v.z;
		return *this;
	}
	inline Vec3f& operator -=(float num) {
		x = x - num, y = y - num, z = z - num;
		return *this;
	}
	inline Vec3f& operator /=(const Vec3f& v) {
		if (isEqual(v.x, 0.0) || isEqual(v.y, 0.0) || isEqual(v.z, 0.0)) {
			cout << "Vec3f 相除时 除数为零";
		}
		x = x / v.x, y = y / v.y, z = z / v.z;
		return *this;
	}
	inline Vec3f& operator /=(float num) {
		cout << "num 相除时 除数为零" << endl;
		x = x / num, y = y / num, z = z / num;
		return *this;
	}
	inline Vec3f& operator *=(const Vec3f& v) {
		x = x * v.x, y = y * v.y, z = z * v.z;
		return *this;
	}
	inline Vec3f& operator *=(float num) {
		x = x * num, y = y * num, z = z * num;
		return *this;
	}
	friend inline Vec3f operator + (const Vec3f& u, double num) { return Vec3f(u.x + num, u.y + num, u.z + num); }
	friend inline Vec3f operator + (float num, const Vec3f& u) { return Vec3f(num + u.x, num + u.y, num + u.z); }
	friend inline Vec3f operator + (const Vec3f& u, const Vec3f& v) { return Vec3f(u.x + v.x, u.y + v.y, u.z + v.z); }
	friend inline Vec3f operator - (const Vec3f& u, float num) { return Vec3f(u.x - num, u.y - num, u.z - num); }
	friend inline Vec3f operator - (float num, const Vec3f& u) { return Vec3f(num - u.x, num - u.y, num - u.z); }
	friend inline Vec3f operator - (const Vec3f& u, const Vec3f& v) { return Vec3f(u.x - v.x, u.y - v.y, u.z - v.z); }
	friend inline Vec3f operator * (const Vec3f& u, float num) { return Vec3f(u.x * num, u.y * num, u.z * num); }
	friend inline Vec3f operator * (float num, const Vec3f& u) { return Vec3f(num * u.x, num * u.y, num * u.z); }
	friend inline Vec3f operator * (const Vec3f& u, const Vec3f& v) { return Vec3f(u.x * v.x, u.y * v.y, u.z * v.z); }
	friend inline Vec3f operator / (const Vec3f& u, float num) { return Vec3f(u.x / num, u.y / num, u.z / num); }
	friend inline Vec3f operator / (float num, const Vec3f& u) { return Vec3f(num / u.x, num / u.y, num / u.z); }
	friend inline Vec3f operator / (const Vec3f& u, const Vec3f& v) { return Vec3f(u.x / v.x, u.y / v.y, u.z / v.z); }

	inline bool operator == (const Vec3f& u) { return isEqual(x, u.x) && isEqual(y, u.y) && isEqual(z, u.z); }
	inline bool operator != (const Vec3f& u) { return !(isEqual(x, u.x) && isEqual(y, u.y) && isEqual(z, u.z)); }

};

inline Vec3f cross(const Vec3f& u, const Vec3f& v)
{
	return Vec3f(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
}

inline float dot(const Vec3f& u, const Vec3f& v)
{
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline float length(const Vec3f& u)
{
	return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
}

inline Vec3f normalize(const Vec3f& u)
{
	return u * (1.0f / (float)sqrt(u.x * u.x + u.y * u.y + u.z * u.z));
}