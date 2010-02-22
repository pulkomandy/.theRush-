

/*
typedef struct TimedKeyStruct_t
{
	TimedKeyStruct_t()
	{
		mTime = 0;
	}
	TimedKeyStruct_t(const RakNetTime &aTime, const keysStruct &ks)
	{
		mTime = aTime;
		mKS = ks;
	}

	RakNetTime mTime;
	keysStruct mKS;
} TimedKeyStruct_t;


typedef struct TimedShipSync_t
{
	TimedShipSync_t()
	{
		mTime = 0;
	}
	TimedShipSync_t(const RakNetTime &aTime, const ShipSync_t &ss)
	{
		mTime = aTime;
		mSync = ss;
	}

	RakNetTime mTime;
	ShipSync_t mSync;
} TimedShipSync_t;
*/
/*
typedef struct ShipKeys_t
{
	TimedKeyStruct_t mInitialKS;
	TimedShipSync_t mTimedSync;
	std::vector<TimedKeyStruct_t> mKS;

} ShipKeys_t;



int SortTimedKeysStruct(const void* a, const void* b)
{
	TimedKeyStruct_t *lhs = (TimedKeyStruct_t *)a;
	TimedKeyStruct_t *rhs = (TimedKeyStruct_t *)b;

	if ( lhs->mTime < rhs->mTime)
		return -1;
	if ( lhs->mTime > rhs->mTime)
		return 1;

	return 0;
}

std::map<SystemAddress, ShipKeys_t> mShipKeys;

void ReceiveKeyStruct(const SystemAddress& addr, const keysStruct& ks, const RakNetTime& aTime)
{
	ShipKeys_t& sk = mShipKeys[addr];

	// received KS is before last shipKey initial time
	if (aTime < sk.mInitialKS.mTime)
		return;
	else if (aTime < sk.mTimedSync.mTime)
	{
		// replace initial
		sk.mInitialKS = TimedKeyStruct_t(aTime, ks);
	}
	else 
	{
		// add new KS
		sk.mKS.push_back(TimedKeyStruct_t(aTime, ks));
	}
}


void ReceiveTimedSync(const SystemAddress& addr, const ShipSync_t& ss, const RakNetTime& aTime)
{
	ShipKeys_t& sk = mShipKeys[addr];

	// can't replace sync with an older one
	if (aTime < sk.mTimedSync.mTime)
		return;
	// set timed sync

	sk.mTimedSync = TimedShipSync_t(aTime, ss);

	// sort keys struct and get newest one
	int mksCount = sk.mKS.size();
	qsort(&sk.mKS[0], mksCount, sizeof(TimedKeyStruct_t), SortTimedKeysStruct);

	sk.mInitialKS = sk.mKS[mksCount-1];

	// clear KS
	sk.mKS.clear();

}

*/

void TestInterpolator()
{
	/*

	keysStruct ks;
	ShipSync_t ss;

	RakNetTime ts = RakNet::GetTime();
	
	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts);
	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts-4);
	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts+3);
	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts+1);
	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts+2);
	ReceiveTimedSync(UNASSIGNED_SYSTEM_ADDRESS, ss, ts+8);

	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts+14);
	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts+13);
	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts+11);
	ReceiveKeyStruct(UNASSIGNED_SYSTEM_ADDRESS, ks, ts+12);
	*/
}

/*
A l'init:
on push 1 SS et 1 KS vide
qstart = qend = time0
Pour chaque KS, on stocke aussi 1 SS
A la frame:

On récupère tous les SS et KS. On les order par timestate et par vaisseau.
Quand on recoie 1 SS ou 1 KS, on tag en dirty tous les SS et KS suivants.
La recherche du temps de base de ressimulation se fait ainsi:
On cherche depuis tous les ships, le temps minimal qui n'est pas dirty.
--------|ooo......... Légende: - KS/SS clean
----|ooooooo.........          | SS/KS clean le plus vieux
-----------|.........          o SS/KS dirty (ou pas de SS/KS)
---|oooooooo.........          . temps à simuler sans SS/KS
   ^ celui-ci nomé time1
           ^ time2 : dernier temps avec SS ou KS
                    ^ time3 : temps du frame


   On déliminite 2 plages de temps:
de time1 a time2 :
on ressimule par petite plage entre 2 KS/SS. on tag en 'clean' les SS et KS.
pour chaque KS, on stocke un SS.
pb: comment etre sur ke pour le + vieux SS clean, on a 1 équivalent chez les autres ships.
	-> pour chaque SS/KS, on en crée 1 chez les autres ships?

de time2 a time3:
on simule tout d'un bloc en live.

Cumuler le temps de simulation. Faire le ratio temps réel passé, temps simulé.

pb de data:
avoir 1 structure dans laquelle on peut pusher et orderer par temps.
au bout de N secodes, supprimer les entrées les + vieilles
*/