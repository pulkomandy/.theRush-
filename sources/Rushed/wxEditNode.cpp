///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : wxZEditNode.cpp
// Creation : 08/11/2006
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

#include <wx/wx.h>
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

#include "wxNavigator.h"
*/
#include <wx/aui/aui.h>

#include "../libbase/LibBase.h"
#include "wxEditNode.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

enum
{
    PARTCTRL_COLOR = 101,
    PARTCTRL_SIZE,
    PARTCTRL_ALPHA,
    PARTCTRL_MOTION,
    PARTCTRL_DISPLAY,
    PARTCTRL_GENERATOR,
    TERRAIN_TERRAFORM,
    TERRAIN_PAINT,
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void wxZEditNode::SetSelectedNode(PSM_Node *aNode)
{
    if (mNode == aNode) return;
    mNode = aNode;

    mCurrentGradientControl = NULL;

    BeginPanel();
    if (aNode)
    {
        switch (aNode->type)
        {
        case PSM_NODE_TYPE_OBJECT:
            EditClass(PSM_Node::mInfosSerializableFields, PSM_Node::mInfosSerializableName, aNode);
            break;

        case PSM_NODE_TYPE_CAMERA:
            EditClass(PSM_Node::mInfosSerializableFields, PSM_Node::mInfosSerializableName, aNode);
            EditClass(PSM_Camera::mInfosSerializableFields, PSM_Camera::mInfosSerializableName, aNode);
            break;
        case PSM_NODE_TYPE_PARTICLE_INSTANCE:
            {

                PSM_ParticleSystem *pSys = ((PSM_ParticleInstance *)aNode)->GetParticleSystem();
                EditClass(PSM_Node::mInfosSerializableFields, PSM_Node::mInfosSerializableName, aNode);

                // Particle systeme
                EditClass(pSys->getInfosFields(), pSys->GetClassName(), pSys);

                tmpGenerator = pSys->generator->typePartGen;
                tmpMotion = pSys->motion->type;
                tmpDisplay = pSys->mParticleDisplay->mDisplayType;

                AddCombo("Display","QUAD|SPARK", &tmpDisplay, PARTCTRL_DISPLAY);
                AddCombo("Generator","Point|Plane|Disk|Sphere|Object", &tmpGenerator, PARTCTRL_GENERATOR);
                AddCombo("Motion","None|Physic|Spline|Direction|Point", &tmpMotion, PARTCTRL_MOTION);
                // Gradient
                AddFolder("Gradient");
                AddGradient(pSys->animDataColor, pSys->animDataSize, &pSys->lifeTime);
                AddColorPicker("Color", &tmpColor, PARTCTRL_COLOR);
                AddNumEdit<float>("Alpha", 0, 1000, &tmpAlpha, 0.01f, PARTCTRL_ALPHA);
                AddNumEdit<float>("Size", 0, 1000, &tmpSize, 0.01f, PARTCTRL_SIZE);

                // generator
                EditClass(pSys->generator->getInfosFields(), pSys->generator->GetClassName(), pSys->generator, true, true);
                // motion
                EditClass(pSys->motion->getInfosFields(), pSys->motion->GetClassName(), pSys->motion, true, true);
            }
            break;
        case PSM_NODE_TYPE_TERRAIN:
            EditClass(aNode->getInfosFields(), aNode->GetClassName(), aNode, true, true);
            AddFolder("Edit");

            AddRadios("Brush", "Box|Sphere", &mEditTerrain.mTerrainBrushType, 301);
            AddSlider("Size", 1, 20, &mEditTerrain.mTerrainBrushSize, 302);
            AddTogglePluginButton("Terraforming",  TERRAIN_TERRAFORM);

            AddSlider("Inner", 1, 50, &mEditTerrain.mTerrainInnerSize, 303);
            AddSlider("Outer", 1, 50, &mEditTerrain.mTerrainOuterSize, 304);
            AddTogglePluginButton("Painting", TERRAIN_PAINT);
            break;
        default:
            {

            }
            break;
        }
    }

    EndPanel();
}

*/
///////////////////////////////////////////////////////////////////////////////////////////////////
// handler callback

void wxZEditNode::HandleButtonClick(unsigned int aButtonID)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditNode::HandleValueChanged(unsigned int aID)
{
    // update particle gradient selected value
/*
    {
        switch (aID)
        {
        case 10: // name
            if (mNode) mNode->SetName(mNode->GetName());
            break;
        case PARTCTRL_COLOR: // color
            if (mCurrentGradientControl) mCurrentGradientControl->SetSelectedColor(tmpColor);
            break;
        case PARTCTRL_ALPHA: // alpha
            if (mCurrentGradientControl) mCurrentGradientControl->SetSelectedAlpha(tmpAlpha);
            break;
        case PARTCTRL_SIZE: // size
            if (mCurrentGradientControl) mCurrentGradientControl->SetSelectedSize(tmpSize);
            break;
        case PARTCTRL_MOTION:
            {
                PSM_ParticleSystem *pSys = ((PSM_ParticleInstance *)mNode)->GetParticleSystem();
                switch (tmpMotion)
                {
                case PSM_PARTMOTION_NONE:
                    pSys->InitNoMotion();
                    break;
                case PSM_PARTMOTION_PHYSIC:
                    pSys->InitPhysicMotion(-0.1f);
                    break;
                case PSM_PARTMOTION_SPLINE:
                    //pSys->InitSplineMotion();
                    break;
                case PSM_PARTMOTION_DIRECTION:
                    //pSys->InitDirectionMotion();
                    break;
                case PSM_PARTMOTION_FORCEPOINT:
                    pSys->InitPointForceMotion();
                    break;
                }
            }
            break;
        case PARTCTRL_GENERATOR:
            {
                PSM_ParticleSystem *pSys = ((PSM_ParticleInstance *)mNode)->GetParticleSystem();
                switch(tmpGenerator)
                {
                case PSM_PARTGEN_POINT:
                    pSys->InitPointGenerator(tvector3::zero, tvector3::zero, 0, 30, 0, 5);
                    break;
                case PSM_PARTGEN_PLANE: break;
                case PSM_PARTGEN_DISK:
                    pSys->InitDiskGenerator(tvector3::zero, 0, 30, tvector3::zero, tvector3::zero, 0, 30, 0, 5, false);
                    break;
                case PSM_PARTGEN_SPHERE:
                    pSys->InitSphereGenerator(tvector3::zero, 0, 30, tvector3::zero,  0, 30, 0, 5, false);
                    break;
                case PSM_PARTGEN_OBJECT:
                    break;
                case PSM_PARTGEN_STATIC:
                    break;
                case PSM_PARTGEN_ONDEMAND:
                    break;
                }
                AskRefresh();
            }
            break;
        case PARTCTRL_DISPLAY:
            break;
        case TERRAIN_TERRAFORM:
            gMyFrame->SetCurrentPlugin(&mEditTerrain);
            mEditTerrain.SetTypeEdit(TERRAIN_EDIT_TERRA);
            break;
        case TERRAIN_PAINT:
            gMyFrame->SetCurrentPlugin(&mEditTerrain);
            mEditTerrain.SetTypeEdit(TERRAIN_EDIT_PAINT);
            break;
        }

    }
    // change generator/motion
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditNode::HandleGradientSelChanged(wxGradientControl *pControl)
{
    // set color/alpha scale -> must update color picker/num edit
    mCurrentGradientControl = pControl;
    const GradientEntry *pGrad = pControl->GetSelectedEntry();
    if (pGrad)
    {
        tmpColor = pGrad->mColor;
        tmpSize = pGrad->mSize;
        tmpAlpha = pGrad->mColor.a;
        UpdateControlFromValue(101);
        UpdateControlFromValue(102);
        UpdateControlFromValue(103);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditNode::OnRefresh(wxEvent& event)
{
    /*
    PSM_Node *oldNode = mNode;
    mNode = NULL;
    SetSelectedNode(oldNode);
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void wxZEditNode::AskRefresh()
{
    wxSizeEvent event;
    event.SetEventObject(this);
    event.SetEventType(14000);
    AddPendingEvent(event);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

