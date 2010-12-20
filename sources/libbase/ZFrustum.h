///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZFrustum.h
// Creation : 19/08/2007
// Author : Cedric Guillemet
// Description : Frustum culling
// Reliability : 
//
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZFRUSTUM_H__
#define ZFRUSTUM_H__

#include "../libbase/ZBaseMaths.h"

struct ZFrustum  
{
public:
    ZFrustum() {}
    virtual ~ZFrustum() {}
    
    
    void    Update(const tmatrix &view, const tmatrix& projection);

    bool    PointInFrustum( const tvector3 & vt ) const
    {
        // If you remember the plane equation (A*x + B*y + C*z + D = 0), then the rest
        // of this code should be quite obvious and easy to figure out yourself.
        // In case don't know the plane equation, it might be a good idea to look
        // at our Plane Collision tutorial at www.GameTutorials.com in OpenGL Tutorials.
        // I will briefly go over it here.  (A,B,C) is the (X,Y,Z) of the normal to the plane.
        // They are the same thing... but just called ABC because you don't want to say:
        // (x*x + y*y + z*z + d = 0).  That would be wrong, so they substitute them.
        // the (x, y, z) in the equation is the point that you are testing.  The D is
        // The distance the plane is from the origin.  The equation ends with "= 0" because
        // that is true when the point (x, y, z) is ON the plane.  When the point is NOT on
        // the plane, it is either a negative number (the point is behind the plane) or a
        // positive number (the point is in front of the plane).  We want to check if the point
        // is in front of the plane, so all we have to do is go through each point and make
        // sure the plane equation goes out to a positive number on each side of the frustum.
        // The result (be it positive or negative) is the distance the point is front the plane.

        // Go through all the sides of the frustum
        for(int i = 0; i < 6; i++ )
        {
            // Calculate the plane equation and check if the point is behind a side of the frustum
            if(m_Frustum[i][A] * vt.x + m_Frustum[i][B] * vt.y + m_Frustum[i][C] * vt.z + m_Frustum[i][D] <= 0)
            {
                // The point was behind a side, so it ISN'T in the frustum
                return false;
            }
        }

        // The point was inside of the frustum (In front of ALL the sides of the frustum)
        return true;
    }
    bool    SphereInFrustum( const tvector3 & vt, float radius ) const
    {
        // Now this function is almost identical to the PointInFrustum(), except we
        // now have to deal with a radius around the point.  The point is the center of
        // the radius.  So, the point might be outside of the frustum, but it doesn't
        // mean that the rest of the sphere is.  It could be half and half.  So instead of
        // checking if it's less than 0, we need to add on the radius to that.  Say the
        // equation produced -2, which means the center of the sphere is the distance of
        // 2 behind the plane.  Well, what if the radius was 5?  The sphere is still inside,
        // so we would say, if(-2 < -5) then we are outside.  In that case it's false,
        // so we are inside of the frustum, but a distance of 3.  This is reflected below.

        // Go through all the sides of the frustum
        for(int i = 0; i < 6; i++ )    
        {
            // If the center of the sphere is farther away from the plane than the radius
            if( m_Frustum[i][A] * vt.x + m_Frustum[i][B] * vt.y + m_Frustum[i][C] * vt.z + m_Frustum[i][D] <= -radius )
            {
                // The distance was greater than the radius so the sphere is outside of the frustum
                return false;
            }
        }
        
        // The sphere was inside of the frustum!
        return true;
    }

    bool    SphereInFrustum( const tvector4 & vt) const
    {
        for(int i = 0; i < 6; i++ )    
        {
            // If the center of the sphere is farther away from the plane than the radius
            if( m_Frustum[i][A] * vt.x + m_Frustum[i][B] * vt.y + m_Frustum[i][C] * vt.z + m_Frustum[i][D] <= -vt.w )
            {
                // The distance was greater than the radius so the sphere is outside of the frustum
                return false;
            }
        }
        
        // The sphere was inside of the frustum!
        return true;
    }

	int SphereInFrustumVis(const tvector4& v) const
	{

		float distance;
		int result = 2;

		for(int i=0; i < 6; i++) {
			distance = m_Frustum[i][A] * v.x + m_Frustum[i][B] * v.y + m_Frustum[i][C] * v.z + m_Frustum[i][D];//pl[i].distance(p);
			if (distance < -v.w)
				return 0;
			else if (distance < v.w)
				result =  1;
		}
		return(result);
	}


    bool    CubeInFrustum( const tvector3 & vt, float size ) const
    {
        // This test is a bit more work, but not too much more complicated.
        // Basically, what is going on is, that we are given the center of the cube,
        // and half the length.  Think of it like a radius.  Then we checking each point
        // in the cube and seeing if it is inside the frustum.  If a point is found in front
        // of a side, then we skip to the next side.  If we get to a plane that does NOT have
        // a point in front of it, then it will return false.

        // *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
        // This happens when all the corners of the bounding box are not behind any one plane.
        // This is rare and shouldn't effect the overall rendering speed.

        for(int i = 0; i < 6; i++ )
        {
            if(m_Frustum[i][A] * (vt.x - size) + m_Frustum[i][B] * (vt.y - size) + m_Frustum[i][C] * (vt.z - size) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x + size) + m_Frustum[i][B] * (vt.y - size) + m_Frustum[i][C] * (vt.z - size) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x - size) + m_Frustum[i][B] * (vt.y + size) + m_Frustum[i][C] * (vt.z - size) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x + size) + m_Frustum[i][B] * (vt.y + size) + m_Frustum[i][C] * (vt.z - size) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x - size) + m_Frustum[i][B] * (vt.y - size) + m_Frustum[i][C] * (vt.z + size) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x + size) + m_Frustum[i][B] * (vt.y - size) + m_Frustum[i][C] * (vt.z + size) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x - size) + m_Frustum[i][B] * (vt.y + size) + m_Frustum[i][C] * (vt.z + size) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x + size) + m_Frustum[i][B] * (vt.y + size) + m_Frustum[i][C] * (vt.z + size) + m_Frustum[i][D] > 0)
            continue;

            // If we get here, it isn't in the frustum
            return false;
        }

        return true;
    }

    bool    BoxInFrustum( const tvector3 & vt, const tvector3 & size ) const
    {
        for(int i = 0; i < 6; i++ )
        {
            if(m_Frustum[i][A] * (vt.x - size.x) + m_Frustum[i][B] * (vt.y - size.y) + m_Frustum[i][C] * (vt.z - size.z) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x + size.x) + m_Frustum[i][B] * (vt.y - size.y) + m_Frustum[i][C] * (vt.z - size.z) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x - size.x) + m_Frustum[i][B] * (vt.y + size.y) + m_Frustum[i][C] * (vt.z - size.z) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x + size.x) + m_Frustum[i][B] * (vt.y + size.y) + m_Frustum[i][C] * (vt.z - size.z) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x - size.x) + m_Frustum[i][B] * (vt.y - size.y) + m_Frustum[i][C] * (vt.z + size.z) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x + size.x) + m_Frustum[i][B] * (vt.y - size.y) + m_Frustum[i][C] * (vt.z + size.z) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x - size.x) + m_Frustum[i][B] * (vt.y + size.y) + m_Frustum[i][C] * (vt.z + size.z) + m_Frustum[i][D] > 0)
            continue;
            if(m_Frustum[i][A] * (vt.x + size.x) + m_Frustum[i][B] * (vt.y + size.y) + m_Frustum[i][C] * (vt.z + size.z) + m_Frustum[i][D] > 0)
            continue;

            // If we get here, it isn't in the frustum
            return false;
        }

        return true;
    }

	// matrix is an orthonormalized matrix. only orientation is used.
	bool OBBInFrustum( const tmatrix &mt, const tvector3 &pos, const tvector3& size) const;

private:

    float m_Frustum[6][4];
    void NormalizePlane(float frustum[6][4], int side);

    enum FrustumSide
    {
        RIGHT    = 0,        // The RIGHT side of the frustum
        LEFT    = 1,        // The LEFT     side of the frustum
        BOTTOM    = 2,        // The BOTTOM side of the frustum
        TOP        = 3,        // The TOP side of the frustum
        BACK    = 4,        // The BACK    side of the frustum
        FRONT    = 5            // The FRONT side of the frustum
    }; 

    // Like above, instead of saying a number for the ABC and D of the plane, we
    // want to be more descriptive.
    enum PlaneData
    {
        A = 0,                // The X value of the plane's normal
        B = 1,                // The Y value of the plane's normal
        C = 2,                // The Z value of the plane's normal
        D = 3                // The distance the plane is from the origin
    };
};

#endif
