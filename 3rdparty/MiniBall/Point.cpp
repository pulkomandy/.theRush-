#include "Point.h"

namespace MiniBall
{
	Point &Point::operator+=(const Vector &v)
	{
		setX(getX() + v.getX());
		setY(getY() + v.getY());
		setZ(getZ() + v.getZ());

		return *this;
	}

	Point &Point::operator-=(const Vector &v)
	{
		setX(getX() - v.getX());
		setY(getY() - v.getY());
		setZ(getZ() - v.getZ());

		return *this;
	}

	Point operator+(const Point &P, const Vector &v)
	{
		return Point(P.getX() + v.getX(), P.getY() + v.getY(), P.getZ() + v.getZ());
	}

	Point operator-(const Point &P, const Vector &v)
	{
		return Point(P.getX() - v.getX(), P.getY() - v.getY(), P.getZ() - v.getZ());
	}

	Vector operator-(const Point &P, const Point &Q)
	{
		return Vector(P.getX() - Q.getX(), P.getY() - Q.getY(), P.getZ() - Q.getZ());
	}

	float Point::d(const Point &P) const
	{
		return Vector::N(*this - P);
	}

	float Point::d2(const Point &P) const
	{
		return Vector::N2(*this - P);
	}

	float Point::d(const Point &P, const Point &Q)
	{
		return Vector::N(P - Q);
	}

	float Point::d2(const Point &P, const Point &Q)
	{
		return Vector::N2(P - Q);
	}
}