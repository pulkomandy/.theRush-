///////////////////////////////////////////////////////////////////////////////////////////////////
// Zenith Engine
// Creation : 22/09/2006
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

#include "ZSoundFMOD.h"

#include "../libbase/ZLogger.h"
#include "../libbase/ZFile.h"
#include "../libbase/ZProfiler.h"
#include "../libbase/ZTimer.h"
#include "../libworld/ZCamera.h"

#include <fmod.hpp>
#include <fmod_errors.h>
#ifdef WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <stdio.h>
#include <math.h>
#include "ZSoundFMOD.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
ZSoundManager *GSoundManager = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////


const float DISTANCEFACTOR = 1.0f;          // Units per meter.  I.e feet would = 3.28.  centimeters would = 100.

void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        LOG("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        //Zexit(-1);
    }
}


FMOD_RESULT F_CALLBACK FMODopen(const char *name, int /*unicode*/, unsigned int *filesize, void **handle, void **userdata)
{
    if (name)
    {

        ZFile *fp = new ZFile;
        if (!fp->Open(name))
        {
            delete fp;
            return FMOD_ERR_FILE_NOTFOUND;
        }
        *filesize = fp->GetSize();
        *userdata = (void*) fp;
        *handle = fp;
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMODclose(void *handle, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    ZFile *fp = (ZFile*)userdata;
    delete fp;
    //fclose((FILE *)handle);

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMODread(void *handle, void *buffer, unsigned int sizebytes, unsigned int *bytesread, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (bytesread)
    {
        ZFile *fp = (ZFile*)userdata;
        *bytesread = (int)fp->Read(buffer, sizebytes);

        //*bytesread = (int)fread(buffer, 1, sizebytes, (FILE *)handle);

        if (*bytesread < sizebytes)
        {
            return FMOD_ERR_FILE_EOF;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT F_CALLBACK FMODseek(void *handle, unsigned int pos, void *userdata)
{
    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    ZFile *fp = (ZFile*)userdata;
    fp->Seek(pos);
    //fseek((FILE *)handle, pos, SEEK_SET);

    return FMOD_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*
typedef struct FMODSampleInstance
{
	FMOD::Channel    *channel;
	bool b3D;
	ITransform *mTransform;
	tvector3 mLastPos;
} FMODSampleInstance;
*/
/*
typedef struct FMODSample
{
	FMOD::Sound      *sound;

	tlist<FMODSampleInstance*> instances;
} FMODSample;
*/
//static std::list<FMODSampleInstance*> mPlaybackInstances;

///////////////////////////////////////////////////////////////////////////////////////////////////

ZSoundManager::ZSoundManager(void)
{
	GSoundManager = this;
	mUpdateStamp = 0;
	Init();
}

ZSoundManager::~ZSoundManager(void)
{
	UnInit();
}

void	ZSoundManager::Init()
{
	unsigned int     version;
	FMOD_RESULT      result;
/*
        Create a System object and initialize.
    */
    result = FMOD::System_Create(&fsystem);
    ERRCHECK(result);

    result = fsystem->getVersion(&version);
    //ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        LOG("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        return ;
    }

	result = fsystem->setDriver(0);
	ERRCHECK(result);

    result = fsystem->init(100, FMOD_INIT_NORMAL, 0);
    ERRCHECK(result);

    result = fsystem->setFileSystem(&FMODopen, &FMODclose, &FMODread, &FMODseek, NULL, NULL, 2048);
    ERRCHECK(result);

    /*
        Set the distance units. (meters/feet etc).
    */
    result = fsystem->set3DSettings(1.0, DISTANCEFACTOR, 1.0f);
    ERRCHECK(result);


	LOG("FMod Init OK V%08x.\r\n", version);
}
/*
// Reads and writes per second of the sound data
// Speex only supports these 3 values
#define SAMPLE_RATE  (8000)
//#define SAMPLE_RATE  (16000)
//#define SAMPLE_RATE  (32000)

#define FRAMES_PER_BUFFER  (2048 / (32000 / SAMPLE_RATE))

// define sample type. Only short(16 bits sound) is supported at the moment.
typedef short SAMPLE;
*/


void	ZSoundManager::UnInit()
{

	FMOD_RESULT      result;
    result = fsystem->close();
    ERRCHECK(result);
    result = fsystem->release();
    ERRCHECK(result);
}

bool	ZSoundManager::Tick()
{
	PROFILER_START(ZSoundManager);

	FMOD_RESULT      result;

	bool bRet = false;
	static const float UpdateFrequency = (1.0f/50.0f);
	float elapsed = gTimer.GetEllapsed();
	mUpdateStamp += elapsed;

	while (mUpdateStamp >= UpdateFrequency)
	{
		// update listener
		if (GetActiveCamera())
		{
			ZCamera * pCam = GetActiveCamera();
			const tmatrix& camMat = pCam->GetTransform()->GetWorldMatrix();
			
			tvector3 vel(0,0,0);

			// ********* NOTE ******* READ NEXT COMMENT!!!!!
			// vel = how far we moved last FRAME (m/f), then time compensate it to SECONDS (m/s).
			vel *= (1.0f / mUpdateStamp);


			FMOD_VECTOR campos = {camMat.V4.position.x, camMat.V4.position.y, camMat.V4.position.z};
			FMOD_VECTOR camdir = {camMat.V4.dir.x, camMat.V4.dir.y, camMat.V4.dir.z};
			FMOD_VECTOR camup = {camMat.V4.up.x, camMat.V4.up.y, camMat.V4.up.z};
			FMOD_VECTOR camvel = {vel.x, vel.y, vel.z};

			result = fsystem->set3DListenerAttributes(0, &campos, &camvel, &camdir, &camup);

			

			ERRCHECK(result);
		}

		// update 3D sounds
		
		std::list<smartptr<ZSoundInstance> >::iterator iiti = mCurrentlyPlaying.begin();
		for (;iiti != mCurrentlyPlaying.end(); )
		{
			ZSoundInstance* pInst = (ZSoundInstance*)(*iiti).ptr();
				
			if (!pInst->isPlaying())
			{
				iiti = mCurrentlyPlaying.erase(iiti);
				continue;
			}

			//update 3D Transform things
			if (pInst->is3DSound() && pInst->getTransform())
			{

				const tvector3& wp = pInst->getTransform()->GetWorldMatrix().V4.position;
				tvector3 velTransform(0,0,0);

/*
				if (pInst->mLastPos != tvector3::zero)
					velTransform = wp - pInst->mLastPos;
*/
				velTransform *= (1.0f / mUpdateStamp);
				pInst->mLastPos = wp;




				FMOD_VECTOR fmpos = { wp.x, wp.y, wp.z };
				FMOD_VECTOR fmvel = { velTransform.x, velTransform.y, velTransform.z };

				result = pInst->mChannel->set3DAttributes(&fmpos, &fmvel);

			}
			
			pInst->mChannel->setVolume(pInst->mVolume);
			pInst->mChannel->setFrequency(pInst->mDefaultFreq * pInst->mPitch);

			++iiti;
		}

		// update FMOD
		fsystem->update();

		mUpdateStamp -= UpdateFrequency;
	}
    
	PROFILER_END();

	return bRet;

}

void ZSoundSample::Preload(const char* szFileName)
{
	FMOD_RESULT      result;

	if (mbStream)
	{
        result = GSoundManager->fsystem->createStream(szFileName, FMOD_HARDWARE|(mb3D?FMOD_3D:FMOD_2D) /*| FMOD_LOOP_NORMAL | FMOD_2D*/, 0, &mSound);
	}
	else
	{
		result = GSoundManager->fsystem->createSound(szFileName, FMOD_HARDWARE|(mb3D?FMOD_3D:FMOD_2D)/*|FMOD_LOOP_NORMAL|FMOD_3D*/, 0, &mSound);
	}
    
    if (result != FMOD_OK)
    {
        WARN("Sound Error : %s not found.", szFileName);
    }
    SetName(szFileName);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

void		ZSoundSample::Preload(const unsigned char* pBuf, int aSampleByteSize)
{
	FMOD_RESULT      result;

	if (mbStream)
	{
        result = GSoundManager->fsystem->createStream((const char*)pBuf, FMOD_OPENMEMORY|FMOD_HARDWARE|(mb3D?FMOD_3D:FMOD_2D) /*| FMOD_LOOP_NORMAL | FMOD_2D*/, 0, &mSound);
	}
	else
	{
		FMOD_CREATESOUNDEXINFO fmcsei;
		memset(&fmcsei, 0, sizeof(FMOD_CREATESOUNDEXINFO));
		fmcsei.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		fmcsei.length = aSampleByteSize;

		result = GSoundManager->fsystem->createSound((const char*)pBuf, FMOD_OPENMEMORY|FMOD_HARDWARE|(mb3D?FMOD_3D:FMOD_2D)/*|FMOD_LOOP_NORMAL|FMOD_3D*/, &fmcsei, &mSound);
	}
    if (result != FMOD_OK)
    {
        WARN("Sound Error : %s not found.", mName.c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

ZSoundSample*	ZSoundManager::NewSoundSample(bool b3D, bool bStreamed)
{
	ZSoundSample* nSS = new ZSoundSample(b3D, bStreamed);
	_AddInstance(nSS);
	return nSS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


BEGIN_SERIALIZATION(ZSoundManager)
END_SERIALIZATION()



BEGIN_SERIALIZATION(ZSoundInstance)
END_SERIALIZATION()



BEGIN_SERIALIZATION(ZSoundSample)
END_SERIALIZATION()


///////////////////////////////////////////////////////////////////////////////////////////////////

ZSoundInstance *			ZSoundSample::Instance()
{
	ZSoundInstance *pInst = new ZSoundInstance(this);
	_AddInstance(pInst);
	return pInst;
}

///////////////////////////////////////////////////////////////////////////////////////////////////



bool	ZSoundInstance::isPlaying()
{
	FMOD_RESULT      result;

	bool bisp;
	result = mChannel->isPlaying(&bisp);
	return bisp;
}

bool	ZSoundInstance::isReady()
{
	return true;
}

float	ZSoundInstance::getPlaybackTime()
{
	return 0.0f;
}



void	ZSoundInstance::play(const bool /*aFlags*/)
{
	if (isPlaying())
		return;

	FMOD_RESULT      result;

	result = GSoundManager->fsystem->playSound(FMOD_CHANNEL_FREE, mSample->mSound, true, &mChannel);

	
	if (mSample->mb3D)
	{
        mChannel->setMode((mbLoop?FMOD_LOOP_NORMAL:FMOD_LOOP_OFF)|FMOD_HARDWARE|FMOD_3D|
			( (mAttnType == 0)?FMOD_3D_LINEARROLLOFF:FMOD_3D_LINEARSQUAREROLLOFF)  );
        mChannel->set3DMinMaxDistance(mInner, mOutter);
	}
    else
    {
        mChannel->setMode((mbLoop?FMOD_LOOP_NORMAL:FMOD_LOOP_OFF)|FMOD_HARDWARE|FMOD_2D);
    }

    //mDefaultFreq;
    mChannel->getFrequency(&mDefaultFreq);
    mChannel->setFrequency(mDefaultFreq * mPitch);
    mChannel->setVolume(mVolume);


    result = mChannel->setPaused(false);

	GSoundManager->mCurrentlyPlaying.push_back(this);
}

void	ZSoundInstance::stop()
{
	if (mChannel&&isPlaying())
	{
		mChannel->stop();
		mChannel = NULL;
		GSoundManager->mCurrentlyPlaying.remove(this);
	}

}
