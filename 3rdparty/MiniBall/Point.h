#ifndef POINT_H
#define POINT_H

#include "Tuple.h"

namespace MiniBall
{
	class Vector;

	class Point : public Tuple<3>
	{
	public:
		Point();
		Point(const int i);
		Point(const Point &P);
		Point(const Vector &v);
		Point(float Px, float Py, float Pz);

		Point &operator=(const Point &P);

		float getX() const;
		void setX(float x);

		float getY() const;
		void setY(float y);

		float getZ() const;
		void setZ(float z);

		Point &operator+=(const Vector &v);
		Point &operator-=(const Vector &v);

		friend Point operator+(const Point &P, const Vector &v);
		friend Point operator-(const Point &P, const Vector &v);

		friend Vector operator-(const Point &P, const Point &Q);

		float d(const Point &P) const;   // Distance between two points
		float d2(const Point &P) const;   // Squared distance between two points

		static float d(const Point &P, const Point &Q);   // Distance between two points
		static float d2(const Point &P, const Point &Q);   // Squared distance between two points
	};
}

#include "Vector.h"

namespace MiniBall
{
	inline Point::Point()
	{
	}

	inline Point::Point(const int i)
	{
		const float s = (float)i;

		setX(s);
		setY(s);
		setZ(s);
	}

	inline Point::Point(const Point &P)
	{
		setX(P.getX());
		setY(P.getY());
		setZ(P.getZ());
	}

	inline Point::Point(const Vector &v)
	{
		setX(v.getX());
		setY(v.getY());
		setZ(v.getZ());
	}

	inline Point::Point(float P_x, float P_y, float P_z)
	{
		setX(P_x);
		setY(P_y);
		setZ(P_z);
	}

	inline Point &Point::operator=(const Point &P)
	{
		setX(P.getX());
		setY(P.getY());
		setZ(P.getZ());
		
		return *this;
	}

	inline float Point::getX() const
	{
		return element[0];
	}

	inline void Point::setX(float x_)
	{
		element[0] = x_;
	}

	inline float Point::getY() const
	{
		return element[1];
	}

	inline void Point::setY(float y_)
	{
		element[1] = y_;
	}

	inline float Point::getZ() const
	{
		return element[2];
	}

	inline void Point::setZ(float z_)
	{
		element[2] = z_;
	}
}

#endif   // POINT_H
