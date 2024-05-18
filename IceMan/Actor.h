#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// abstract base class for all of game's actors
class Actor : public GraphObject {
public:
	Actor(int imageID, int startX, int startY, StudentWorld* sw, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: GraphObject(imageID, startX, startY, dir, size, depth) {
		m_studentWorld = sw;
		m_alive = true;
	};
	virtual ~Actor() {}
	virtual void doSomething() = 0;


	StudentWorld* getWorld() { return m_studentWorld; }
	bool isAlive() { return m_alive; }
	void setDead() { m_alive = false; }
	void setBlockAbility(bool flag) { m_blockAbility = flag; }
	bool getBlockAbility() { return m_blockAbility; }

private:
	StudentWorld* m_studentWorld;
	bool m_alive;
	bool m_blockAbility = false; // only actor that can block is boulder
};

// base class for Iceman and NPCs (protestors)
class Character : public Actor {
public:
	Character(int imageID, int startX, int startY, StudentWorld* sw, int hitPoints, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(imageID, startX, startY, sw, dir, size, depth) {
		setVisible(true);
		m_hitPoints = hitPoints;
	}
	virtual ~Character() {}


	void setHitPoints(int health) { m_hitPoints = health; }
	int getHitPoints() { return m_hitPoints; }

private:
	int m_hitPoints = 0;
};


class Iceman : public Character {
public:
	Iceman(StudentWorld* sw, int health) : Character(IID_PLAYER, 30, 60, sw, health, right, 1.0, 0) {};
	virtual ~Iceman() {}
	virtual void doSomething() override;
private:
	int m_waterLevel = 5;
	int m_sonar = 1;
	int m_goldNuggets = 0;
};


class Ice : public Actor {
public:
	Ice(StudentWorld* sw, int x, int y) : Actor(IID_ICE, x, y, sw, right, 0.25, 3) { setVisible(true); };
	virtual ~Ice() {}
	virtual void doSomething() override {};

};

class Boulder : public Actor {
public:
	Boulder(StudentWorld* sw, int x, int y) : Actor(IID_BOULDER, x, y, sw, down, 1.0, 1) {
		setVisible(true);
		setBlockAbility(true);
		currentState = stable;
	};
	virtual ~Boulder() {}
	virtual void doSomething() override;
private:
	enum BoulderState { stable, waiting, falling };
	BoulderState currentState;
	int waitTime = 30;
};


#endif // ACTOR_H_
