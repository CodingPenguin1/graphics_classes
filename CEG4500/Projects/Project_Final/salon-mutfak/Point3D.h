/*
 *  Point3D.h
 *  TestConsole
 *
 *  Created by Paul Fultz II on 4/4/09.
 *  Copyright 2009 KM Core. All rights reserved.
 *
 */

#ifndef _____Point3D_____
#define _____Point3D_____

#include "Operators.h"
#include <cmath>

namespace km { namespace Drawing {
	using namespace km::Core;
	
	
class Point3D : Addable<Point3D>, Subtractable<Point3D>, Multipliable<Point3D>, Equalable<Point3D>
{
public:
	double x;
	double y;
	double z;
	
	Point3D()
	{
		x=0;
		y=0;
		z=0;
	}

	template<typename U>
	Point3D(const U& p)
	{
		this->x = p.x;
		this->y = p.y;
		this->z = p.z;
	}
	
	Point3D(double x, double y, double z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	
	void Clear()
	{
		x=0;
		y=0;
		z=0;
	}
	
	Point3D& Scale(const double& k)
	{
		x *= k;
		y *= k;
		z *= k;
		return *this;
	}
	
	Point3D& operator+=(const Point3D& p)
	{
		this->x += p.x;
		this->y += p.y;
		this->z += p.z;
		return *this;
	}
	
	Point3D& operator-=(const Point3D& p)
	{
		this->x -= p.x;
		this->y -= p.y;
		this->z -= p.z;
		return *this;
	}
	
	Point3D& operator*=(const Point3D& p)
	{
		double x = (this->y*p.z) - (this->z*p.y);
		double y = (this->z*p.x) - (this->x*p.z);
		double z = (this->x*p.y) - (this->y*p.x);
		this->x = x;
		this->y = y;
		this->z = z;
		return *this;
	}
	
	double DotProduct(const Point3D& p) const
	{
		return ( (this->x * p.x) + (this->y * p.y) + (this->z * p.z));
	}
	
	double Magnitude() const
	{
		return (sqrt(x*x + y*y + z*z));
	}
	
	double Angle(const Point3D& p) const
	{
		return acos( this->DotProduct(p) / (this->Magnitude() * p.Magnitude()) );
	}
	
	Point3D& Normalize()
	{
		this->Scale(1.0/Magnitude());
		return *this;
	}
	
	bool operator==(const Point3D &p) const
	{
		return ( (this->x == p.x) && (this->y == p.y) && (this->z == p.z));
	}
	
	template<typename U>
	void CopyTo(U& p)
	{
		p.x = this->x;
		p.y = this->y;
		p.z = this->z;
	}
	
	static Point3D Empty;

	/*class operators : Addable<Point3D>, Subtractable<Point3D>, Multipliable<Point3D>, Equalable<Point3D>
	{
	
	};
	static operators Operators;*/
};

	
}}


#endif
