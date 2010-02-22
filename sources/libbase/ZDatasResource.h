#ifndef ZDATASRESOURCES_H__
#define ZDATASRESOURCES_H__

#include <list>
#include <vector>
#include <iterator>

template <typename T> struct ZDataResource
{
public:
	typedef struct entry
		{
			entry(ZBaseClass *aInstance, const T& aData)
			{
				mpZInstance = aInstance;
				mpData = aData;
			}

			ZBaseClass *mpZInstance;
			T mpData;
		} entry;

protected:

	ZCriticalSection_t *mCriticalSection;


	std::list<entry> mEntries;

public:
	ZDataResource()
	{
		mCriticalSection = NewCriticalSection();

	}
	virtual ~ZDataResource()
	{
		DestroyCriticalSection(mCriticalSection);
	}

	void Lock()
	{
		LockCriticalSection(mCriticalSection);

	}

	void Unlock()
	{
		UnLockCriticalSection(mCriticalSection);
	}

	// return number of elements pushed in array
	int ListToArray(ZBaseClass **pInstances, T * pDatas, int aMaxNbElements)
	{
		Lock();
		int nbElt = 0;
/*
		std::list<entry>::const_iterator iter;
		iter = mEntries.begin();
		while ((iter != mEntries.end())&&(nbElt<aMaxNbElements))
*/
		typename std::list<entry>::iterator entryIter (mEntries.begin());
		while ((entryIter != mEntries.end())&&(nbElt<aMaxNbElements))
		{
			if (pInstances)
				pInstances[nbElt] = (*entryIter).mpZInstance;

			if (pDatas)
				pDatas[nbElt] = (*entryIter).mpData;

			nbElt++;
			mEntries.erase(entryIter);

			entryIter = mEntries.begin();
		}
		Unlock();
		return nbElt;
	}

	void ArrayToList(ZBaseClass **pInstances, T * pDatas, int aNbEltsInArray)
	{
		Lock();
		if (pInstances)
		{
			for (int i=0;i<aNbEltsInArray;i++)
			{
				mEntries.push_back(entry(pInstances[i], pDatas[i]));
			}
		}
		else
		{
			for (int i=0;i<aNbEltsInArray;i++)
			{
				mEntries.push_back(entry(NULL, pDatas[i]));
			}
		}

		Unlock();
	}

	void SingleToList(ZBaseClass *pInstances, T pDatas)
	{
		Lock();
		mEntries.push_back(entry(pInstances, pDatas));
		Unlock();
	}

	void Clear()
	{
		Lock();
		mEntries.clear();
		Unlock();
	}

	bool empty() const { return mEntries.empty(); }
	unsigned int size() const { return mEntries.size(); }
};

#endif
