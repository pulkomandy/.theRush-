///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZAnimation.cpp
// Creation : 18/08/2001
// Author : Cedric Guillemet
// Description : 
///////////////////////////////////////////////////////////////////////////////////////////////////

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "ZAnimation.h"

#include "..\libbase\ZSerializator.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_SERIALIZATION(ZAnimation)
END_SERIALIZATION()


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ZAnimationInstance::Init(tulong playtype, float speed, tulong priority)
{
    m_PlayType    = playtype;
    m_Speed        = speed;
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

ZAnimationTrackInternal * ZAnimation::AddAnimationTrack(tstring /*name*/, ZANIMATION_KEY_TYPE keyType, ZANIMATION_INTERP_TYPE interpType)
{
    ZAnimationTrackInternal * animTrack;

    animTrack = ZAnimationTrackInternal::Create(keyType);

    switch(keyType)
    {
    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            AddAnimationTrack((ZAnimationTrack<float>*)animTrack, interpType);
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:
        {
            AddAnimationTrack((ZAnimationTrack<tlong>*)animTrack, interpType);
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            AddAnimationTrack((ZAnimationTrack<tvector3>*)animTrack, interpType);
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            AddAnimationTrack((ZAnimationTrack<tcolor>*)animTrack, interpType);
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            AddAnimationTrack((ZAnimationTrack<tquaternion>*)animTrack, interpType);
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            AddAnimationTrack((ZAnimationTrack<bool>*)animTrack, interpType);
            break;
        }
*/
    default:
        {
            return NULL;
            break;
        }
    }


    return animTrack;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddAnimationTrack(ZAnimationTrack<float> * pTrack, ZANIMATION_INTERP_TYPE interpType) 
{ 
    if(pTrack)
    {
        pTrack->m_KeyFrameType = ZANIMATION_KEY_TYPE_FLOAT;
        pTrack->m_InterpType = interpType;
    }

    m_AnimTrackList.push_back(pTrack); 
    ComputeLength();
}  

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddAnimationTrack(ZAnimationTrack<tlong> * pTrack, ZANIMATION_INTERP_TYPE interpType) 
{ 
    if(pTrack)
    {
        pTrack->m_KeyFrameType = ZANIMATION_KEY_TYPE_LONG;
        pTrack->m_InterpType = interpType;
    }

    m_AnimTrackList.push_back(pTrack); 
    ComputeLength();
}  

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddAnimationTrack(ZAnimationTrack<tvector3> * pTrack, ZANIMATION_INTERP_TYPE interpType) 
{ 
    if(pTrack)
    {
        pTrack->m_KeyFrameType = ZANIMATION_KEY_TYPE_VECTOR3;
        pTrack->m_InterpType = interpType;
    }

    m_AnimTrackList.push_back(pTrack); 
    ComputeLength();
}  

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddAnimationTrack(ZAnimationTrack<tcolor> * pTrack, ZANIMATION_INTERP_TYPE interpType) 
{ 
    if(pTrack)
    {
        pTrack->m_KeyFrameType = ZANIMATION_KEY_TYPE_COLOR;
        pTrack->m_InterpType = interpType;
    }

    m_AnimTrackList.push_back(pTrack); 
    ComputeLength();
}  

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddAnimationTrack(ZAnimationTrack<tquaternion> * pTrack, ZANIMATION_INTERP_TYPE interpType) 
{ 
    if(pTrack)
    {
        pTrack->m_KeyFrameType = ZANIMATION_KEY_TYPE_QUATERNION;
        pTrack->m_InterpType = interpType;
    }

    m_AnimTrackList.push_back(pTrack); 
    ComputeLength();
}  

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ZAnimation::AddAnimationTrack(ZAnimationTrack<bool> * pTrack, ZANIMATION_INTERP_TYPE interpType) 
{ 
    if(pTrack)
    {
        pTrack->m_KeyFrameType = ZANIMATION_KEY_TYPE_BOOL;
        pTrack->m_InterpType = interpType;
    }

    m_AnimTrackList.push_back(pTrack); 
    ComputeLength();
}  
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddKeyFrame(tulong trackID, float time, float value, float paramIn, float paramOut)
{
    if(m_AnimTrackList[trackID]->m_KeyFrameType == ZANIMATION_KEY_TYPE_FLOAT)
    {
        ((ZAnimationTrack<float> *)m_AnimTrackList[trackID])->AddKey(time, value);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddKeyFrame(tulong trackID, float time, tlong value, tlong paramIn, tlong paramOut)
{
    if(m_AnimTrackList[trackID]->m_KeyFrameType == ZANIMATION_KEY_TYPE_LONG)
    {
        ((ZAnimationTrack<tlong> *)m_AnimTrackList[trackID])->AddKey(time, value);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddKeyFrame(tulong trackID, float time, tcolor value, tcolor paramIn, tcolor paramOut)
{
    if(m_AnimTrackList[trackID]->m_KeyFrameType == ZANIMATION_KEY_TYPE_COLOR)
    {
        ((ZAnimationTrack<tcolor> *)m_AnimTrackList[trackID])->AddKey(time, value);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddKeyFrame(tulong trackID, float time, tvector3 value, tvector3 paramIn, tvector3 paramOut)
{
    if(m_AnimTrackList[trackID]->m_KeyFrameType == ZANIMATION_KEY_TYPE_VECTOR3)
    {
        ((ZAnimationTrack<tvector3> *)m_AnimTrackList[trackID])->AddKey(time, value);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::AddKeyFrame(tulong trackID, float time, tquaternion value, tquaternion paramIn, tquaternion paramOut)
{
    if(m_AnimTrackList[trackID]->m_KeyFrameType == ZANIMATION_KEY_TYPE_QUATERNION)
    {
        ((ZAnimationTrack<tquaternion> *)m_AnimTrackList[trackID])->AddKey(time, value);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ZAnimation::AddKeyFrame(tulong trackID, float time, bool value, bool paramIn, bool paramOut)
{
    if(m_AnimTrackList[trackID]->m_KeyFrameType == ZANIMATION_KEY_TYPE_BOOL)
    {
        ((ZAnimationTrack<bool> *)m_AnimTrackList[trackID])->AddKey(time, value);
    }
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ZAnimationInstance::Update(float elapsed)
{
    ZAnimationState::Update(elapsed);

    for(tulong i=0; i<m_Animation->m_AnimTrackList.size(); i++)
    {
        m_Animation->m_AnimTrackList[i]->Interpolate(m_AnimDestData[i], m_CurrentTime, (tlong&)m_CurrentKey[i]);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimationInstance::GoToTime(float time)
{
    for(tulong i=0; i<m_Animation->m_AnimTrackList.size(); i++)
    {
        m_Animation->m_AnimTrackList[i]->Interpolate(m_AnimDestData[i], time, (tlong&)m_CurrentKey[i]);
    }
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::ComputeLength()
{

	mLength = 0.0f;

	for(tulong i=0; i<m_AnimTrackList.size(); i++)
	{
		float keyTrackLength = m_AnimTrackList[i]->GetAnimationTrackLength();
		
		if(keyTrackLength > mLength)
		{
			mLength = keyTrackLength;
		}
		
	}
	//m_NextStep = m_Length;

}

///////////////////////////////////////////////////////////////////////////////////////////////////

tulong ZAnimation::Optimize()
{
    tulong keyCount=0;

    for(tulong i=0; i<m_AnimTrackList.size(); i++)
    {
        keyCount += m_AnimTrackList[i]->Optimize();
    }
    return keyCount;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
void ZAnimation::RebuildReferences(ZAnimation * animation)
{
for(tulong i=0; i<m_AnimTrackList.GetCount(); i++)
{
for(tulong j=0; j<animation->m_AnimTrackList.GetCount(); j++)
{
if(animation->m_AnimTrackList[j]->m_Name == m_AnimTrackList[i]->m_Name)
{
m_AnimDestData.push_back( animation->m_AnimDestData[j] );
}
}
}
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::Load(ZFile & file)
{
    m_Name.LoadFile(file);
 
	unsigned int checkType;
	tulong nbTrack;
	file >> checkType;
	if (checkType >= 0xFFFF0000)
	{
		file >> mType;
		file >> mLength;
		file.Read(&nbTrack, sizeof(nbTrack));
	}
	else
	{
		nbTrack = checkType;
	}


    
    

    //m_AnimTrackList.SetIncr(nbTrack);
    m_AnimTrackList.resize(nbTrack);

    ZAnimationTrackInternal * animTrack;
    for(tulong i=0 ; i<nbTrack ; i++)
    { 
        animTrack = ZAnimationTrackInternal::Create(file);
        m_AnimTrackList[i] = animTrack;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimation::Save(ZFile & file)
{
    m_Name.SaveFile(file);

	unsigned int checkType = 0xFFFF0000;
	file << checkType;

	file << mType;
	file << mLength;

    tulong nbTrack = (tulong)m_AnimTrackList.size();
    file.Write(&nbTrack, sizeof(nbTrack));

    for(tulong i=0; i<m_AnimTrackList.size(); i++)
    {
        m_AnimTrackList[i]->Save(file);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
