#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include "Actor.h"
#include <cmath>
#include <iostream>
#include <list>
#include<cstdlib>
using namespace std;


class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir)
	{
	}
	~StudentWorld() {
		cleanUp();
	}

	virtual int init()
	{
		int currentLevel = int(getLevel());
		int B = min(currentLevel / 2 + 2, 9); // number of boulders in each level
		int G = max(5 - currentLevel / 2, 2); // number of gold in each level
		int L = min(2 + currentLevel, 21);    // number of oil barrels in each level


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

		spawnBoulders(B);

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
		for (Actor* actor : actorList) {
			Actor* temp = actor;
			actor = nullptr;
			delete temp;
		}
		actorList.clear();
	}

	void clearIce(int x, int y);

	void spawnBoulders(int boulderNum);
	bool validEuclideanDistance(int x1, int y1, int x2, int y2);

private:
	Iceman* m_iceman;
	Ice* m_iceField[64][64];
	list<Actor*> actorList;
};

#endif // STUDENTWORLD_H_
