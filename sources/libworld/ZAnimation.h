///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZAnimation.h
// Creation : 18/08/2001
// Author : Cedric Guillemet
// Description :
// Reliability : 5/10 
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZAnimation_H
#define ZAnimation_H

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "ZAnimationTrack.h"

#include "../libbase/ZSerializator.h"

enum ANIM_CHUNKS
{
	ANIM_TRANSLATION = 1,
	ANIM_ROTATION = 1<<1,
	ANIM_SCALE = 1<<2,
};

DECLAREZCLASS(ZAnimation);

// Class Import ///////////////////////////////////////////////////////////////////////////////////

template <class type> class ZKeyFrame;
class ZNode;

// Constants //////////////////////////////////////////////////////////////////////////////////////

// Globals ////////////////////////////////////////////////////////////////////////////////////////

// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZAnimation : public ZBaseClass //IAnimation
{
public:
    DECLARE_SERIALIZABLE
public:
    // Animation Data
    tstring                                 m_Name; // name of node to apply 
    tarray<ZAnimationTrackInternal*>     m_AnimTrackList;

public:
    // Initialisation 
    ZAnimation() { mType = 0; mLength = 0;}
    //void Init(tulong p_playtype = ZANIMATION_PLAY_TYPE_LOOP, float speed = 1.0f, tulong priority = 5);
    //void Free();

	virtual void SetType(unsigned int type) { mType = type; }
	virtual unsigned int GetType() { return mType; }
    ZAnimationTrackInternal * AddAnimationTrack(tstring name, ZANIMATION_KEY_TYPE keyType = ZANIMATION_KEY_TYPE_NONE, ZANIMATION_INTERP_TYPE interpType = ZANIMATION_INTERP_LINEAR); 

    void AddAnimationTrack(ZAnimationTrack<float> * pTrack, ZANIMATION_INTERP_TYPE interpType = ZANIMATION_INTERP_LINEAR); 
    void AddAnimationTrack(ZAnimationTrack<tlong> * pTrack, ZANIMATION_INTERP_TYPE interpType = ZANIMATION_INTERP_LINEAR); 
    void AddAnimationTrack(ZAnimationTrack<tvector3> * pTrack, ZANIMATION_INTERP_TYPE interpType = ZANIMATION_INTERP_LINEAR);
    void AddAnimationTrack(ZAnimationTrack<tcolor> * pTrack, ZANIMATION_INTERP_TYPE interpType = ZANIMATION_INTERP_LINEAR); 
    void AddAnimationTrack(ZAnimationTrack<tquaternion> * pTrack, ZANIMATION_INTERP_TYPE interpType = ZANIMATION_INTERP_LINEAR); 
    //void AddAnimationTrack(ZAnimationTrack<bool> * pTrack, ZANIMATION_INTERP_TYPE interpType = ZANIMATION_INTERP_LINEAR); 

    // Key Frames operations
    void AddKeyFrame(tulong trackIdx, float time, float value, float paramIn = 0, float paramOut = 0);
    void AddKeyFrame(tulong trackIdx, float time, tlong value, tlong paramIn = 0, tlong paramOut = 0 );
    void AddKeyFrame(tulong trackIdx, float time, tshort value, tshort paramIn = 0, tshort paramOut = 0);
    void AddKeyFrame(tulong trackIdx, float time, tcolor value, tcolor paramIn = tcolor(0,0,0,0), tcolor paramOut = tcolor(0,0,0,0));
    void AddKeyFrame(tulong trackIdx, float time, tvector3 value, tvector3 paramIn = tvector3(0,0,0), tvector3 paramOut = tvector3(0,0,0));
    void AddKeyFrame(tulong trackIdx, float time, tquaternion value, tquaternion paramIn = tquaternion(0,0,0,0), tquaternion paramOut = tquaternion(0,0,0,0));
    //void AddKeyFrame(tulong trackIdx, float time, bool value, bool paramIn = false, bool paramOut = false);

    //        void ComputeBezierTangent(tulong dimension);
    void RebuildReferences(ZAnimation * animation);

    tulong Optimize();

    void ComputeLength();
    void Load(ZFile & file);
    void Save(ZFile & file);
	virtual float GetLength()
	{
		return mLength;
		/*
		float alen=0;
        for (uint i=0;i<m_AnimTrackList.size();i++)
        {
			alen = (m_AnimTrackList[i]->m_Length>alen)?m_AnimTrackList[i]->m_Length:alen;
		}
		return alen;
		*/

	}

    void RemoveKey(float time) 
    {
        for (unsigned int i=0;i<m_AnimTrackList.size();i++)
        {
            switch (m_AnimTrackList[i]->m_KeyFrameType)
            {
                case ZANIMATION_KEY_TYPE_VECTOR3:  
                    ((ZAnimationTrack<tvector3>*)m_AnimTrackList[i])->RemoveKey(time);
                    break;
                case ZANIMATION_KEY_TYPE_QUATERNION:  
                    ((ZAnimationTrack<tquaternion>*)m_AnimTrackList[i])->RemoveKey(time);
                    break;
				default:
					break;
            }
        }
    }
    void ResampleKeys(float timescale) 
    {
        for (unsigned int i=0;i<m_AnimTrackList.size();i++)
        {
            switch (m_AnimTrackList[i]->m_KeyFrameType)
            {
                case ZANIMATION_KEY_TYPE_VECTOR3:  
                    ((ZAnimationTrack<tvector3>*)m_AnimTrackList[i])->ResampleKeys(timescale);
                    break;
                case ZANIMATION_KEY_TYPE_QUATERNION:  
                    ((ZAnimationTrack<tquaternion>*)m_AnimTrackList[i])->ResampleKeys(timescale);
                    break;
				default:
					break;

            }
        }
    }

    void RemoveKey(tulong trackIdx, float time) 
    {
        switch (m_AnimTrackList[trackIdx]->m_KeyFrameType)
        {
            case ZANIMATION_KEY_TYPE_VECTOR3:  
                ((ZAnimationTrack<tvector3>*)m_AnimTrackList[trackIdx])->RemoveKey(time);
                break;
            case ZANIMATION_KEY_TYPE_QUATERNION:  
                ((ZAnimationTrack<tquaternion>*)m_AnimTrackList[trackIdx])->RemoveKey(time);
                break;
			default:
				break;

        }

    }

    void InsertKey(tulong trackIdx, float time, const tvector3& keyValue)
    {
        ((ZAnimationTrack<tvector3>*)m_AnimTrackList[trackIdx])->InsertKey(time, keyValue);
    }

    void InsertKey(tulong trackIdx, float time, const tquaternion& keyValue)
    {
        ((ZAnimationTrack<tquaternion>*)m_AnimTrackList[trackIdx])->InsertKey(time, keyValue);
    }

	unsigned int mType;
	float mLength;

	virtual tulong GetMemoryUsed() const 
	{
		tulong sz = sizeof(ZAnimation);
        for (unsigned int i=0;i<m_AnimTrackList.size();i++)
        {
			sz += m_AnimTrackList[i]->GetMemoryUsed();
		}

		return sz;
	}
};

/*
class ZAnimationInstance : public ZAnimationState
{
public:
    // Animation Data
    ZAnimation*                          m_Animation;                           
    tarray<void *>                         m_AnimDestData;               
    tarray<tulong>                          m_CurrentKey;
    ZNode*                               m_Node;

    bool                                   valid;
    bool                                   validNode;
    bool                                   instanciedRessources;


    tvector3            stTranslation;
    tvector3            stScale;
    tquaternion         stRotation;
    //tquaternion         compScaleRotation;
    //float              compDeterminantSign;
    //tvector3            compScale;
    bool                stVisibility;

public:
    // Initialisation 
    ZAnimationInstance(ZAnimation *pAnimation, ZNode *pNode) : ZAnimationState()
    {
        m_Animation             = pAnimation;
        valid                   = true;
        validNode               = true;
        instanciedRessources    = false;
        m_Node                  = pNode;

        m_Length                = m_Animation->m_AnimTrackList[0]->m_Length;

        tulong nbTracks = (tulong)m_Animation->m_AnimTrackList.size();
        m_CurrentKey.resize(nbTracks);
        m_AnimDestData.resize(nbTracks);
        for (tulong i = 0 ; i < nbTracks ; i++)
        {
            // key
            m_CurrentKey[i] = 0;
            // dest value
            switch (m_Animation->m_AnimTrackList[i]->m_TrackType)
            {
            case TRACK_TYPE_TRANSLATION:    m_AnimDestData[i] = &stTranslation; break;
            case TRACK_TYPE_ROTATION:       m_AnimDestData[i] = &stRotation;    break;
            case TRACK_TYPE_SCALE:          m_AnimDestData[i] = &stScale;       break;
            case TRACK_TYPE_VISIBILITY:     m_AnimDestData[i] = &stVisibility;  break;

            }

        }
    }

    void Init(tulong p_playtype = ZANIMATION_PLAY_TYPE_LOOP, float speed = 1.0f, tulong priority = 5);

    // Update
    void GoToTime(float time);
    void Update(float elapsed);

    void ComputeLength() { return m_Animation->ComputeLength(); }
};
*/
// Inlines ////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
