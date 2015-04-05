#ifndef VECTOR_H
#define VECTOR_H

#include "Tuple.h"
#include "../../sources/libbase/ZCollisionsUtils.h"

namespace MiniBall
{
	class Point;

	class Vector : public Tuple<3>
	{
	public:
		Vector();
		
		Vector(const int i);
		Vector(const Vector &v);
		Vector(const Point &p);
		Vector(float v_x, float v_y, float v_z);
		

		Vector &operator=(const Vector &v);

		float getX() const;
		void setX(float x);

		float getY() const;
		void setY(float y);

		float getZ() const;
		void setZ(float z);

		Vector operator+() const;
		Vector operator-() const;

		Vector &operator+=(const Vector &v);
		Vector &operator-=(const Vector &v);
		Vector &operator*=(float s);
		Vector &operator/=(float s);

		friend bool operator==(const Vector &u, const Vector &v);
		friend bool operator!=(const Vector &u, const Vector &v);

		friend Vector operator+(const Vector &u, const Vector &v);
		friend Vector operator-(const Vector &u, const Vector &v);
		friend float operator*(const Vector &u, const Vector &v);   // Dot product
		friend Vector operator*(float s, const Vector &v);
		friend Vector operator*(const Vector &v, float s);
		friend Vector operator/(const Vector &v, float s);
		friend float operator^(const Vector &v, const int n);   // Vector power
		friend float operator^(const Vector &u, const Vector &v);   // Angle between vectors
		friend Vector operator%(const Vector &u, const Vector &v);   // Cross product

		static float N(const Vector &v);   // Norm
		static float N2(const Vector &v);   // Squared norm
	};
}

#include "Point.h"

#include <assert.h>

namespace MiniBall
{
	inline Vector::Vector()
	{
	}

	inline Vector::Vector(const int i)
	{
		const float s = (float)i;

		setX(s);
		setY(s);
		setZ(s);
	}

	inline Vector::Vector(const Vector &v)
	{
		setX(v.getX());
		setY(v.getY());
		setZ(v.getZ());
	}

	inline Vector::Vector(const Point &P)
	{
		setX(P.getX());
		setY(P.getY());
		setZ(P.getZ());
	}

	inline Vector::Vector(float v_x, float v_y, float v_z)
	{
		setX(v_x);
		setY(v_y);
		setZ(v_z);
	}

	inline Vector &Vector::operator=(const Vector &v)
	{
		setX(v.getX());
		setY(v.getY());
		setZ(v.getZ());
		
		return *this;
	}

	inline float Vector::getX() const
	{
		return element[0];
	}

	inline void Vector::setX(float x_)
	{
		element[0] = x_;
	}

	inline float Vector::getY() const
	{
		return element[1];
	}

	inline void Vector::setY(float y_)
	{
		element[1] = y_;
	}

	inline float Vector::getZ() const
	{
		return element[2];
	}

	inline void Vector::setZ(float z_)
	{
		element[2] = z_;
	}

	inline float operator^(const Vector &v, const int n)
	{
		switch(n)
		{
		case 2:
			return v*v;
		default:
			assert(n == 2);   // FIXME: Compile time assert?
			return 1;
		}
	}
}

#endif   // VECTOR_H
