///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// File Name : ZKeyFrame.h
// Creation : 24/11/2004
// Author : Cedric Guillemet
// Description :
// Reliability : 0/10 
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef ZKeyFrame_H__
#define ZKeyFrame_H__

// Includes ///////////////////////////////////////////////////////////////////////////////////////

#include "../libbase/LibBase.h"

// Constants //////////////////////////////////////////////////////////////////////////////////////

enum ZANIMATION_KEY_TYPE
{
   ZANIMATION_KEY_TYPE_NONE,            
   ZANIMATION_KEY_TYPE_FLOAT,      
   ZANIMATION_KEY_TYPE_LONG,       
   ZANIMATION_KEY_TYPE_VECTOR3,    
   ZANIMATION_KEY_TYPE_COLOR,      
   ZANIMATION_KEY_TYPE_QUATERNION, 
   ZANIMATION_KEY_TYPE_BOOL
};

enum ZANIMATION_INTERP_TYPE
{
    ZANIMATION_INTERP_NONE,
    ZANIMATION_INTERP_LINEAR,
    ZANIMATION_INTERP_BEZIER,
    ZANIMATION_INTERP_HERMITE
};

enum ZANIMATION_OUT_OF_RANGE_TYPE
{
    ZANIMATION_OUT_OF_RANGE_NONE,          
    ZANIMATION_OUT_OF_RANGE_CONSTANT,    
    ZANIMATION_OUT_OF_RANGE_CYCLE,        
    ZANIMATION_OUT_OF_RANGE_LOOP,         
    ZANIMATION_OUT_OF_RANGE_PINGPONG,    
    ZANIMATION_OUT_OF_RANGE_LINEAR,        
    ZANIMATION_OUT_OF_RANGE_RELATIVE    
};


// Globals ////////////////////////////////////////////////////////////////////////////////////////

// Class //////////////////////////////////////////////////////////////////////////////////////////

template <class type> class ZKeyFrame
{
    public:
        float m_Time;
        type   m_Value;

    public:
        // Constructors

        ZKeyFrame()
        {
        }

        ZKeyFrame(ZKeyFrame * srcKeyFrame)
        {
            m_Time = srcKeyFrame->m_Time;
            m_Value = srcKeyFrame->m_Value;
        }

        ZKeyFrame(float time, type value)
        {
            m_Time = time;
            m_Value = value;
        }

        
        // Accessors

        type GetValue()
        {
            return m_Value;
        }

        void SetValue(type value)
        {
            m_Value = value;
        }

        float GetTime() { return m_Time; }


        // Interpolation Function
        type InterpolateFlat(void * res)  
        {  
            return *(type*)res = m_Value;
        }  

        // Interpolation Function
        float InterpolateLinear(void * res, ZKeyFrame<float> & nextKey, float ratio=0)  
        {  
            return (*(float*)res = ( nextKey.m_Value - m_Value ) * ratio + m_Value);
        }  

        float InterpolateHermite(void * res, ZKeyFrame<float> &nextKey, float &nextKeyP1, float &prevKey, float ratio=0)  
        {  
            //((tvector3*)res)->Lerp(m_Value, nextKey.m_Value, ratio );
            //return *((tvector3*)res);
            float t = ratio;
            float t2 = t*t;
            float t3 = t2*t;
            float h1 = 2*t3 - 3*t2 + 1.0f;
            float h2 = -2*t3 + 3*t2;
            float h3 = (t3 - 2*t2 + t) * .5f;
            float h4 = (t3 - t2) *.5f;

            return m_Value * h1 + nextKey.m_Value *h2 + (nextKey.m_Value - prevKey) * h3 + (nextKeyP1 - m_Value) * h4;
        } 

        tlong InterpolateLinear(void * res, ZKeyFrame<tlong> &nextKey, float ratio=0)  
        {  
            return (*(tlong*)res = (tlong)( ((float)nextKey.m_Value - m_Value ) * ratio + (float)m_Value ));
        }  

        tlong InterpolateHermite(void * res, ZKeyFrame<tlong> &nextKey, tlong &nextKeyP1, tlong &prevKey, float ratio=0)  
        {  
            //((tvector3*)res)->Lerp(m_Value, nextKey.m_Value, ratio );
            //return *((tvector3*)res);
            float t = ratio;
            float t2 = t*t;
            float t3 = t2*t;
            float h1 = 2*t3 - 3*t2 + 1.0f;
            float h2 = -2*t3 + 3*t2;
            float h3 = (t3 - 2*t2 + t) * .5f;
            float h4 = (t3 - t2) *.5f;

            return (tlong)(m_Value * h1 + nextKey.m_Value *h2 + (nextKey.m_Value - prevKey) * h3 + (nextKeyP1 - m_Value) * h4);
        } 

        tvector3 InterpolateLinear(void * res, ZKeyFrame<tvector3> &nextKey, float ratio=0)  
        {  
            ((tvector3*)res)->Lerp(m_Value, nextKey.m_Value, ratio );
            return *((tvector3*)res);
        } 

        tvector3 InterpolateHermite(void * res, ZKeyFrame<tvector3> &nextKey, tvector3 &nextKeyP1, tvector3 &prevKey, float ratio=0)  
        {  
            //((tvector3*)res)->Lerp(m_Value, nextKey.m_Value, ratio );
            //return *((tvector3*)res);
            float t = ratio;
            float t2 = t*t;
            float t3 = t2*t;
            float h1 = 2*t3 - 3*t2 + 1.0f;
            float h2 = -2*t3 + 3*t2;
            float h3 = (t3 - 2*t2 + t) * .5f;
            float h4 = (t3 - t2) *.5f;

            *((tvector3*)res) = m_Value * h1 + nextKey.m_Value *h2 + (nextKey.m_Value - prevKey) * h3 + (nextKeyP1 - m_Value) * h4;
            return *((tvector3*)res);
        } 

        tcolor InterpolateLinear(void * res, ZKeyFrame<tcolor> &nextKey, float ratio=0)  
        {  
            ((tcolor*)res)->Lerp(m_Value, nextKey.m_Value, ratio );
            return *((tcolor*)res);
        } 

        tcolor InterpolateHermite(void * res, ZKeyFrame<tcolor> &nextKey, tcolor &nextKeyP1, tcolor &prevKey, float ratio=0)  
        {  
            //((tvector3*)res)->Lerp(m_Value, nextKey.m_Value, ratio );
            //return *((tvector3*)res);
            float t = ratio;
            float t2 = t*t;
            float t3 = t2*t;
            float h1 = 2*t3 - 3*t2 + 1.0f;
            float h2 = -2*t3 + 3*t2;
            float h3 = (t3 - 2*t2 + t) * .5f;
            float h4 = (t3 - t2) *.5f;

            return m_Value * h1 + nextKey.m_Value *h2 + (nextKey.m_Value - prevKey) * h3 + (nextKeyP1 - m_Value) * h4;
        } 

        tquaternion InterpolateLinear(void * res, ZKeyFrame<tquaternion> &nextKey, float ratio=0)  
        {  
            ((tquaternion*)res)->Slerp(m_Value, nextKey.m_Value, ratio );
            return *((tquaternion*)res);
        }  

        tquaternion InterpolateHermite(void * res, ZKeyFrame<tquaternion> &nextKey, tquaternion &nextKeyP1, tquaternion &prevKey, float ratio=0)  
        {  
            //((tvector3*)res)->Lerp(m_Value, nextKey.m_Value, ratio );
            //return *((tvector3*)res);
            float t = ratio;
            float t2 = t*t;
            float t3 = t2*t;
            float h1 = 2*t3 - 3*t2 + 1.0f;
            float h2 = -2*t3 + 3*t2;
            float h3 = (t3 - 2*t2 + t) * .5f;
            float h4 = (t3 - t2) *.5f;

            return m_Value * h1 + nextKey.m_Value *h2 + (nextKey.m_Value - prevKey) * h3 + (nextKeyP1 - m_Value) * h4;
        } 

        bool InterpolateLinear(void * res, ZKeyFrame<bool> &kf2, float ratio=0)  
        {  
            return (*(bool*)res = m_Value);
        } 
        bool InterpolateHermite(void * res, ZKeyFrame<bool> &nextKey, bool &nextKeyP1, bool &prevKey, float ratio=0)  
        {  
            return (*(bool*)res = m_Value);
        } 
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif
