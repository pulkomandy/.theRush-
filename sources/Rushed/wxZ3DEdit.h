#ifndef WXZ3DEDIT_H__
#define WXZ3DEDIT_H__

#include "../LibEdit/LibEdit.h"

enum EDIT_MODES
{
    EDIT_MODE_SELECT,
    EDIT_MODE_MOVE,
    EDIT_MODE_ROTATE,
    EDIT_MODE_SCALE,
    EDIT_MODE_PLUGIN,
};

enum EDIT_LOCATE
{
    LOCATE_VIEW,
    LOCATE_WORLD,
    LOCATE_LOCAL,
};

class ZSoundInstance;

class Zen3DEdit
{
public:
	Zen3DEdit();
	virtual ~Zen3DEdit();

	void SetEditMode(EDIT_MODES aMode);

    void SetGizmoSnap(bool bSnapMove, bool bSnapRotate, bool bSnapScale);
    void DeleteSelected();

    void DrawGrid(bool bDraw) { mbDrawGrid = bDraw; }
    bool IsDrawGrid() { return mbDrawGrid; }

    void SetLocate(EDIT_LOCATE mLocate);

	// world
	virtual void LoadWorld(const char *szWorldFileName) {}
	virtual void SaveWorld(const char *szWorldFileName) {}
	virtual void LoadDefaultTrack(int worldNumber) {}
	// selection

	void SelectOccluder(ZOccluderBox *pOcc);
	void SelectLight(ZLight *pLight);
#ifdef FIXED	
	void SelectEnv(ISky* pcls);
	void SelectMetaEmitter(IMetaEmitter* pcls);
	void SelectParticleSys(IMetaEmitter* pme, IParticleSystem* pcls);
#endif
	void SelectSoundInstance(ZSoundInstance *psnd);

	virtual void SetSelection(ZSpatialEntity *pse) {}
	// Mouse support
	bool OnLButtonUp(int X, int Y);
	bool OnLButtonDown(int X, int Y, bool bHasShiftDown, int wSizeX, int wSizeY);
	bool OnMouseMotion(int X, int Y);
	bool IsDocumentModified() { return mbDocumentModified; }
	void ClearDocumentModifiedFlag();
	void SetDocumentModified();
public:
	// gizmo edit

    smartptr<IGizmo >        mGizmoEditMove;
    smartptr<IGizmo >        mGizmoEditRotate;
    smartptr<IGizmo >        mGizmoEditScale;

	IGizmo *        mGizmoEdit;

    bool mbGizmoEditing;
    bool mbDrawGrid;

	int mEditAnimationKey;
	int mEditRoadMeshIndex;
	tmatrix mMetaEmitEditMatrix;
	ZBaseClass *mCurrentBaseClassEditing;
	ZBaseClass *mCurrentSubBaseClassEditing;

	friend class ZenFWGizmosNode;
	bool mbDocumentModified;

};

#endif