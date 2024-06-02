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
#include <iomanip>
#include <sstream>
#include <queue>
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
		numOfBarrels = min(2 + currentLevel, 21);    // number of oil barrels in each level

		int tickToWaitBetweenMoves = max(0, 3 - currentLevel / 4);

		// initialize iceman
		m_iceman = new Iceman(this);

		// set up the ice field
		for (int x = 0; x < 64; x++) {
			for (int y = 0; y < 64; y++) {
				if (x >= 30 && x < 34 && y > 3) { // dont put ice for starting middle tunnel
					m_iceField[x][y] = nullptr;
				}
				else if (y > 59) {				  // so the squirt gun can spray at that top level	
					m_iceField[x][y] = nullptr;
				}

				else
					m_iceField[x][y] = new Ice(this, x, y);
			}
		}

		spawnBoulders(B);
		spawnGoldNuggets(G);
		spawnBarrels(numOfBarrels);

		actorList.push_back(new Protester(this, tickToWaitBetweenMoves));

		return GWSTATUS_CONTINUE_GAME;
	}

	virtual int move()
	{
		setDisplayText();

		spawnSonarOrWater(getLevel());

		m_iceman->doSomething();

		for (Actor* actor : actorList) {
			if (actor->isAlive()) {
				actor->doSomething();
			}
		}

		removeDeadGameObjects();

		if (numOfBarrels == m_iceman->getBarrelCount()) { // found all barrels
			playSound(SOUND_FINISHED_LEVEL);
			return GWSTATUS_FINISHED_LEVEL;
		}

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
	Ice* getIce(int x, int y) const { return m_iceField[x][y]; }
	list<Actor*> getActorList() const { return actorList; }
	Iceman* getPlayer() const { return m_iceman; }
	void useSquirt(int x, int y, Actor::Direction direction);
	void dropGold(int x, int y);
	void SonarAbility(int x, int y);
	bool outsideEuclideanDistance(int x1, int y1, int x2, int y2, int Radius);
	bool withinEuclideanDistance(int x1, int y1, int x2, int y2, int radius);
	bool blockedByBoulder(const int x, const int y, Actor::Direction direction);
	bool canMoveTo(const int x, const int y, Actor::Direction direction);
	bool isFacingIceMan(const int x, const int y, Actor::Direction direction);

	bool isNearIceMan(Actor* a, int radius);

	bool lineOfSightToIceMan(Actor* a, GraphObject::Direction& dirToPlayer);
	void findShortestPath(int startX, int startY, int endX, int endY);
	GraphObject::Direction dirToShortestPath(int startX, int startY, int endX, int endY);
private:
	Iceman* m_iceman;
	Ice* m_iceField[64][64];
	list<Actor*> actorList;
	int shortestPath[64][64];
	int numOfBarrels = 0;
	int sonarWaterChance = 0;

	void setDisplayText();
	string formatString(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score);
	void removeDeadGameObjects();
	void spawnBarrels(int barrelNum);
	void spawnBoulders(int boulderNum);
	void spawnGoldNuggets(int nuggetNum);
	void spawnSonarOrWater(int level);
	bool inTunnel(int x, int y);
	

	// for bfs algorithm
	struct Vertex {
		Vertex(int x, int y) : m_x(x), m_y(y) { };
		int m_x;
		int m_y;
	};
};

#endif // STUDENTWORLD_H_
