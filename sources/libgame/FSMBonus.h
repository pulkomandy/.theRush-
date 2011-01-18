//////////////////////////////////////////////////////////////////////////////////////////////
/*
	ShipBonus State machine
	
	File generated 01/02/2010 at 15:47:37
	
	Do not edit this file directly.
	Use Rapido! by Cedric Guillemet.
	
	Rapido! is licensed under GPLV2. Here is a copy of Rapido! License :
	
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; version 2 of the License.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
*/
//////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ShipBonus_H__
#define ShipBonus_H__

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../libbase/ZFSM.h"
#include "ZShip.h"



///////////////////////////////////////////////////////////////////////////////////////////////////

enum {
Type_NewBonus,
Type_TriggerBonus

};

///////////////////////////////////////////////////////////////////////////////////////////////////




/*

*/
typedef struct NewBonus
{
int mBonusIndex;
} NewBonus;




/*

*/
typedef struct TriggerBonus
{

} TriggerBonus;



///////////////////////////////////////////////////////////////////////////////////////////////////

class ShipBonus : public ZFSM
{
public:
	ShipBonus()
	{
		Init();
	}
	
	void Init()
	{
		
		mState = NoBonus;
		EnterNoBonus();	
	}

	//////////////////////////////////////////////////////////////////////////////////////////////
	void Tick(float timeEllapsed);

	//////////////////////////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////////////////////////
	void Handle(const NewBonus& mes)
{
	switch(mState)
	{
		case NoBonus:
			HandleNoBonus(mes);
			break;
		case Missile:
			HandleMissile(mes);
			break;
		case HomingMissile:
			HandleHomingMissile(mes);
			break;
		case Mines:
			HandleMines(mes);
			break;
		case MinesDropping:
			HandleMinesDropping(mes);
			break;
		case Shield:
			HandleShield(mes);
			break;
		case ShieldActivated:
			HandleShieldActivated(mes);
			break;
		case DestructionShield:
			HandleDestructionShield(mes);
			break;
		case DestructionShieldActivated:
			HandleDestructionShieldActivated(mes);
			break;
		case Autopilot:
			HandleAutopilot(mes);
			break;
		case AutopilotActivated:
			HandleAutopilotActivated(mes);
			break;
		case MoreEnergy:
			HandleMoreEnergy(mes);
			break;
		case GravityGrip:
			HandleGravityGrip(mes);
			break;
		case GravityGripActivated:
			HandleGravityGripActivated(mes);
			break;
		case Booster:
			HandleBooster(mes);
			break;
		case BoosterActivated:
			HandleBoosterActivated(mes);
			break;
		case MachineGun:
			HandleMachineGun(mes);
			break;
		case MachineGunActivated:
			HandleMachineGunActivated(mes);
			break;
	}
}

void Handle(const TriggerBonus& mes)
{
	switch(mState)
	{
		case NoBonus:
			HandleNoBonus(mes);
			break;
		case Missile:
			HandleMissile(mes);
			break;
		case HomingMissile:
			HandleHomingMissile(mes);
			break;
		case Mines:
			HandleMines(mes);
			break;
		case MinesDropping:
			HandleMinesDropping(mes);
			break;
		case Shield:
			HandleShield(mes);
			break;
		case ShieldActivated:
			HandleShieldActivated(mes);
			break;
		case DestructionShield:
			HandleDestructionShield(mes);
			break;
		case DestructionShieldActivated:
			HandleDestructionShieldActivated(mes);
			break;
		case Autopilot:
			HandleAutopilot(mes);
			break;
		case AutopilotActivated:
			HandleAutopilotActivated(mes);
			break;
		case MoreEnergy:
			HandleMoreEnergy(mes);
			break;
		case GravityGrip:
			HandleGravityGrip(mes);
			break;
		case GravityGripActivated:
			HandleGravityGripActivated(mes);
			break;
		case Booster:
			HandleBooster(mes);
			break;
		case BoosterActivated:
			HandleBoosterActivated(mes);
			break;
		case MachineGun:
			HandleMachineGun(mes);
			break;
		case MachineGunActivated:
			HandleMachineGunActivated(mes);
			break;
	}
}



	//////////////////////////////////////////////////////////////////////////////////////////////
public:

	enum STATE
	{
	NoBonus,
Missile,
HomingMissile,
Mines,
MinesDropping,
Shield,
ShieldActivated,
DestructionShield,
DestructionShieldActivated,
Autopilot,
AutopilotActivated,
MoreEnergy,
GravityGrip,
GravityGripActivated,
Booster,
BoosterActivated,
MachineGun,
MachineGunActivated

	};

	//////////////////////////////////////////////////////////////////////////////////////////////
	
	STATE mState;
	ZShip *mShip;

	//////////////////////////////////////////////////////////////////////////////////////////////
	
	void SetState(STATE newState)
	{
			switch(mState)
	{
		case NoBonus:
			LeaveNoBonus();
			break;
		case Missile:
			LeaveMissile();
			break;
		case HomingMissile:
			LeaveHomingMissile();
			break;
		case Mines:
			LeaveMines();
			break;
		case MinesDropping:
			LeaveMinesDropping();
			break;
		case Shield:
			LeaveShield();
			break;
		case ShieldActivated:
			LeaveShieldActivated();
			break;
		case DestructionShield:
			LeaveDestructionShield();
			break;
		case DestructionShieldActivated:
			LeaveDestructionShieldActivated();
			break;
		case Autopilot:
			LeaveAutopilot();
			break;
		case AutopilotActivated:
			LeaveAutopilotActivated();
			break;
		case MoreEnergy:
			LeaveMoreEnergy();
			break;
		case GravityGrip:
			LeaveGravityGrip();
			break;
		case GravityGripActivated:
			LeaveGravityGripActivated();
			break;
		case Booster:
			LeaveBooster();
			break;
		case BoosterActivated:
			LeaveBoosterActivated();
			break;
		case MachineGun:
			LeaveMachineGun();
			break;
		case MachineGunActivated:
			LeaveMachineGunActivated();
			break;
	}
mState = newState;	switch(newState)
	{
		case NoBonus:
			EnterNoBonus();
			break;
		case Missile:
			EnterMissile();
			break;
		case HomingMissile:
			EnterHomingMissile();
			break;
		case Mines:
			EnterMines();
			break;
		case MinesDropping:
			EnterMinesDropping();
			break;
		case Shield:
			EnterShield();
			break;
		case ShieldActivated:
			EnterShieldActivated();
			break;
		case DestructionShield:
			EnterDestructionShield();
			break;
		case DestructionShieldActivated:
			EnterDestructionShieldActivated();
			break;
		case Autopilot:
			EnterAutopilot();
			break;
		case AutopilotActivated:
			EnterAutopilotActivated();
			break;
		case MoreEnergy:
			EnterMoreEnergy();
			break;
		case GravityGrip:
			EnterGravityGrip();
			break;
		case GravityGripActivated:
			EnterGravityGripActivated();
			break;
		case Booster:
			EnterBooster();
			break;
		case BoosterActivated:
			EnterBoosterActivated();
			break;
		case MachineGun:
			EnterMachineGun();
			break;
		case MachineGunActivated:
			EnterMachineGunActivated();
			break;
	}
		
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	inline void TickNoBonus(float timeEllapsed)
{


}
inline void TickMissile(float timeEllapsed)
{


}
inline void TickHomingMissile(float timeEllapsed)
{


}
inline void TickMines(float timeEllapsed)
{


}
inline void TickMinesDropping(float timeEllapsed)
{

SetState(NoBonus);

}
inline void TickShield(float timeEllapsed)
{


}
inline void TickShieldActivated(float timeEllapsed)
{

SetState(NoBonus);

}
inline void TickDestructionShield(float timeEllapsed)
{


}
inline void TickDestructionShieldActivated(float timeEllapsed)
{

SetState(NoBonus);
}
inline void TickAutopilot(float timeEllapsed)
{


}
inline void TickAutopilotActivated(float timeEllapsed)
{

SetState(NoBonus);
}
inline void TickMoreEnergy(float timeEllapsed)
{


}
inline void TickGravityGrip(float timeEllapsed)
{


}
inline void TickGravityGripActivated(float timeEllapsed)
{

SetState(NoBonus);

}
inline void TickBooster(float timeEllapsed)
{


}
inline void TickBoosterActivated(float timeEllapsed)
{

SetState(NoBonus);

}
inline void TickMachineGun(float timeEllapsed)
{


}
inline void TickMachineGunActivated(float timeEllapsed)
{

SetState(NoBonus);
}

	
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	inline void EnterNoBonus()
{


}
inline void EnterMissile()
{


}
inline void EnterHomingMissile()
{


}
inline void EnterMines()
{


}
inline void EnterMinesDropping()
{


}
inline void EnterShield()
{


}
inline void EnterShieldActivated()
{


}
inline void EnterDestructionShield()
{


}
inline void EnterDestructionShieldActivated()
{


}
inline void EnterAutopilot()
{


}
inline void EnterAutopilotActivated()
{


}
inline void EnterMoreEnergy()
{


}
inline void EnterGravityGrip()
{


}
inline void EnterGravityGripActivated()
{


}
inline void EnterBooster()
{


}
inline void EnterBoosterActivated()
{


}
inline void EnterMachineGun()
{


}
inline void EnterMachineGunActivated()
{


}

	
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	inline void LeaveNoBonus()
{


}
inline void LeaveMissile()
{


}
inline void LeaveHomingMissile()
{


}
inline void LeaveMines()
{


}
inline void LeaveMinesDropping()
{


}
inline void LeaveShield()
{


}
inline void LeaveShieldActivated()
{


}
inline void LeaveDestructionShield()
{


}
inline void LeaveDestructionShieldActivated()
{


}
inline void LeaveAutopilot()
{


}
inline void LeaveAutopilotActivated()
{


}
inline void LeaveMoreEnergy()
{


}
inline void LeaveGravityGrip()
{


}
inline void LeaveGravityGripActivated()
{


}
inline void LeaveBooster()
{


}
inline void LeaveBoosterActivated()
{


}
inline void LeaveMachineGun()
{


}
inline void LeaveMachineGunActivated()
{


}

	
	//////////////////////////////////////////////////////////////////////////////////////////////
	
	void HandleNoBonus(const NewBonus& mes)
{
switch (mes.mBonusIndex)
{
case 1:
    SetState(Missile);
    break;
case 2:
    SetState(HomingMissile);
    break;
case 3:
    SetState(Mines);
    break;
case 4:
    SetState(Shield);
    break;
case 5:
    SetState(DestructionShield);
    break;
case 6:
    SetState(Autopilot);
    break;
case 7:
    SetState(MoreEnergy);
    break;
case 8:
    SetState(GravityGrip);
    break;
case 9:
    SetState(Booster);
    break;
case 10:
    SetState(MachineGun);
    break;

}

}

void HandleNoBonus(const TriggerBonus& mes)
{

}

void HandleMissile(const NewBonus& mes)
{

}

void HandleMissile(const TriggerBonus& mes)
{
SetState(NoBonus);

}

void HandleHomingMissile(const NewBonus& mes)
{

}

void HandleHomingMissile(const TriggerBonus& mes)
{
SetState(NoBonus);

}

void HandleMines(const NewBonus& mes)
{

}

void HandleMines(const TriggerBonus& mes)
{
SetState(MinesDropping);

}

void HandleMinesDropping(const NewBonus& mes)
{

}

void HandleMinesDropping(const TriggerBonus& mes)
{

}

void HandleShield(const NewBonus& mes)
{

}

void HandleShield(const TriggerBonus& mes)
{
SetState(ShieldActivated);

}

void HandleShieldActivated(const NewBonus& mes)
{

}

void HandleShieldActivated(const TriggerBonus& mes)
{

}

void HandleDestructionShield(const NewBonus& mes)
{

}

void HandleDestructionShield(const TriggerBonus& mes)
{
SetState(DestructionShieldActivated);
}

void HandleDestructionShieldActivated(const NewBonus& mes)
{

}

void HandleDestructionShieldActivated(const TriggerBonus& mes)
{

}

void HandleAutopilot(const NewBonus& mes)
{

}

void HandleAutopilot(const TriggerBonus& mes)
{
SetState(AutopilotActivated);

}

void HandleAutopilotActivated(const NewBonus& mes)
{

}

void HandleAutopilotActivated(const TriggerBonus& mes)
{

}

void HandleMoreEnergy(const NewBonus& mes)
{

}

void HandleMoreEnergy(const TriggerBonus& mes)
{
SetState(NoBonus);

}

void HandleGravityGrip(const NewBonus& mes)
{

}

void HandleGravityGrip(const TriggerBonus& mes)
{
SetState(GravityGripActivated);
}

void HandleGravityGripActivated(const NewBonus& mes)
{

}

void HandleGravityGripActivated(const TriggerBonus& mes)
{

}

void HandleBooster(const NewBonus& mes)
{

}

void HandleBooster(const TriggerBonus& mes)
{
SetState(BoosterActivated);
}

void HandleBoosterActivated(const NewBonus& mes)
{

}

void HandleBoosterActivated(const TriggerBonus& mes)
{

}

void HandleMachineGun(const NewBonus& mes)
{

}

void HandleMachineGun(const TriggerBonus& mes)
{
SetState(MachineGunActivated);
}

void HandleMachineGunActivated(const NewBonus& mes)
{

}

void HandleMachineGunActivated(const TriggerBonus& mes)
{

}


	
	//////////////////////////////////////////////////////////////////////////////////////////////
};

///////////////////////////////////////////////////////////////////////////////////////////////////

inline void ShipBonus::Tick(float timeEllapsed)
{
	ZFSM::Tick(timeEllapsed);
	
		switch(mState)
	{
		case NoBonus:
			TickNoBonus(timeEllapsed);
			break;
		case Missile:
			TickMissile(timeEllapsed);
			break;
		case HomingMissile:
			TickHomingMissile(timeEllapsed);
			break;
		case Mines:
			TickMines(timeEllapsed);
			break;
		case MinesDropping:
			TickMinesDropping(timeEllapsed);
			break;
		case Shield:
			TickShield(timeEllapsed);
			break;
		case ShieldActivated:
			TickShieldActivated(timeEllapsed);
			break;
		case DestructionShield:
			TickDestructionShield(timeEllapsed);
			break;
		case DestructionShieldActivated:
			TickDestructionShieldActivated(timeEllapsed);
			break;
		case Autopilot:
			TickAutopilot(timeEllapsed);
			break;
		case AutopilotActivated:
			TickAutopilotActivated(timeEllapsed);
			break;
		case MoreEnergy:
			TickMoreEnergy(timeEllapsed);
			break;
		case GravityGrip:
			TickGravityGrip(timeEllapsed);
			break;
		case GravityGripActivated:
			TickGravityGripActivated(timeEllapsed);
			break;
		case Booster:
			TickBooster(timeEllapsed);
			break;
		case BoosterActivated:
			TickBoosterActivated(timeEllapsed);
			break;
		case MachineGun:
			TickMachineGun(timeEllapsed);
			break;
		case MachineGunActivated:
			TickMachineGunActivated(timeEllapsed);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif

