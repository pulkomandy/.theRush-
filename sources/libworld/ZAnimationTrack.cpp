///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZAnimationData.h
// Creation : 24/11/2004
// Author : Cedric Guillemet
// Description :
// Reliability : 5/10 
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ZAnimationTrack.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

ZAnimationTrackInternal * ZAnimationTrackInternal::GetAnimationTrack(void * srcAnimationTrack)
{
    switch(((ZAnimationTrackInternal*)srcAnimationTrack)->m_KeyFrameType)
    {
    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            ZAnimationTrack<float> * newKeyFrameTrack = new ZAnimationTrack<float>((ZAnimationTrack<float>*)srcAnimationTrack);
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:       
        {
            ZAnimationTrack<tlong> * newKeyFrameTrack = new ZAnimationTrack<tlong>((ZAnimationTrack<tlong>*)srcAnimationTrack);
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            ZAnimationTrack<tvector3> * newKeyFrameTrack = new ZAnimationTrack<tvector3>((ZAnimationTrack<tvector3>*)srcAnimationTrack);
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            ZAnimationTrack<tcolor> * newKeyFrameTrack = new ZAnimationTrack<tcolor>((ZAnimationTrack<tcolor>*)srcAnimationTrack);
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            ZAnimationTrack<tquaternion> * newKeyFrameTrack = new ZAnimationTrack<tquaternion>((ZAnimationTrack<tquaternion>*)srcAnimationTrack);
            return newKeyFrameTrack;
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            ZAnimationTrack<bool> * newKeyFrameTrack = new ZAnimationTrack<bool>((ZAnimationTrack<bool>*)srcAnimationTrack);
            return newKeyFrameTrack;
            break;
        }
*/
    default:
        {
            return NULL;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZAnimationTrackInternal * ZAnimationTrackInternal::Create(ZANIMATION_KEY_TYPE keyType)
{

    switch(keyType)
    {
    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            ZAnimationTrack<float> * newKeyFrameTrack = new ZAnimationTrack<float>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:
        {
            ZAnimationTrack<tlong> * newKeyFrameTrack = new ZAnimationTrack<tlong>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            ZAnimationTrack<tvector3> * newKeyFrameTrack = new ZAnimationTrack<tvector3>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            ZAnimationTrack<tcolor> * newKeyFrameTrack = new ZAnimationTrack<tcolor>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            ZAnimationTrack<tquaternion> * newKeyFrameTrack = new ZAnimationTrack<tquaternion>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            return newKeyFrameTrack;
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            ZAnimationTrack<bool> * newKeyFrameTrack = new ZAnimationTrack<bool>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            return newKeyFrameTrack;
            break;
        }
*/
    default:
        {
            return NULL;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZAnimationTrackInternal * ZAnimationTrackInternal::Create(ZFile & file)
{
    ZANIMATION_KEY_TYPE keyType;
    file >> keyType;

    switch(keyType)
    {
    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            ZAnimationTrack<float> * newKeyFrameTrack = new ZAnimationTrack<float>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            newKeyFrameTrack->Load(file);
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:
        {
            ZAnimationTrack<tlong> * newKeyFrameTrack = new ZAnimationTrack<tlong>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            newKeyFrameTrack->Load(file);
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            ZAnimationTrack<tvector3> * newKeyFrameTrack = new ZAnimationTrack<tvector3>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            newKeyFrameTrack->Load(file);
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            ZAnimationTrack<tcolor> * newKeyFrameTrack = new ZAnimationTrack<tcolor>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            newKeyFrameTrack->Load(file);
            return newKeyFrameTrack;
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            ZAnimationTrack<tquaternion> * newKeyFrameTrack = new ZAnimationTrack<tquaternion>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            newKeyFrameTrack->Load(file);
            return newKeyFrameTrack;
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            ZAnimationTrack<bool> * newKeyFrameTrack = new ZAnimationTrack<bool>;
            newKeyFrameTrack->m_KeyFrameType = keyType;
            newKeyFrameTrack->Load(file);
            return newKeyFrameTrack;
            break;
        }
*/
    default:
        {
            return NULL;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimationTrackInternal::Save(ZFile &file)
{
    switch(m_KeyFrameType)
    {
    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            ((ZAnimationTrack<float> *)this)->Save(file);
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:       
        {
            ((ZAnimationTrack<tlong> *)this)->Save(file);
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            ((ZAnimationTrack<tvector3> *)this)->Save(file);
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            ((ZAnimationTrack<tcolor> *)this)->Save(file);
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            ((ZAnimationTrack<tquaternion> *)this)->Save(file);
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            ((ZAnimationTrack<bool> *)this)->Save(file);
            break;
        }
*/
    default:
        {
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

float ZAnimationTrackInternal::GetAnimationTrackLength()
{
    switch(m_KeyFrameType)
    {

    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            if(!((ZAnimationTrack<float> *)this)->m_KeyFrameList.empty())
            {
                return ((ZAnimationTrack<float> *)this)->m_KeyFrameList[((ZAnimationTrack<float> *)this)->m_KeyFrameList.size()-1].m_Time;
            }
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:       
        {
            if(!((ZAnimationTrack<tlong> *)this)->m_KeyFrameList.empty())
            {
                return ((ZAnimationTrack<tlong> *)this)->m_KeyFrameList[((ZAnimationTrack<tlong> *)this)->m_KeyFrameList.size()-1].m_Time;
            }
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            if(!((ZAnimationTrack<tvector3> *)this)->m_KeyFrameList.empty())
            {
                return ((ZAnimationTrack<tvector3> *)this)->m_KeyFrameList[((ZAnimationTrack<tvector3> *)this)->m_KeyFrameList.size()-1].m_Time;
            }
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            if(!((ZAnimationTrack<tcolor> *)this)->m_KeyFrameList.empty())
            {
                return ((ZAnimationTrack<tcolor> *)this)->m_KeyFrameList[((ZAnimationTrack<tcolor> *)this)->m_KeyFrameList.size()-1].m_Time;
            }
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            if(!((ZAnimationTrack<tquaternion> *)this)->m_KeyFrameList.empty())
            {
                return ((ZAnimationTrack<tquaternion> *)this)->m_KeyFrameList[((ZAnimationTrack<tquaternion> *)this)->m_KeyFrameList.size()-1].m_Time;
            }
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            if(!((ZAnimationTrack<bool> *)this)->m_KeyFrameList.empty())
            {
                return ((ZAnimationTrack<bool> *)this)->m_KeyFrameList[((ZAnimationTrack<bool> *)this)->m_KeyFrameList.size()-1].m_Time;
            }
            break;
        }
*/
    default:
        {
            return 0;
            break;
        }
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

tulong ZAnimationTrackInternal::GetKeyFrameCount()
{
    switch(m_KeyFrameType)
    {
    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            return (tulong)((ZAnimationTrack<float> *)this)->m_KeyFrameList.size();
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:       
        {
            return (tulong)((ZAnimationTrack<tlong> *)this)->m_KeyFrameList.size();
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            return (tulong)((ZAnimationTrack<tvector3> *)this)->m_KeyFrameList.size();
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            return (tulong)((ZAnimationTrack<tcolor> *)this)->m_KeyFrameList.size();
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            return (tulong)((ZAnimationTrack<tquaternion> *)this)->m_KeyFrameList.size();
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            return (tulong)((ZAnimationTrack<bool> *)this)->m_KeyFrameList.size();
            break;
        }
*/
    default:
        {
            return 0;
            break;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void ZAnimationTrackInternal::Interpolate(void * res, float time, tlong &curKey)
{
    switch(m_KeyFrameType)
    {
    case ZANIMATION_KEY_TYPE_NONE:
        return ;

    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            ((ZAnimationTrack<float> *)this)->Interpolate((float*)res, time, curKey);
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:       
        {
            ((ZAnimationTrack<tlong> *)this)->Interpolate((tlong*)res, time, curKey);
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            ((ZAnimationTrack<tvector3> *)this)->Interpolate((tvector3*)res, time, curKey);
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            ((ZAnimationTrack<tcolor> *)this)->Interpolate((tcolor*)res, time, curKey);
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            ((ZAnimationTrack<tquaternion> *)this)->Interpolate((tquaternion*)res, time, curKey);
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            ((ZAnimationTrack<bool> *)this)->Interpolate((bool*)res, time, curKey);
            break;
        }
*/
    default:
        {
            return ;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
tulong ZAnimationTrackInternal::Optimize()
{
switch(m_KeyFrameType)
{
case ZANIMATION_KEY_TYPE_NONE:
{
break;
}

case ZANIMATION_KEY_TYPE_FLOAT: 
{
return ((ZAnimationTrack<float> *)this)->OptimizeKey();
break;
}

case ZANIMATION_KEY_TYPE_LONG:       
{
return ((ZAnimationTrack<tlong> *)this)->OptimizeKey();
break;
}

case ZANIMATION_KEY_TYPE_VECTOR3:    
{
return ((ZAnimationTrack<tvector3> *)this)->OptimizeKey();
break;
}

case ZANIMATION_KEY_TYPE_COLOR:      
{
return ((ZAnimationTrack<tcolor> *)this)->OptimizeKey();
break;
}

case ZANIMATION_KEY_TYPE_QUATERNION: 
{
return ((ZAnimationTrack<tquaternion> *)this)->OptimizeKey();
break;
}

case ZANIMATION_KEY_TYPE_BOOL:
{
return ((ZAnimationTrack<bool> *)this)->OptimizeKey();
break;
}

default:
{
return 0;
break;
}
}    
}
*/
///////////////////////////////////////////////////////////////////////////////////////////////////

tulong ZAnimationTrackInternal::Optimize()
{
    tulong keyCount=0;

    switch(m_KeyFrameType)
    {
    case ZANIMATION_KEY_TYPE_NONE:
        {
            break;
        }

    case ZANIMATION_KEY_TYPE_FLOAT: 
        {
            tarray<ZKeyFrame<float> > * trackKeys = &(((ZAnimationTrack<float> *)this)->m_KeyFrameList);

            if((*trackKeys).size()>=3)
            {
                for(tulong i=1 ; i< (*trackKeys).size()-1; i++)
                {
                    if( MathFloatIsEqual((*trackKeys)[i-1].m_Value, (*trackKeys)[i].m_Value) && MathFloatIsEqual((*trackKeys)[i+1].m_Value, (*trackKeys)[i].m_Value))
                    {
                        (*trackKeys).erase(trackKeys->begin() + i);
                        keyCount++;
                    }
                }
            }
            return keyCount;            
            break;
        }

    case ZANIMATION_KEY_TYPE_LONG:       
        {
            tarray<ZKeyFrame<tlong> > * trackKeys = &(((ZAnimationTrack<tlong> *)this)->m_KeyFrameList);

            if((*trackKeys).size()>=3)
            {
                for(tulong i=1 ; i< (*trackKeys).size()-1; i++)
                {
                    if(((*trackKeys)[i-1].m_Value == (*trackKeys)[i].m_Value) && ((*trackKeys)[i+1].m_Value == (*trackKeys)[i].m_Value))
                    {
                        (*trackKeys).erase(trackKeys->begin() + i);
                        keyCount++;
                    }
                }
            }
            return keyCount;            
            break;
        }

    case ZANIMATION_KEY_TYPE_VECTOR3:    
        {
            tarray<ZKeyFrame<tvector3> > * trackKeys = &(((ZAnimationTrack<tvector3> *)this)->m_KeyFrameList);

            if((*trackKeys).size()>=3)
            {
                for(tulong i=1 ; i< (*trackKeys).size()-1; i++)
                {
                    if((*trackKeys)[i-1].m_Value.IsVeryClose( (*trackKeys)[i].m_Value) && ((*trackKeys)[i+1].m_Value.IsVeryClose((*trackKeys)[i].m_Value)) )
                    {
                        (*trackKeys).erase(trackKeys->begin() + i);
                        keyCount++;
                    }
                }
            }
            return keyCount;            
            break;
        }

    case ZANIMATION_KEY_TYPE_COLOR:      
        {
            tarray<ZKeyFrame<tcolor> > * trackKeys = &(((ZAnimationTrack<tcolor> *)this)->m_KeyFrameList);

            if((*trackKeys).size()>=3)
            {
                for(tulong i=1 ; i< (*trackKeys).size()-1; i++)
                {
                    if((*trackKeys)[i-1].m_Value.IsVeryClose( (*trackKeys)[i].m_Value) && ((*trackKeys)[i+1].m_Value.IsVeryClose((*trackKeys)[i].m_Value)) )
                    {
                        (*trackKeys).erase(trackKeys->begin() + i);
                        keyCount++;
                    }
                }
            }
            return keyCount;            
            break;
        }

    case ZANIMATION_KEY_TYPE_QUATERNION: 
        {
            tarray<ZKeyFrame<tquaternion> > * trackKeys = &(((ZAnimationTrack<tquaternion> *)this)->m_KeyFrameList);

            if((*trackKeys).size()>=3)
            {
                for(tulong i=1 ; i< (*trackKeys).size()-1; i++)
                {
                    if((*trackKeys)[i-1].m_Value.IsVeryClose( (*trackKeys)[i].m_Value) && ((*trackKeys)[i+1].m_Value.IsVeryClose((*trackKeys)[i].m_Value)) )
                    {
                        (*trackKeys).erase(trackKeys->begin() + i);
                        keyCount++;
                    }
                }
            }
            return keyCount;            
            break;
        }
/*
    case ZANIMATION_KEY_TYPE_BOOL:
        {
            tarray<ZKeyFrame<bool> > * trackKeys = &(((ZAnimationTrack<bool> *)this)->m_KeyFrameList);

            if((*trackKeys).size()>=3)
            {
                for(tulong i=1 ; i< (*trackKeys).size()-1; i++)
                {
                    if(((*trackKeys)[i-1].m_Value == (*trackKeys)[i].m_Value) && ((*trackKeys)[i+1].m_Value == (*trackKeys)[i].m_Value))
                    {
                        (*trackKeys).erase(trackKeys->begin() + i);
                        keyCount++;
                    }
                }
            }
            return keyCount;            
            break;
        }
*/
    }    

    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
