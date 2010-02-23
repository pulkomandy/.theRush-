#include "Quaternion.h"

namespace MiniBall
{
	Quaternion::operator float() const
	{
		return getW();
	}

	Quaternion::operator Vector() const
	{
		return Vector(getX(), getY(), getZ());
	}

	Quaternion::operator Matrix() const
	{
		return Matrix(1 - 2 * (getY() * getY() + getZ() * getZ()), 2 * (getX() * getY() - getW() * getZ()),     2 * (getX() * getZ() + getW() * getY()),
		              2 * (getX() * getY() + getW() * getZ()),     1 - 2 * (getX() * getX() + getZ() * getZ()), 2 * (getY() * getZ() - getW() * getX()),
		              2 * (getX() * getZ() - getW() * getY()),     2 * (getY() * getZ() + getW() * getX()),     1 - 2 * (getX() * getX() + getY() * getY()));
	}

	Quaternion Quaternion::operator+() const
	{
		return *this;
	}

	Quaternion Quaternion::operator-() const
	{
		return Quaternion(-getX(), -getY(), -getZ(), -getW());
	}

	Quaternion Quaternion::operator!() const
	{
		return ~(*this) / N2(*this);
	}

	Quaternion Quaternion::operator~() const
	{
		return Quaternion(-getX(), -getY(), -getZ(), getW());
	}

	Quaternion &Quaternion::operator+=(const Quaternion &Q)
	{
		setX(getX() + Q.getX());
		setY(getY() + Q.getY());
		setZ(getZ() + Q.getZ());
		setW(getW() + Q.getW());

		return *this;
	}

	Quaternion &Quaternion::operator-=(const Quaternion &Q)
	{
		setX(getX() - Q.getX());
		setY(getY() - Q.getY());
		setZ(getZ() - Q.getZ());
		setW(getW() - Q.getW());

		return *this;
	}

	Quaternion &Quaternion::operator*=(float s)
	{
		setX(getX() * s);
		setY(getY() * s);
		setZ(getZ() * s);
		setW(getW() * s);

		return *this;
	}

	Quaternion &Quaternion::operator*=(const Quaternion &Q)
	{
		float Qx = getW() * Q.getX() + getX() * Q.getW() + getY() * Q.getZ() - getZ() * Q.getY();
		float Qy = getW() * Q.getY() - getX() * Q.getZ() + getY() * Q.getW() + getZ() * Q.getX();
		float Qz = getW() * Q.getZ() + getX() * Q.getY() - getY() * Q.getX() + getZ() * Q.getW();
		float Qw = getW() * Q.getW() - getX() * Q.getX() - getY() * Q.getY() - getZ() * Q.getZ();

		setW(Qw);
		setX(Qx);
		setY(Qy);
		setZ(Qz);

		return (*this);
	}

	Quaternion &Quaternion::operator/=(float s)
	{
		float r = 1.0f / s;

		return *this *= r;
	}

	bool operator==(const Quaternion &Q, const Quaternion &R)
	{
		if(Q.getX() == R.getX() && Q.getY() == R.getY() && Q.getZ() == R.getZ() && Q.getW() == R.getW())
			return true;
		else
			return false;
	}

	bool operator!=(const Quaternion &Q, const Quaternion &R)
	{
		if(Q.getX() != R.getX() || Q.getY() != R.getY() || Q.getZ() != R.getZ() || Q.getW() != R.getW())
			return true;
		else
			return false;
	}

	Quaternion operator+(const Quaternion &Q, const Quaternion &R)
	{
		return Quaternion(Q.getX() + R.getX(), Q.getY() + R.getY(), Q.getZ() + R.getZ(), Q.getW() + R.getW());
	}

	Quaternion operator-(const Quaternion &Q, const Quaternion &R)
	{
		return Quaternion(Q.getX() - R.getX(), Q.getY() - R.getY(), Q.getZ() - R.getZ(), Q.getW() - R.getW());
	}

	Quaternion operator*(const Quaternion &Q, const Quaternion &R)
	{
		float _x = +Q.getX() * R.getW() + Q.getY() * R.getZ() - Q.getZ() * R.getY() + Q.getW() * R.getX();
		float _y = -Q.getX() * R.getZ() + Q.getY() * R.getW() + Q.getZ() * R.getX() + Q.getW() * R.getY();
		float _z = +Q.getX() * R.getY() - Q.getY() * R.getX() + Q.getZ() * R.getW() + Q.getW() * R.getZ();
		float _w = -Q.getX() * R.getX() - Q.getY() * R.getY() - Q.getZ() * R.getZ() + Q.getW() * R.getW();

		return Quaternion(_x, _y, _z, _w);
	}

	Quaternion operator*(float s, const Quaternion &Q)
	{
		return Quaternion(s * Q.getX(), s * Q.getY(), s * Q.getZ(), s * Q.getW());
	}

	Quaternion operator*(const Quaternion &Q, float s)
	{
		return Quaternion(Q.getX() * s, Q.getY() * s, Q.getZ() * s, Q.getW() * s);
	}

	Quaternion operator/(const Quaternion &Q, float s)
	{
		float r = 1.0f / s;

		return Q * r;
	}

	float N(const Quaternion &Q)
	{
		return sqrtf(Q.getX()*Q.getX() + Q.getY()*Q.getY() + Q.getZ()*Q.getZ() + Q.getW()*Q.getW());
	}

	float N2(const Quaternion &Q)
	{
		return Q.getX()*Q.getX() + Q.getY()*Q.getY() + Q.getZ()*Q.getZ() + Q.getW()*Q.getW();
	}

	Quaternion slerp(const Quaternion &Q, const Quaternion &R, float t)
	{
		return Quaternion(Q, R, t);
	}
}