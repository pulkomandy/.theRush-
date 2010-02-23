#include "Vector.h"

#include <math.h>

namespace MiniBall
{
	Vector Vector::operator+() const
	{
		return *this;
	}

	Vector Vector::operator-() const
	{
		return Vector(-getX(), -getY(), -getZ());
	}

	Vector &Vector::operator+=(const Vector &v)
	{
		setX(getX() + v.getX());
		setY(getX() + v.getY());
		setZ(getZ() + v.getZ());

		return *this;
	}

	Vector &Vector::operator-=(const Vector &v)
	{
		setX(getX() - v.getX());
		setY(getY() - v.getY());
		setZ(getZ() - v.getZ());

		return *this;
	}

	Vector &Vector::operator*=(float s)
	{
		setX(getX() * s);
		setY(getY() * s);
		setZ(getZ() * s);

		return *this;
	}

	Vector &Vector::operator/=(float s)
	{
		float r = 1.0f / s;

		return *this *= r;
	}

	bool operator==(const Vector &U, const Vector &v)
	{
		if(U.getX() == v.getX() && U.getY() == v.getY() && U.getZ() == v.getZ())
			return true;
		else
			return false;
	}

	bool operator!=(const Vector &U, const Vector &v)
	{
		if(U.getX() != v.getX() || U.getY() != v.getY() || U.getZ() != v.getZ())
			return true;
		else
			return false;
	}

	Vector operator+(const Vector &u, const Vector &v)
	{
		return Vector(u.getX() + v.getX(), u.getY() + v.getY(), u.getZ() + v.getZ());
	}

	Vector operator-(const Vector &u, const Vector &v)
	{
		return Vector(u.getX() - v.getX(), u.getY() - v.getY(), u.getZ() - v.getZ());
	}

	float operator*(const Vector &u, const Vector &v)
	{
		return u.getX() * v.getX() + u.getY() * v.getY() + u.getZ() * v.getZ();
	}

	Vector operator*(float s, const Vector &v)
	{
		return Vector(s * v.getX(), s * v.getY(), s * v.getZ());
	}

	Vector operator*(const Vector &v, float s)
	{
		return Vector(v.getX() * s, v.getY() * s, v.getZ() * s);
	}

	Vector operator/(const Vector &v, float s)
	{
		float r = 1.0f / s;

		return Vector(v.getX() * r, v.getY() * r, v.getZ() * r);
	}

	float operator^(const Vector &u, const Vector &v)
	{
		return acosf(u / Vector::N(u) * v / Vector::N(v));
	}

	Vector operator%(const Vector &u, const Vector &v)
	{
		return Vector(u.getY() * v.getZ() - u.getZ() * v.getY(), u.getZ() * v.getX() - u.getX() * v.getZ(), u.getX() * v.getY() - u.getY() * v.getX());
	}

	float Vector::N(const Vector &v)
	{
		return sqrtf(v^2);
	}

	float Vector::N2(const Vector &v)
	{
		return v^2;
	}
}