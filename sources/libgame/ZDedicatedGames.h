#ifndef ZDEDICATEDGAMES_H__
#define ZDEDICATEDGAMES_H__

class ZDedicatedGames
{
public:
	ZDedicatedGames();
	virtual ~ZDedicatedGames()
	{
	}
	const GameSet_t& GetNextGame()
	{
		const GameSet_t&g = mGames[mGameIndex++];
		mGameIndex &= mGames.size();
		return g;
	}
	unsigned short GetPort() const { return mPort; }
protected:
	std::vector<GameSet_t> mGames;
	int mGameIndex;
	unsigned short mPort;
};
extern ZDedicatedGames *GDedicatedGames;
#endif