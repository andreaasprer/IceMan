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

private:
	StudentWorld* m_studentWorld;
	bool m_alive;
};

// base class for Iceman and NPCs (protestors)
class Character : public Actor {
public:
	Character(int imageID, int startX, int startY, StudentWorld* sw, int health, Direction dir = right, double size = 1.0, unsigned int depth = 0)
		: Actor(imageID, startX, startY, sw, dir, size, depth) {
		setVisible(true);
		m_hitPoints = health;
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
};


class Ice : public Actor {
public:
	Ice(StudentWorld* sw, int x, int y) : Actor(IID_ICE, x, y, sw, right, 0.25, 3) { setVisible(true); };
	virtual ~Ice() {}
	virtual void doSomething() override {};

};


#endif // ACTOR_H_
