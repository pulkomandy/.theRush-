// AutomaticDoorExample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <windows.h>
#include "../res.h"

// Affichage d'une porte suivant un facteur de fermeture
// Et le nom de l'etat.
void DrawDoor(float openRatio, const char *szState)
{
	printf("[");
	int doorWidth = 20;
	int drawRatio = int(openRatio*doorWidth) ;
	int spaceRatio = doorWidth-drawRatio;
	for (int i=0;i<drawRatio;i++)
		printf("=");
	for (int i=0;i<spaceRatio;i++)
		printf(" ");
	printf("] %s    \r", szState);
}
int _tmain(int argc, _TCHAR* argv[])
{
	// Instance de la FSM
	AutomaticDoor mDoor;

	bool quit = false;
	while(!quit)
	{
		// Gestion du clavier
		// ESC pour quitter
		char c=0;
		int i=kbhit();  
         if (i!=0)  
		 {
             c=getch();  
			 // si on a un appuie de touche, on envoi l'evenement
			 // a la FSM.
			 // Ca émule un trigger dans la porte
			 mDoor.Handle(Trigger());
		 }
		if (c == 27)
			quit = true;

		// 10Hz update
		mDoor.Tick(0.1f);
		Sleep(100);

	}
	printf("\n");
	return 0;
}

