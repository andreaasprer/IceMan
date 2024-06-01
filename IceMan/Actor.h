#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <algorithm>
class StudentWorld;

// abstract base class for all of game's actors
class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, StudentWorld* sw, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: GraphObject(imageID, startX, startY, dir, size, depth) {
		m_studentWorld = sw;
		m_alive = true;
	}
	virtual ~Actor() {}
	virtual void doSomething() = 0;


	StudentWorld* getWorld() const { return m_studentWorld; }
	bool isAlive() const { return m_alive; }
	void setDead() { m_alive = false; }
	void setBlockAbility(bool flag) { m_blockAbility = flag; }
	bool getBlockAbility() const { return m_blockAbility; }

private:
	StudentWorld* m_studentWorld;
	bool m_alive;
	bool m_blockAbility = false; // only actor that can block is boulder
};

// base class for Iceman and NPCs (protestors)
class Character : public Actor {
public:
	Character(int imageID, int startX, int startY, StudentWorld* sw, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(imageID, startX, startY, sw, dir, size, depth) {
		setVisible(true);
	}
	virtual ~Character() {}
	virtual bool annoy(unsigned int amt) = 0;


	void setHitPoints(int hitPoints) { m_hitPoints = hitPoints; }
	int getHitPoints() const { return m_hitPoints; }
	void decrementHitPoints(unsigned int amt) { m_hitPoints -= amt; }

private:
	int m_hitPoints = 0;
};

// base class for Sonar Kit and Water Pool
class TemporaryGoodies : public Actor {
public:
	TemporaryGoodies(int imageID, int startX, int startY, StudentWorld* sw, int lifetime, Direction dir = right, double size = 1.0, unsigned int depth = 2)
		: Actor(imageID, startX, startY, sw, dir, size, depth) {
		setVisible(true);
		setTickLifetime(lifetime);
	}
	virtual ~TemporaryGoodies() {};

	int getTickLifetime() const { return tickLifetime; }
	void tickDecrement() { tickLifetime--; }
private:
	int tickLifetime;
	void setTickLifetime(int lifetime) { tickLifetime = lifetime; }
};

class Iceman : public Character {
public:
	Iceman(StudentWorld* sw) : Character(IID_PLAYER, 30, 60, sw, right, 1.0, 0) {
		setVisible(true);
		setHitPoints(10);
	};
	virtual ~Iceman() {}
	virtual void doSomething() override;
	virtual bool annoy(unsigned int amt) override;

	int getSquirtsLeft() const { return m_waterLevel; }
	int getGoldCount() const { return m_goldNuggets; }
	int getSonarCount() const { return m_sonar; }
	int getBarrelCount() const { return m_barrels; }
	void sprayedSquirt() { m_waterLevel--; }
	void foundBarrel() { m_barrels++; }
	void foundGoldNugget() { m_goldNuggets++; }
	void droppedGoldNugget() { m_goldNuggets--; }
	void foundSonar() { m_sonar++; }
	void foundWaterPool() { m_waterLevel += 5; }
	void usedSonar() { m_sonar--; }
private:
	int m_waterLevel = 50;
	int m_sonar = 1;
	int m_goldNuggets = 0;
	int m_barrels = 0;
};



class Protester : public Character {
public:
	Protester(StudentWorld* sw, int tickWait) : Character(IID_PROTESTER, 60, 60, sw, left, 1.0, 0) {
		setVisible(true);
		setHitPoints(5);
		waitTime = tickWait;
	}
	virtual ~Protester() {}
	virtual void doSomething() override;
	virtual bool annoy(unsigned int amt) override { return true; }

private:
	int numSquaresToMoveInCurrentDirection = 0;
	bool leaveTheOilFieldState = false;
	int waitTime = 0;
	int yellWaitTime = 0;

	void resetYell() { yellWaitTime = 15; }
};

class Ice : public Actor {
public:
	Ice(StudentWorld* sw, int x, int y) : Actor(IID_ICE, x, y, sw, right, 0.25, 3) { setVisible(true); }
	virtual ~Ice() {}
	virtual void doSomething() override {};
};

class Boulder : public Actor {
public:
	Boulder(StudentWorld* sw, int x, int y) : Actor(IID_BOULDER, x, y, sw, down, 1.0, 1) {
		setVisible(true);
		setBlockAbility(true);
		currentState = stable;
	}
	virtual ~Boulder() {}
	virtual void doSomething() override;
private:
	enum BoulderState { stable, waiting, falling };
	BoulderState currentState;
	int waitTime = 30;
	bool hitsCharacter(StudentWorld* sw);
};

class Squirt : public Actor {
public:
	Squirt(StudentWorld* sw, int x, int y, Direction dir) : Actor(IID_WATER_SPURT, x, y, sw, dir, 1.0, 1) {
		setVisible(true);
	}
	virtual ~Squirt() {}
	virtual void doSomething() override;

private:
	int travelDistance = 8;
};


class Barrel : public Actor {
public:
	Barrel(StudentWorld* sw, int x, int y) : Actor(IID_BARREL, x, y, sw, right, 1.0, 2) {
		setVisible(false);
	}
	virtual ~Barrel() {}
	virtual void doSomething() override;
};

class GoldNugget : public Actor {
public:
	GoldNugget(StudentWorld* sw, int x, int y, bool droppedByIceman) : Actor(IID_GOLD, x, y, sw, right, 1.0, 2) {
		if (!droppedByIceman) {
			setVisible(false);
			currentState = permanent;
		}
		else if (droppedByIceman) {
			setVisible(true);
			currentState = temporary;
		}
	}
	virtual ~GoldNugget() {}
	virtual void doSomething() override;


private:
	enum NuggetState { permanent, temporary, bait }; // bait is when protestor can pick it up
	NuggetState currentState;
	int waitTime = 100;
};

class Sonar : public TemporaryGoodies {
public:
	Sonar(StudentWorld* sw, int lifetime) : TemporaryGoodies(IID_SONAR, 0, 60, sw, lifetime, right, 1.0, 2) {}
	virtual ~Sonar() {};
	virtual void doSomething() override;
};

class WaterPool : public TemporaryGoodies {
public:
	WaterPool(StudentWorld* sw, int x, int y, int lifetime) : TemporaryGoodies(IID_WATER_POOL, x, y, sw, lifetime, right, 1.0, 2) {}
	virtual ~WaterPool() {}
	virtual void doSomething() override;
};

#endif // ACTOR_H_
