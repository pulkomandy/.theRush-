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

#ifndef ZSOUNDFMOD_LIB_H_
#define ZSOUNDFMOD_LIB_H_

#include <fmod.hpp>


DECLAREZCLASS(ZSoundSample);
DECLAREZCLASS(ZSoundInstance);
DECLAREZCLASS(ZSoundManager);


enum FSoundFlags
{
	MASND_UNDEFINED		=	0x0000,		// undefined
	MASND_STREAMED		=	0x0001,		// sound is streamed
	MASND_LOOP			=	0x0002,		// sound is looping
	MASND_ISPLAYING		=	0x0004,		// sound is currently playing
	MASND_PLANDESTROY	=	0x0008,		// destroy sound when it's over
	MASND_STOPAFTERFADE	=	0x0010,		// stop playing sound after fade is over
	MASND_ISSYNCING		=	0x0020,		// stream is buffering (keyon disabled)
};

class ZSoundInstance;

class ZSoundSample : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE

	ZSoundSample() { mAttnType = 0;}

	ZSoundSample(bool b3D, bool bStreamed) : mVolume(1.0f), mInner(2.0f), mOutter(15.0f),
		mVolumeGroup(0)
	{
		mb3D = b3D;
		mbStream = bStreamed;
		mbLoop = false;
        mPitch = 1;
        mVolume = 1;
		mAttnType = 0;
	}
    virtual ~ZSoundSample()
	{
	}

	float	getCurrentVolume() { return mVolume; }
	void	setCurrentVolume(const float aVolume=1.0f) {	mVolume = aVolume; }
	float	getInnerAttenuationRadius() { return mInner; }
	float	getOuterAttenuationRadius() { return mOutter; }
	void	setInnerAttenuationRadius(const float aInnerRadius=1.0f) { mInner = aInnerRadius; }
	void	setOuterAttenuationRadius(const float aOuterRadius=5.0f) { mOutter = aOuterRadius; }
	void	setGroupVolume(int aVolumeGroup) { mVolumeGroup = aVolumeGroup; }
	int		getGroupVolume() { return mVolumeGroup; }
	float	getPitch() { return mPitch; }
	void	setPitch(const float aPitch) {mPitch = aPitch; }		// Can only be set before playing, after it's ignored
	bool	isStreaming() { return mbStream; }
	void	setLoop(bool bLoop) { mbLoop = bLoop; }
	bool	getLoop() { return mbLoop; }
	void	setAttenuationType(int aType) { mAttnType = aType; }
	int     getAttenuationType() { return mAttnType; }


	void	Preload(const unsigned char* pBuf, int aSampleByteSize);
	void	Preload(const char* szFileName);

	ZSoundInstance *Instance();

	const char *GetArtist(unsigned int &alength)
	{
		FMOD_RESULT result;
		// sound tag test
		FMOD_TAG mTag;
		alength = 0;
		result =  mSound->getTag("ARTIST",0, &mTag);
		if (result == FMOD_OK)
		{
			alength=mTag.datalen;
			return (const char *)mTag.data;
		}
		else
			return NULL;
	}
	const char *GetSongName(unsigned int &alength)
	{
		FMOD_RESULT result;
		FMOD_TAG mTag;
		alength = 0;
		result =  mSound->getTag("TITLE",0, &mTag);
		if (result == FMOD_OK)
		{
			alength=mTag.datalen;
			return (const char *)mTag.data;
		}
		else
			return NULL;
	}
	const char *GetYear(unsigned int &alength)
	{
		FMOD_RESULT result;
		FMOD_TAG mTag;
		alength = 0;
		result =  mSound->getTag("DATE",0, &mTag);
		if (result == FMOD_OK)
		{
			alength=mTag.datalen;
			return (const char *)mTag.data;
		}
		else
			return NULL;
	}


public:
	bool						mb3D;
	float						mVolume;
	float						mInner;
	float						mOutter;
	int							mVolumeGroup;
	bool						mbStream;
	float						mPitch;
	bool						mbLoop;
	FMOD::Sound					*mSound;
	unsigned int				mAttnType;

	friend class ZSoundInstance;
};


class ZSoundInstance : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE


	ZSoundInstance() {mAttnType = 0; mChannel = NULL; }
	ZSoundInstance(ZSoundSample *pSample)
	{
		mChannel = NULL;
		SetSample(pSample);
	}

	virtual ~ZSoundInstance()
	{
		stop();
	}



	void SetSample(ZSoundSample *pSample)
	{
		// copy sample
		mSample = pSample;

		mVolume = mSample->mVolume;
		mInner = mSample->mInner;
		mOutter = mSample->mOutter;
		mVolumeGroup = mSample->mVolumeGroup;
		mbStream = mSample->mbStream;
		mbLoop = mSample->mbLoop;
		mPitch = mSample->mPitch;
		mAttnType = mSample->getAttenuationType();
	}

	ZSoundSample *GetSample() const { return mSample; }

	float	getCurrentVolume() const { return mVolume; }
	void	setCurrentVolume(const float aVolume=1.0f)
	{
		mVolume = aVolume;
	}
	float	getInnerAttenuationRadius() const { return mInner; }
	float	getOuterAttenuationRadius() const { return mOutter; }
	void	setInnerAttenuationRadius(const float aInnerRadius=1.0f) { mInner = aInnerRadius; }
	void	setOuterAttenuationRadius(const float aOuterRadius=5.0f) { mOutter = aOuterRadius; }

	void	setAttenuationType(int aType) { mAttnType = aType; }
	int     getAttenuationType() const { return mAttnType; }

	void	setGroupVolume(int aVolumeGroup) { mVolumeGroup = aVolumeGroup; }
	int		getGroupVolume() const { return mVolumeGroup; }
	float	getPitch() const { return mPitch; }
	void	setPitch(const float aPitch)
	{
		mPitch = aPitch;
	}		// Can only be set before playing, after it's ignored
	bool	isStreaming() const { return mbStream; }
	void	setLoop(bool bLoop) { mbLoop = bLoop; }
	bool	getLoop() const { return mbLoop; }
	void	setTransform(ZTransform*pTransform) 
	{ 
		mTransform = pTransform;
	}
	ZTransform* getTransform() const { return mTransform; }


	// Queries
	bool	isPlaying();
	bool	isReady();
	float	getPlaybackTime();
	bool	is3DSound() { return mSample->mb3D; }

	// Settings

	// Actions
	void	play(const bool aFlags=0);			// Bit0:Loop Bit1:KeyOff
	void	stop();

	// load/save

	//virtual void	softstop();

protected:
	smartptr<ZSoundSample>		mSample;
	float						mVolume;
	float						mInner;
	float						mOutter;
	int							mVolumeGroup;
	
	bool						mbStream;
	float						mPitch;
	smartptr<ZTransform>		mTransform;
	bool						mbLoop;
	FMOD::Channel*				mChannel;
	tvector3					mLastPos;
	float						mDefaultFreq;
	unsigned int				mAttnType;

	friend class ZSoundManager;

};

class ZSoundManager : public ZBaseClass
{
public:
    DECLARE_SERIALIZABLE

public:
	ZSoundManager();
	~ZSoundManager();


	// Manager life
	bool			Tick();

	// Action
	/*
	virtual ZSoundSample*	Preload(const tstring& aFileName, bool aStreamed = false, bool b3D = false);
	
	virtual void			Preload(ZSoundSample* pSS, const unsigned char*pBuf, int aSampleByteSize, bool b3D = false);
*/
	ZSoundSample*	NewSoundSample(bool b3D, bool bStreamed);

	void			setDefaultGroupVolume(int aGroup) {}
	int				getDefaultGroupVolume() { return 0; }
	void			setGroupMasterVolume(int aGroup, float aLeft, float aRight) {}
	void			getGroupMasterVolume(int aGroup, float& aLeft, float& aRight) {}

private:
	FMOD::System    *fsystem;

	tvector3 mLastpos;
	std::list<smartptr<ZSoundSample> > mPreloadedSounds;
	std::list<smartptr<ZSoundInstance> > mCurrentlyPlaying;

	float mUpdateStamp;

	friend class ZSoundSample;
	friend class ZSoundInstance;

	void			Init();
	void			UnInit();

};

extern ZSoundManager *GSoundManager;

#endif
