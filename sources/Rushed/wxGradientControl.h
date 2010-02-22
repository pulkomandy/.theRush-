///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxGradientControl.h
// Creation : 15/03/2007
// Author : Cedric Guillemet
// Description :
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

#ifndef WXGRADIENTCONTROL_H__
#define WXGRADIENTCONTROL_H__

#include <wx/wx.h>
#include "../libbase/LibBase.h"
/*
#include <wx/window.h>
#include <wx/grid.h>
#include <wx/treectrl.h>
#include <wx/spinctrl.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/image.h>
#include <wx/colordlg.h>
#include <wx/wxhtml.h>
#include <wx/stc/stc.h>
#include <wx/aui/aui.h>

#include "../LibWorld/ZAnimationTrack.h"
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct GradientEntry
{
    GradientEntry() {}
    GradientEntry(float aPos, const tcolor& aCol, float aSize, float aRot)
    {
        mPosition = aPos;
        mColor = aCol;
        mSize = aSize;
		mRot = aRot;
    }

    float mPosition;
    float mSize;
	float mRot;
    tcolor mColor;
} GradientEntry;

class wxZEditHandler;

///////////////////////////////////////////////////////////////////////////////////////////////////

class wxGradientControl : public wxPanel
{
public:
    wxGradientControl(wxWindow* parent, wxZEditHandler * aHandler,
                    ZAnimationTrack<tcolor> *animColor = NULL,
                    ZAnimationTrack<float> *animSize = NULL,
					ZAnimationTrack<float> *animRot = NULL,
                    float *pLifeTime = NULL,
                     wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize);

    virtual ~wxGradientControl();


    void AddEntry(const GradientEntry& entry);

    const GradientEntry* GetSelectedEntry() { return mpSelected; }

    void SetSelectedColor(const tcolor & aCol);
    void SetSelectedAlpha(const tcolor & aAlpha);
    void SetSelectedSize(float aSize);
	void SetSelectedRot(float aRot);

    void SetSelected(const tcolor & aCol, float aSize, float aRot);

	virtual void SetGradientRamp(tcolor *pCol, int nbCols)
	{
		mRampCol = pCol;
		mNbRampCols = nbCols;
	}

	virtual void SetRotationRamp(float *pRot, int nbRots)
	{
		mRampRot = pRot;
		mNbRampRots = nbRots;
	}

	virtual void SetScaleRamp(float *pScale, int nbScales)
	{
		mRampScale = pScale;
		mNbRampScales = nbScales;
	}

protected:
    DECLARE_EVENT_TABLE();
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnLButtonDown(wxMouseEvent& event);
    void OnLButtonUp(wxMouseEvent& event);
	void OnRButtonDown(wxMouseEvent& event);
    void OnMouseMotion(wxMouseEvent& event);
    void OnLDblClick(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
	void OnLostCapture(wxMouseCaptureLostEvent& event);


    void BuildGradient();
    void GetInterpolation(float nPos, GradientEntry& entry, float &fPrev, float &fNext);

    wxBitmap mBitmap;
    wxMemoryDC memDC;
    wxSize mSize;
    bool mbCanMove;
    int mX, mY;
    float mPrevLock, mNextLock;

    tarray<GradientEntry> mEntries;
    static int GradientSortFunc(const void *a1, const void *a2);

    wxBitmap mPointer, mPointerSelected;
    GradientEntry *mpSelected;
    ZAnimationTrack<tcolor> *mAnimColor;
    ZAnimationTrack<float> *mAnimSize;
	ZAnimationTrack<float> *mAnimRot;
    float *mLifeTime;
    wxZEditHandler * mHandler;


	tcolor *mRampCol;
	int mNbRampCols;

	float *mRampRot;
	int mNbRampRots;

	float *mRampScale;
	int mNbRampScales;

};

#endif
