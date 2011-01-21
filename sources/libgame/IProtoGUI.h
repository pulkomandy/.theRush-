#ifndef IPROTOGUI_H__
#define IPROTOGUI_H__

#include "../libbase/ZSerializator.h"
#include "../libbase/ZFunctionSlot.h"
#include "IProtoGUI.h"

DECLAREZCLASS(ZProtoGUI);

class IProtoGUI : public ZBaseClass
{
public:
	virtual void BuildLoadingScreen(const char *szImgName) = 0;
	virtual void SetProgress(float aVal) = 0;
	virtual void Tick() = 0;
	virtual void ShowLan(const ZSubscriberSlot& onBack) = 0;
	virtual void ShowLanServer(const ZSubscriberSlot& onBack) = 0;

	virtual void BuildIngameMenu() = 0;
	virtual void BuildUnableToConnect(const char *szReason) = 0;
	virtual void ShowConnectingToServer(const char *szServer) = 0;
	virtual void BuildDisconnection(const char *szReason) = 0;

	// Net States
	virtual void BuildSpectatorPlayerGui() = 0;

	virtual void SetNbSlotsAvailable(int nbSlots) = 0;
	virtual void ShowLanShipSelection() = 0;

	virtual void ShowTimeBeforeNextMap(float timebefore) = 0;
	virtual void ShowTimeBeforeCountDown(float timebefore) = 0;
	virtual void ShowTimeBeforeRaceEnds(float timebefore) = 0;
	virtual void ShowWaitingForPlayers() = 0;
	virtual void ValidateLanSelShip() = 0;
	// Hide everything
	// Call it in case of deconnection
	//virtual void Hide() = 0;

	virtual void ShowEndResult() = 0;


	// Music
	virtual void PlayNewMusic() = 0;
	virtual void MusicVolumeChanged() = 0;

	virtual void ShowMainMenu() = 0;

	virtual void HideInGameMenu() = 0;
	virtual void HideEndResults() = 0;


	virtual void ShowQuickRace(const ZSubscriberSlot& onBack) = 0;

	// called when a ship is spawn or removed
	virtual void UpdateConnectedPlayers() = 0;

	virtual void ShowSpectator() = 0;
	virtual void NicknameChanged() = 0;

	virtual void BuildGameSetForMenu() = 0;

	// Sounds
	virtual void PlayErrorSound() = 0;
	virtual void PlayQuestionSound() = 0;
	virtual void PlayOKSound() = 0;
	virtual void PlayCancelSound() = 0;

	// chosen ship
	virtual tstring& GetChosenShipString() = 0;
};

extern IProtoGUI* GProtoGui;

void IncStackCount();
void DecStackCount();

void ZShowCursor();
void ZHideCursor();
int ZGetCursorVisibility();
#endif
