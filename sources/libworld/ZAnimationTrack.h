///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZAnimationData.h
// Creation : 24/11/2004
// Author : Cedric Guillemet
// Description :
// Reliability : 5/10 
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZAnimationData_H
#define ZAnimationData_H

// Includes ///////////////////////////////////////////////////////////////////////////////////////
#include "../libbase/ZFile.h"
#include "ZKeyFrame.h"

// Class Import ///////////////////////////////////////////////////////////////////////////////////

// Constants //////////////////////////////////////////////////////////////////////////////////////

enum TRACK_TYPE
{
    TRACK_TYPE_UNDETERMINED,
    TRACK_TYPE_TRANSLATION,
    TRACK_TYPE_ROTATION,
    TRACK_TYPE_SCALE,
    TRACK_TYPE_VISIBILITY,
    TRACK_TYPE_MORPH,
    TRACK_TYPE_LAST = 0xFF
};

// Globals ////////////////////////////////////////////////////////////////////////////////////////

// Class //////////////////////////////////////////////////////////////////////////////////////////

class ZAnimationTrackInternal
{  
public:  
    TRACK_TYPE                         m_TrackType;
    ZANIMATION_KEY_TYPE             m_KeyFrameType;
    bool                              m_Valid;
    float                             m_Length;               
    float                             m_Offset;     

    ZANIMATION_INTERP_TYPE          m_InterpType;

    ZANIMATION_OUT_OF_RANGE_TYPE    m_OutOfRangeIn;
    ZANIMATION_OUT_OF_RANGE_TYPE    m_OutOfRangeOut;

public:  
    ZAnimationTrackInternal() 
    { 
        m_KeyFrameType  = ZANIMATION_KEY_TYPE_NONE;
        m_Valid         = true;
        m_Length        = 0;
        m_Offset        = 0;
        m_OutOfRangeIn  = ZANIMATION_OUT_OF_RANGE_NONE;
        m_OutOfRangeOut = ZANIMATION_OUT_OF_RANGE_NONE;
        m_InterpType    = ZANIMATION_INTERP_LINEAR;
        m_TrackType     = TRACK_TYPE_UNDETERMINED;
    }

    ZAnimationTrackInternal(ZAnimationTrackInternal * srcAnimationTrackType) 
    { 
        m_TrackType     = srcAnimationTrackType->m_TrackType;
        m_KeyFrameType  = srcAnimationTrackType->m_KeyFrameType;
        m_Valid         = srcAnimationTrackType->m_Valid;
        m_Length        = srcAnimationTrackType->m_Length;
        m_Offset        = srcAnimationTrackType->m_Offset;
        m_OutOfRangeIn  = srcAnimationTrackType->m_OutOfRangeIn;
        m_OutOfRangeOut = srcAnimationTrackType->m_OutOfRangeOut;
        m_InterpType    = ZANIMATION_INTERP_LINEAR;
    }

    static ZAnimationTrackInternal * GetAnimationTrack(void * srcAnimationTrack);
    static ZAnimationTrackInternal * Create(ZFile & file);
    static ZAnimationTrackInternal * Create(ZANIMATION_KEY_TYPE keyType);
    float GetAnimationTrackLength();
    tulong GetKeyFrameCount();
    void Interpolate(void * res, float time, tlong &curKey);
    void Save(ZFile &file);  
    tulong Optimize();
	virtual tulong GetMemoryUsed() const { return 0; }
    
};

///////////////////////////////////////////////////////////////////////////////////////////////////

template <class type> class ZAnimationTrack : public ZAnimationTrackInternal
{
public:
    tarray<ZKeyFrame<type> >    m_KeyFrameList;

public:
    // Initialisation 
    ZAnimationTrack() : ZAnimationTrackInternal()
    {
    }

    ZAnimationTrack(ZAnimationTrack * srcAnimationTrack) : ZAnimationTrackInternal( (ZAnimationTrackInternal*)srcAnimationTrack )
    {
        m_KeyFrameList.resize(srcAnimationTrack->m_KeyFrameList.size());  
        //m_KeyFrameList.SetCount(srcAnimationTrack->m_KeyFrameList.size());  
        memcpy(&m_KeyFrameList[0], &srcAnimationTrack->m_KeyFrameList[0], m_KeyFrameList.size() * sizeof(ZKeyFrame<type>));
    }

    void Init(TRACK_TYPE trackType, ZANIMATION_KEY_TYPE keyFramesType)
    {
        m_TrackType            = trackType;
        m_KeyFrameType = keyFramesType;
    }

    void AddKey(float time, type  keyValue)
    {
        m_KeyFrameList.push_back(ZKeyFrame<type>(time, keyValue));
    }

    void AddKey(ZKeyFrame<type> * keyframe)
    {
        m_KeyFrameList.push_back(*keyframe);
    }

    void ResampleKeys(float timescale) 
    {
		for (unsigned int i=0;i<m_KeyFrameList.size();i++)
        {
            m_KeyFrameList[i].m_Time = i*timescale;
        }
    }

    void RemoveKey(float time)
    {
        for (unsigned int i=0;i<m_KeyFrameList.size();i++)
        {
            if (m_KeyFrameList[i].m_Time == time)
            {
				int okl = (int)m_KeyFrameList.size();
				for (unsigned int j=i;j<(m_KeyFrameList.size()-1);j++)
				{
					m_KeyFrameList[j] = m_KeyFrameList[j+1];
				}

                m_KeyFrameList.resize(okl-1);
                return;
            }
        }
    }

    int InsertKey(float time,type  keyValue)
    {
        if (time <= m_KeyFrameList[0].m_Time)
        {
            m_KeyFrameList.resize(m_KeyFrameList.size()+1);
            m_KeyFrameList[0].m_Time = time;
            m_KeyFrameList[0].m_Value = keyValue;
            return 0;
        }
        if (time >= m_KeyFrameList[m_KeyFrameList.size()-1].m_Time)
        {
            AddKey(time, keyValue);
            return (int)m_KeyFrameList.size()-1;
        }

        for (unsigned int i=0;i<m_KeyFrameList.size()-1;i++)
        {
            if ( (time >= m_KeyFrameList[i].m_Time ) && (time <= m_KeyFrameList[i+1].m_Time ) )
            {
                int sz = (int)m_KeyFrameList.size();
                m_KeyFrameList.resize(sz+1);
                for (unsigned int j=sz;j>(i+1);j--)
                {
                    m_KeyFrameList[j].m_Time = m_KeyFrameList[j-1].m_Time;
                    m_KeyFrameList[j].m_Value = m_KeyFrameList[j-1].m_Value;
                }

                m_KeyFrameList[i+1].m_Value = keyValue;
                m_KeyFrameList[i+1].m_Time = time;
                return (i+1);
            }
        }
		return -1;
    }

    void UpdateLenghtAndOffset()
    {
        m_Length = m_KeyFrameList[m_KeyFrameList.size()-1].m_Time;
        m_Offset = m_KeyFrameList[0].m_Time;
    }

    void Free()
    {
        this->m_KeyFrameList.clear();
    }

    // Interpolate Function
    void Interpolate(type * res, float time, tlong &curKeyID)
    {
        ZKeyFrame<type> *    curKey;
        ZKeyFrame<type> *    nextKey;
        float            ratio;
        bool                found;

        //if(m_Valid)
        {
            if(GetKeyFrameCount() > 0)
            {
                // Adjust time with param in and out influence
                if(time > m_Length + m_Offset)
                {
                    switch(m_OutOfRangeOut)
                    {
                    case ZANIMATION_OUT_OF_RANGE_CONSTANT:
                        //                                time = m_CurrentTime;
                        break;

                    case ZANIMATION_OUT_OF_RANGE_CYCLE:
                        time = (float)fabs(fmod(m_Offset - time, m_Length)) + m_Offset;    
                        break;
                    }
                }
                else
                {
                    if(time < m_Offset)
                    {
                        switch(m_OutOfRangeOut)
                        {
                        case ZANIMATION_OUT_OF_RANGE_CONSTANT:
                            //                                    time = m_CurrentTime;
                            break;

                        case ZANIMATION_OUT_OF_RANGE_CYCLE:
                            time = m_Length - (float)fabs(fmod(m_Offset - time, m_Length)) + m_Offset;    
                            break;
                        }
                    }
                    /*                        else
                    {
                    time = m_CurrentTime;
                    }
                    */                    }            

                    if(m_KeyFrameList.size()>0)
                    {
                        if(m_KeyFrameList[0].m_Time >= time)
                        {
                            *res = m_KeyFrameList[0].m_Value;
                        }
                        else
                        {
                            if(m_KeyFrameList[m_KeyFrameList.size()-1].m_Time <= time)
                            {
                                *res = m_KeyFrameList[m_KeyFrameList.size()-1].m_Value;
                            }
                            else
                            {
                                found = false;
                                tlong j= 0;

                                while((j < (tlong)m_KeyFrameList.size()-1) && ! found)
                                {
                                    curKeyID %= (tlong)m_KeyFrameList.size() - 1;

                                    curKey    = &m_KeyFrameList[curKeyID];
                                    nextKey    = &m_KeyFrameList[curKeyID+1];

                                    if( (curKey->m_Time < time) && ( nextKey->m_Time >= time) )
                                    {
                                        // Updtate current key for speed up animation    
                                        ratio = (time - curKey->m_Time) / (nextKey->m_Time - curKey->m_Time);
                                        found = true;
                                        if(m_InterpType == ZANIMATION_INTERP_LINEAR)
                                        {
                                            // Interpolate between Keyframes
                                            curKey->InterpolateLinear( res, *nextKey, ratio);
                                        }
                                        else if (m_InterpType == ZANIMATION_INTERP_HERMITE)
                                        {
                                            type pnextp1;
                                            type pprev;

                                            if ((curKeyID+2)>=(tlong)m_KeyFrameList.size())
                                            {
                                                pnextp1 = (type)curKey->m_Value + (type)(nextKey->m_Value-curKey->m_Value);
                                            }
                                            else
                                            {
                                                pnextp1 = m_KeyFrameList[curKeyID+2].m_Value;
                                            }

                                            if (curKeyID == 0)
                                            {
                                                pprev = (type)curKey->m_Value - (type)(nextKey->m_Value-curKey->m_Value);
                                            }
                                            else
                                            {
                                                pprev = m_KeyFrameList[curKeyID-1].m_Value;
                                            }
                                            curKey->InterpolateHermite(res, *nextKey, pnextp1 , pprev , ratio);
                                        }
                                        else
                                        {
                                            curKey->InterpolateFlat( res );
                                        }
                                    }
                                    j++;
                                    curKeyID++;
                                }
                            }
                        }
                    }
            }
        }

        //return curKeyID;
    }

    //        void ComputeBezierTangent();

    void Save(ZFile &file)  
    {  
        file << m_KeyFrameType;  
        file << m_TrackType;
        file << m_Valid;
		unsigned char interptype = m_InterpType;
		file << interptype;
        if(m_Valid)
        {
            int nbEnts = (int)m_KeyFrameList.size();  
            file << nbEnts;  
            file.Write(&m_KeyFrameList[0], sizeof(ZKeyFrame<type>)*nbEnts);  
        }
    }  

    // pour cette methode, on connait deja le type  
    void Load(ZFile &file)  
    {  
        file >> m_TrackType;
        file >> m_Valid;
		unsigned char interptype;
		file >> interptype;
		m_InterpType = (ZANIMATION_INTERP_TYPE)interptype;
        if(m_Valid)
        {
            int nbEnts;  
            file >> nbEnts;  
            m_KeyFrameList.resize(nbEnts);  
            //m_KeyFrameList.SetCount(nbEnts);  
            file.Read(&m_KeyFrameList[0], sizeof(ZKeyFrame<type>)*nbEnts);
        }
        UpdateLenghtAndOffset();
    }  

	virtual tulong GetMemoryUsed() const 
	{
		tulong sz = sizeof (ZAnimationTrackInternal);
		sz += sizeof (ZKeyFrame<type>) * m_KeyFrameList.size();
		return sz;
	}
};

// Inlines ////////////////////////////////////////////////////////////////////////////////////////


#endif
