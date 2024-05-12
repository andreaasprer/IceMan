#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"


class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}
	~StudentWorld() {
		delete m_iceman;
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 60; y++) {
				delete m_iceField[x][y];
			}
		}
	}

	virtual int init()
	{
		// initialize iceman
		m_iceman = new Iceman(this, 10);

		// set up the ice field
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 60; y++) {
				if (x >= 30 && x < 34 && y > 3) { // dont put ice for starting middle tunnel
					m_iceField[x][y] = nullptr;
				}
				else
					m_iceField[x][y] = new Ice(this, x, y);
			}
		}

		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		m_iceman->doSomething();

		if (m_iceman->isAlive() == false) {
			decLives();
			return GWSTATUS_PLAYER_DIED;
		}

		return GWSTATUS_CONTINUE_GAME;
	}

	virtual void cleanUp()
	{
		delete m_iceman;
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 60; y++) {
				delete m_iceField[x][y];
			}
		}
	}

	void clearIce(int x, int y);


private:
	Iceman* m_iceman;
	Ice* m_iceField[64][64];
};

#endif // STUDENTWORLD_H_
