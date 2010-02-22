///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZPIDCtrl.h
// Creation : 17/01/2008
// Author : Cedric Guillemet
// Description :
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

#ifndef PIDCTRL_H__
#define PIDCTRL_H__


template<typename T> struct ZPID
{
	inline ZPID()
	{
		Ki = 1;
		Kp = 1;
		Kd = 1;
		I = 0.f;
	}
	inline ZPID(float _Ki, float _Kp, float _Kd)
	{
		Ki = _Ki;
		Kp = _Kp;
		Kd = _Kd;
		I = 0.f;
	}
	inline void SetIPD(float _Ki, float _Kp, float _Kd)
	{
		Ki = _Ki;
		Kp = _Kp;
		Kd = _Kd;
	}
	/*
	start:
	previous_error = error or 0 if undefined
	error = setpoint - actual_position
	P = Kp * error
	I = I + Ki * error * dt
	D = Kd * (error - previous_error) / dt
	output = P + I + D
	wait(dt)
	goto start
	*/
	inline T Compute(const T& desiredPos, const T& currentPos, float dt)
	{
		previouserror = error;
		error = desiredPos - currentPos;
		T P = Kp * error;
		I += Ki * error * dt;
		T D = Kd * (error - previouserror) / dt;
		T output = P + I + D;
		return output;
	}

	T error, previouserror, I;
	float Ki, Kp, Kd;
};
/*
Example of use

	// reset position/velocity/desired position
	currentPos = 0;
	currentVel = 0;
	desiredPos = 20;

	PID<float> mPid(1,1,1);

	printf("--------------------------------%d\n", sizeof(PID<float>));

	for (int i=0;i<50;i++)
	{
		float output = mPid.Compute(desiredPos, currentPos, 0.1f);

		// this part of the code is similar to the code above
		// simulate turning wheel
		if (output> currentPos)
			if (currentVel<1.0f) currentVel += 0.2f;

		if (output< currentPos)
			if (currentVel>-1.0f) currentVel -= 0.2f;

		currentPos+=currentVel;

		printf("%5.2f\n",currentPos);
	}

*/

#endif
