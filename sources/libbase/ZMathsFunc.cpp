///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : PSM_Math.cpp
// Creation : 07/07/2001
// Author : Cedric Guillemet
// Description : Mathematic functions and constants
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
// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "ZMathsFunc.h"
// Globals ////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////

float MathFloatBezierCubic(const float &v1, const float &v2, const float& v3, const float& v4, const float s)
{
    float x = v1 * MathBezierCubicSplineF4(s) + v2 * MathBezierCubicSplineF3(s) + v4 * MathBezierCubicSplineF2(s) + v3 * MathBezierCubicSplineF1(s);
    return x;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
