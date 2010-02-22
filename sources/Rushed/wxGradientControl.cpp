///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxGradientControl.cpp
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
//#include "wxStdAfx.h"
#include "wxGradientControl.h"
//#include "../Base/PSM_Memory.h"
#include "wxStackedControls.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGradientControl, wxPanel)
EVT_PAINT(wxGradientControl::OnPaint)
EVT_SIZE(wxGradientControl::OnSize)
EVT_LEFT_DOWN(wxGradientControl::OnLButtonDown) 	    //Process a wxEVT_LEFT_DOWN event. The handler of this event should normally call event.Skip() to allow the default processing to take place as otherwise the window under mouse wouldn't get the focus.
EVT_RIGHT_DOWN(wxGradientControl::OnRButtonDown)
EVT_LEFT_UP(wxGradientControl::OnLButtonUp) 	        //Process a wxEVT_LEFT_UP event.
EVT_MOTION(wxGradientControl::OnMouseMotion) 	        //Process a wxEVT_MOTION event.
EVT_LEFT_DCLICK(wxGradientControl::OnLDblClick)
EVT_CHAR(wxGradientControl::OnKeyDown)
EVT_KEY_UP(wxGradientControl::OnKeyDown)
EVT_KEY_DOWN(wxGradientControl::OnKeyDown)
EVT_MOUSE_CAPTURE_LOST(wxGradientControl::OnLostCapture)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////////////////////////

wxGradientControl::wxGradientControl(wxWindow* parent, wxZEditHandler * aHandler,
                    ZAnimationTrack<tcolor> *animColor,
                    ZAnimationTrack<float> *animSize,
					ZAnimationTrack<float> *animRot,
                    float *pLifeTime,
                    wxWindowID id ,
                     const wxPoint& pos ,
                     const wxSize& size )
                     : wxPanel(parent, id, pos, size, wxNO_BORDER)
{
    mAnimColor = animColor;
    mAnimSize = animSize;
	mAnimRot = animRot;
    mLifeTime = pLifeTime;
    mHandler = aHandler;

	mRampCol = NULL;
	mRampRot = NULL;
	mRampScale = NULL;


    if (mAnimColor && mAnimSize && mAnimRot)
    {
        tcolor tcol0 = mAnimColor->m_KeyFrameList[0].m_Value;
        float tsize0 = mAnimSize->m_KeyFrameList[0].m_Value;
		float trot0 = mAnimRot->m_KeyFrameList[0].m_Value;
        tcolor tcol1 = mAnimColor->m_KeyFrameList[mAnimColor->m_KeyFrameList.size()-1].m_Value;
        float tsize1 = mAnimSize->m_KeyFrameList[mAnimSize->m_KeyFrameList.size()-1].m_Value;
		float trot1 = mAnimRot->m_KeyFrameList[mAnimRot->m_KeyFrameList.size()-1].m_Value;
        //tcol0.SwapRB();
        //tcol1.SwapRB();
        AddEntry(GradientEntry(0, tcol0, tsize0, trot0));
        AddEntry(GradientEntry(1, tcol1, tsize1, trot1));

        for (uint i=1;i<mAnimColor->m_KeyFrameList.size()-1;i++)
        {
            tcolor tcoln = mAnimColor->m_KeyFrameList[i].m_Value;
            tcoln.SwapRB();
            float tsizen = mAnimSize->m_KeyFrameList[i].m_Value;
			float trotn = mAnimRot->m_KeyFrameList[i].m_Value;
            float ttime = mAnimSize->m_KeyFrameList[i].m_Time/(*mLifeTime * 1000.0f);
            AddEntry(GradientEntry(ttime, tcoln, tsizen, trotn));

        }
    }
    else
    {
        // default
        AddEntry(GradientEntry(0, tcolor(1,1,1,0), 1, 0));
        AddEntry(GradientEntry(1, tcolor(0,1,0,1), 1, 0));
    }
    //AddEntry(GradientEntry(0.5,tcolor(1,0,1,0.8),1));

    mPointer = wxBitmap(_T("./ZenithRes/GradientPointer.bmp"), wxBITMAP_TYPE_BMP);
    mPointerSelected = wxBitmap(_T("./ZenithRes/GradientPointerSelected.bmp"), wxBITMAP_TYPE_BMP);
    mpSelected = NULL;
    mbCanMove = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

wxGradientControl::~wxGradientControl()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);

    dc.Blit(4, 5, mSize.x-8, mSize.y-9, &memDC, 0, 0 );
    for (uint i=0;i<mEntries.size();i++)
    {
        if (mpSelected == &mEntries[i])
            dc.DrawBitmap(mPointerSelected, mEntries[i].mPosition*(mSize.x-9), 0, true);
        else
            dc.DrawBitmap(mPointer, mEntries[i].mPosition*(mSize.x-9), 0, true);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnSize(wxSizeEvent& event)
{
    //wxFrame:::OnSize(event);
    mSize = GetClientSize();
    mBitmap.Create(mSize.x, mSize.y);
    BuildGradient();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::BuildGradient()
{
	unsigned int i;

    //qsort(&mEntries[0], mEntries.size(), sizeof(GradientEntry), GradientSortFunc);

    GradientEntry* tmpEntries = (GradientEntry*)alloca(sizeof(GradientEntry)*mEntries.size());
    for (i=0;i<mEntries.size();i++)
        tmpEntries[i] = mEntries[i];

    qsort(&tmpEntries[0], mEntries.size(), sizeof(GradientEntry), GradientSortFunc);

	// -- build ramps
	if (mRampCol)
	{

		GradientEntry tmp;
		uint li =0;
		for (float i = 0;i<=1;i+=(1.f/float(mNbRampCols-1)))
		{
			float aprev, anext;

			GetInterpolation(i, tmp, aprev, anext);
			mRampCol[li] = tmp.mColor;
			mRampScale[li] = tmp.mSize;
			mRampRot[li] = tmp.mRot;
			li++;
		}

	}

	// --
    memDC.SelectObject(mBitmap);

    memDC.SetPen(*wxBLACK_PEN);
    memDC.SetBrush(*wxTRANSPARENT_BRUSH);
    memDC.DrawRectangle(0,0,mSize.x-8,mSize.y-9);
    memDC.DrawLine(0,0,mSize.x-8,mSize.y-9);



    wxRect rect;
    rect.SetTop(1);
    rect.SetBottom(1+(mSize.y-12)*0.6666667);

    // color
    for (uint i=0;i<mEntries.size()-1;i++)
    {
        rect.SetLeft(tmpEntries[i].mPosition*(mSize.x-11)+1);
        rect.SetRight(tmpEntries[i+1].mPosition*(mSize.x-11)+1);

        wxColor col1 = tcolorTowxColour(tmpEntries[i].mColor);
        wxColor col2 = tcolorTowxColour(tmpEntries[i+1].mColor);
        memDC.GradientFillLinear(rect, col1, col2);
    }

    // alpha
    rect.SetTop(rect.GetBottom());
    rect.SetBottom(mSize.y -11);

    for (i=0;i<mEntries.size()-1;i++)
    {
        rect.SetLeft(tmpEntries[i].mPosition*(mSize.x-11)+1);
        rect.SetRight(tmpEntries[i+1].mPosition*(mSize.x-11)+1);

        wxColor col1(tmpEntries[i].mColor.a*255, tmpEntries[i].mColor.a*255, tmpEntries[i].mColor.a*255);
        wxColor col2(tmpEntries[i+1].mColor.a*255, tmpEntries[i+1].mColor.a*255, tmpEntries[i+1].mColor.a*255);
        memDC.GradientFillLinear(rect, col1, col2);
    }
    // build animation gradient

    float asysLifeTime = 1;
    if (mLifeTime)
        asysLifeTime = *mLifeTime;
    if (mAnimColor)
    {
        mAnimColor->Free();
        for (i=0;i<mEntries.size();i++)
        {
            tcolor tmpC = tmpEntries[i].mColor;
            //tmpC.SwapRB();
            mAnimColor->AddKey(tmpEntries[i].mPosition * asysLifeTime * 1000.0f,
                tmpC);
        }

    }
    if (mAnimSize)
    {
        mAnimSize->Free();
        for (i=0;i<mEntries.size();i++)
        {
            mAnimSize->AddKey(tmpEntries[i].mPosition * asysLifeTime * 1000.0f,
                tmpEntries[i].mSize);
        }
    }
	if (mAnimRot)
	{
		mAnimRot->Free();
		for (i=0;i<mEntries.size();i++)
		{
			mAnimRot->AddKey(tmpEntries[i].mPosition * asysLifeTime * 1000.0f,
				tmpEntries[i].mRot);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int wxGradientControl::GradientSortFunc(const void *a1, const void *a2)
{
    GradientEntry *e1 = (GradientEntry*)a1;
    GradientEntry *e2 = (GradientEntry*)a2;
    if (e1->mPosition< e2->mPosition) return -1;
    if (e1->mPosition> e2->mPosition) return 1;
    return 0;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::AddEntry(const GradientEntry& entry)
{
    for (uint i=0;i<mEntries.size();i++)
    {
        if (mEntries[i].mPosition == entry.mPosition)
        {
            mEntries[i] = entry;
            return;
        }
   }

    mEntries.push_back(entry);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnRButtonDown(wxMouseEvent& event)
{
    if ( (mpSelected) && (mpSelected != &mEntries[0]) && (mpSelected != &mEntries[1]) )
    {
		for (tarray<GradientEntry>::iterator it = mEntries.begin() + 2; it < mEntries.end(); ++it)
		{
			if (it->mPosition == mpSelected->mPosition
				&& it->mColor == mpSelected->mColor
				&& it->mSize == mpSelected->mSize)
			{
				mEntries.erase(it);
				break;
			}
		}
        mpSelected = NULL;
        mHandler->HandleGradientSelChanged(this);
        BuildGradient();
        Refresh();
        return;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnLButtonDown(wxMouseEvent& event)
{
    event.Skip();

    for (uint i=0;i<mEntries.size();i++)
    {
        int pos = mEntries[i].mPosition*(mSize.x-9);
        if ( (event.m_x>=(pos-4)) &&
            (event.m_x<=(pos+4)) &&
            (event.m_y>=0) &&
            (event.m_y<=12)
			)
        {
            mpSelected = &mEntries[i];
            mHandler->HandleGradientSelChanged(this);

            GradientEntry aInterpolated;
            GetInterpolation(mpSelected->mPosition, aInterpolated, mPrevLock, mNextLock);

            mbCanMove = true;
            mX = event.m_x;
            mY = event.m_y;
            if ((mEntries[i].mPosition>0.f)&& (mEntries[i].mPosition<1.f) )
                CaptureMouse();
            else
				mbCanMove = false;


			if (mbCanMove)
			{
				LOG("Mouse captured\n");
			}
            Refresh();
            //Update();
            return;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnLButtonUp(wxMouseEvent& event)
{
    if (mbCanMove)
    {
		LOG("Mouse released\n");
        ReleaseMouse();
        mbCanMove = false;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnMouseMotion(wxMouseEvent& event)
{
    if (mbCanMove)
    {
        int dx = event.m_x-mX;
        float newPos = mpSelected->mPosition + (float)dx/(float)(mSize.x-9);
        if ((newPos > mPrevLock) &&(newPos< mNextLock))
        {
            mpSelected->mPosition = newPos;
			LOG("%5.2f\n", mpSelected->mPosition);
            if (mpSelected->mPosition<0.f) mpSelected->mPosition = 0.f;
            if (mpSelected->mPosition>1.f) mpSelected->mPosition = 1.f;

            mX = event.m_x;
            BuildGradient();
            Refresh();
            //Update();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnLDblClick(wxMouseEvent& event)
{
    if ((event.m_x>4)&&(event.m_x<(mSize.x-8))&&(event.m_y>4))
    {
        float nPos = (float)(event.m_x-4)/(float)(mSize.x-9);
        GradientEntry aInterpolated;
        GetInterpolation(nPos, aInterpolated, mPrevLock, mNextLock);
        AddEntry(aInterpolated);
        mpSelected = &mEntries[mEntries.size()-1];
        mHandler->HandleGradientSelChanged(this);
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::GetInterpolation(float nPos, GradientEntry& entry, float &fPrev, float &fNext)
{
    GradientEntry justBefore = mEntries[0], justAfter = mEntries[1];
    for (uint i = 2 ; i < mEntries.size() ; i++)
    {
        if ((mEntries[i].mPosition > justBefore.mPosition) && (mEntries[i].mPosition<nPos))
            justBefore = mEntries[i];
        if ((mEntries[i].mPosition < justAfter.mPosition) && (mEntries[i].mPosition>nPos))
            justAfter = mEntries[i];
    }
    // interpolation
    float interpFactor = (nPos-justBefore.mPosition)/(justAfter.mPosition-justBefore.mPosition);
    entry.mPosition = nPos;
    entry.mSize = (justAfter.mSize-justBefore.mSize)*interpFactor + justBefore.mSize;
    entry.mColor.Lerp(justBefore.mColor, justAfter.mColor, interpFactor);
	entry.mRot = LERP(justBefore.mRot, justAfter.mRot, interpFactor);

    float pixelDecal = 1.f/(float)(mSize.x-9);
    mPrevLock = justBefore.mPosition + pixelDecal;
    mNextLock = justAfter.mPosition - pixelDecal;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnKeyDown(wxKeyEvent& event)
{
    if (event.GetKeyCode() == WXK_DELETE)
    {
        if ( (mpSelected) && (mpSelected != &mEntries[0]) && (mpSelected != &mEntries[1]) )
        {
			for (tarray<GradientEntry>::iterator it = mEntries.begin() + 2; it < mEntries.end(); ++it)
			{
				if (it->mPosition == mpSelected->mPosition
					&& it->mColor == mpSelected->mColor
					&& it->mSize == mpSelected->mSize)
				{
					mEntries.erase(it);
					break;
				}
			}
            mpSelected = NULL;
            mHandler->HandleGradientSelChanged(this);
            BuildGradient();
            Refresh();
            return;
        }
        return;
    }
    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::SetSelectedColor(const tcolor & aCol)
{
    if (mpSelected)
    {

        mpSelected->mColor.r = aCol.b;
        mpSelected->mColor.g = aCol.g;
        mpSelected->mColor.b = aCol.r;

		//mpSelected->mColor = aCol;

        BuildGradient();
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::SetSelectedAlpha(const tcolor & aAlpha)
{
    if (mpSelected)
    {
        mpSelected->mColor.a = aAlpha;
        BuildGradient();
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::SetSelectedSize(float aSize)
{
    if (mpSelected)
    {
        mpSelected->mSize = aSize;
        BuildGradient();
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::SetSelectedRot(float aRot)
{
    if (mpSelected)
    {
        mpSelected->mRot = aRot;
        BuildGradient();
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::OnLostCapture(wxMouseCaptureLostEvent& event)
{

	event.Skip();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxGradientControl::SetSelected(const tcolor & aCol, float aSize, float aRot)
{
    if (mpSelected)
    {

        mpSelected->mColor.r = aCol.b;
        mpSelected->mColor.g = aCol.g;
        mpSelected->mColor.b = aCol.r;
		mpSelected->mColor.a = aCol.a;

		mpSelected->mSize = aSize;
		mpSelected->mRot = aRot;

		//mpSelected->mColor = aCol;

        BuildGradient();
        Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

