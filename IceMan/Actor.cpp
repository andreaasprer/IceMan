#include "Actor.h"
#include "StudentWorld.h"

void Character::takeStep(const int x, const int y, Direction dir) {
	switch (dir) {
	case up:
		moveTo(getX(), getY() + 1);
		break;
	case down:
		moveTo(getX(), getY() - 1);
		break;
	case right:
		moveTo(getX() + 1, getY());
		break;
	case left:
		moveTo(getX() - 1, getY());
		break;
	}
}

void Iceman::doSomething() {
	if (!isAlive()) {
		return;
	}

	getWorld()->clearIce(getX(), getY());

	// check if human player has pressed a key
	int ch;
	if (getWorld()->getKey(ch) == true) {
		// user hit a key this tick!
		switch (ch)
		{
		case KEY_PRESS_ESCAPE:
			setDead();
			break;

		case KEY_PRESS_TAB:
			if (m_goldNuggets) {
				getWorld()->dropGold(getX(), getY());
			}
			break;
		case 'Z':
		case 'z':
			if (getSonarCount()) {
				usedSonar();
				getWorld()->SonarAbility(getX(), getY());
				getWorld()->playSound(SOUND_SONAR);
			}
			break;

		case KEY_PRESS_SPACE:
			if (m_waterLevel) {
				if (getDirection() == right && getX() <= 60) {
					getWorld()->useSquirt(getX() + 3, getY(), right);
				}
				else if (getDirection() == left && getX() >= 0) {
					getWorld()->useSquirt(getX() - 3, getY(), getDirection());
				}
				else if (getDirection() == up && getY() <= 60) {
					getWorld()->useSquirt(getX(), getY() + 3, getDirection());
				}
				else if (getDirection() == down && getY() >= 0) {
					getWorld()->useSquirt(getX(), getY() - 3, getDirection());
				}
			}
			break;

		case KEY_PRESS_RIGHT:
			if (getDirection() == right && getX() == 60) { // iceman is at right border
				moveTo(getX(), getY());
			}
			else if (getDirection() == right) {
				if (!(getWorld()->blockedByBoulder(getX(), getY(), right)))
					moveTo(getX() + 1, getY());
			}
			else {
				setDirection(right);
			}
			break;

		case KEY_PRESS_LEFT:
			if (getDirection() == left && getX() == 0) { // iceman is at left border
				moveTo(getX(), getY());
			}
			else if (getDirection() == left) {
				if (!(getWorld()->blockedByBoulder(getX(), getY(), left)))
					moveTo(getX() - 1, getY());
			}
			else {
				setDirection(left);
			}
			break;

		case KEY_PRESS_UP:
			if (getDirection() == up && getY() == 60) { // iceman is at top border
				moveTo(getX(), getY());
			}
			else if (getDirection() == up) {
				if (!(getWorld()->blockedByBoulder(getX(), getY(), up)))
					moveTo(getX(), getY() + 1);
			}
			else {
				setDirection(up);
			}
			break;

		case KEY_PRESS_DOWN:
			if (getDirection() == down && getY() == 0) { // iceman is at bottom border
				moveTo(getX(), getY());
			}
			else if (getDirection() == down) {
				if (!(getWorld()->blockedByBoulder(getX(), getY(), down)))
					moveTo(getX(), getY() - 1);
			}
			else {
				setDirection(down);
			}
			break;
		}
	}
}

bool Iceman::annoy(unsigned int amt) {
	decrementHitPoints(amt);
	// iceman dead
	if (getHitPoints() <= 0) { return true; } // true to indicate iceman dies
	return false; // false to indicate iceman still alive
}


void Boulder::doSomething() {
	if (!isAlive()) {
		return;
	}

	if (currentState == stable) {
		int iceGoneNum = 0;
		// count number of ice gone below
		for (int i = 0; i < 4; i++) {
			if (getWorld()->getIce(getX() + i, getY() - 1) == nullptr) {
				iceGoneNum++;
			}
		}

		if (iceGoneNum == 4) { // no ice below
			currentState = waiting;
		}
	}

	if (currentState == waiting) {
		if (waitTime == 0) {
			currentState = falling;
			getWorld()->playSound(SOUND_FALLING_ROCK);
		}
		waitTime--;
	}

	if (currentState == falling) {
		if (getY() == 0) { // hits bottom
			setDead();
		}
		for (int i = 0; i < 4; i++) {
			if (getWorld()->getIce(getX() + i, getY() - 1) != nullptr) { // hits ice
				setDead();
			}
		}
		for (Actor* actor : getWorld()->getActorList()) { // hits top of boulder
			if (actor->getBlockAbility() == true) {
				// check if current boulder is on top of a boulder
				if (abs(getX() - actor->getX()) < 4 && getY() == actor->getY() + 4) {
					setDead();
				}
			}
		}

		hitsCharacter(getWorld());

		moveTo(getX(), getY() - 1);
	}
}

void Boulder::hitsCharacter(StudentWorld* sw) {
	// hits iceman
	if (abs(getX() - sw->getPlayer()->getX()) < 4 && getY() == sw->getPlayer()->getY() + 4) {
		sw->getPlayer()->decrementHitPoints(100);
		sw->getPlayer()->setDead();
		sw->playSound(SOUND_PLAYER_GIVE_UP);
	}

	// hits protester
	for (Actor* actor : sw->getActorList()) {
		if (actor->getIfProt()) { // only protesters
			// downcast to protester only to be able to see if it isn't leaving the oil field
			Protester* p = dynamic_cast<Protester*>(actor);
			if (p->getAnnoyedState() == false) {
				if (abs(getX() - p->getX()) < 4 && getY() == p->getY() + 4) {
					p->annoy(100);
					sw->increaseScore(500);
				}
			}
		}
	}
}

void Barrel::doSomething() {
	if (!isAlive()) {
		return;
	}

	int iceManX = getWorld()->getPlayer()->getX();
	int iceManY = getWorld()->getPlayer()->getY();

	// iceman 4 units away
	if (!isVisible() && getWorld()->withinEuclideanDistance(iceManX, iceManY, getX(), getY(), 4)) {
		setVisible(true);
		return;
	}

	// 3 units away
	if (getWorld()->withinEuclideanDistance(iceManX, iceManY, getX(), getY(), 3)) {
		setDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->getPlayer()->foundBarrel();
	}
}

void GoldNugget::doSomething() {
	if (!isAlive()) {
		return;
	}

	int iceManX = getWorld()->getPlayer()->getX();
	int iceManY = getWorld()->getPlayer()->getY();

	// iceman 4 units away
	if (!isVisible() && getWorld()->withinEuclideanDistance(iceManX, iceManY, getX(), getY(), 4)) {
		setVisible(true);
		return;
	}

	// 3 units away and permanent state
	if (currentState == permanent && getWorld()->withinEuclideanDistance(iceManX, iceManY, getX(), getY(), 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->getPlayer()->foundGoldNugget();
	}

	if (currentState == temporary) {
		if (waitTime == 0) {
			setDead();
		}
		waitTime--;
	}
}


void Sonar::doSomething() {
	if (!isAlive()) {
		return;
	}

	int iceManX = getWorld()->getPlayer()->getX();
	int iceManY = getWorld()->getPlayer()->getY();

	// 3 units away
	if (getWorld()->withinEuclideanDistance(iceManX, iceManY, getX(), getY(), 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->foundSonar();
		getWorld()->increaseScore(75);
	}

	// tick lifetime has elapsed
	if (getTickLifetime() == 0) {
		setDead();
	}

	// decrement the lifetime every tick the sonar is alive
	tickDecrement();
}

void WaterPool::doSomething() {
	if (!isAlive()) {
		return;
	}

	int iceManX = getWorld()->getPlayer()->getX();
	int iceManY = getWorld()->getPlayer()->getY();

	// 3 units away
	if (getWorld()->withinEuclideanDistance(iceManX, iceManY, getX(), getY(), 3)) {
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->foundWaterPool();
		getWorld()->increaseScore(100);
	}

	// tick lifetime has elapsed
	if (getTickLifetime() == 0) {
		setDead();
	}

	tickDecrement();
}


void Squirt::doSomething() {
	if (!isAlive()) {
		return;
	}

	// check if its hits any of the protesters
	hitsProtester(getWorld());

	// traveled all 4 squares
	if (travelDistance == 0) {
		setDead();
	}

	if (travelDistance > 0) {
		switch (getDirection()) {
		case right:
			if (!getWorld()->canMoveTo(getX(), getY(), right)) {
				setDead();
			}
			moveTo(getX() + 1, getY());
			break;
		case left:
			if (!getWorld()->canMoveTo(getX(), getY(), left)) {
				setDead();
			}
			moveTo(getX() - 1, getY());
			break;
		case up:
			if (!getWorld()->canMoveTo(getX(), getY(), up)) {
				setDead();
			}
			moveTo(getX(), getY() + 1);
			break;
		case down:
			if (!getWorld()->canMoveTo(getX(), getY(), down)) {
				setDead();
			}
			moveTo(getX(), getY() - 1);
			break;
		}
		travelDistance--;
	}
}

void Squirt::hitsProtester(StudentWorld* sw) {
	for (Actor* actor : sw->getActorList()) {
		if (actor->getIfProt()) { // find protester in list
			Protester* p = dynamic_cast<Protester*>(actor);
			if (p->getAnnoyedState() == false) {
				if (getWorld()->withinEuclideanDistance(p->getX(), p->getY(), getX(), getY(), 3)) { // hit protester
					getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
					p->annoy(2);
					setDead(); // get rid of squirt once it hits protester
				}
			}
		}
	}
}


void Protester::doSomething() {
	int iceManX = getWorld()->getPlayer()->getX();
	int iceManY = getWorld()->getPlayer()->getY();

	if (!isAlive()) {
		return;
	}

	if (waitTime > 0) { // in rest state
		waitTime--;
		return;
	}

	if (yellWaitTime > 0) { // decrement yell wait time 
		yellWaitTime--;
	}

	turnWaitTime--;
	resetWait();

	if (leaveTheOilFieldState) {
		if (getX() == 60 && getY() == 60) {
			setDead();
		}


		Direction dir = getWorld()->dirToShortestPath(60, 60, getX(), getY());
		cout << dir << endl;
		setDirection(dir);
		takeStep(getX(), getY(), dir);
		return;
	}

	else {
		// is at yelling distance and facing iceman
		if (getWorld()->isNearIceMan(this, 4) && getWorld()->isFacingIceMan(getX(), getY(), getDirection())) {
			if (yellWaitTime == 0) {
				if (getWorld()->getPlayer()->annoy(2)) { // iceman died
					getWorld()->getPlayer()->setDead();
					getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
				}
				getWorld()->playSound(SOUND_PROTESTER_YELL);
				resetYell();
				return;
			}
		}

		// move to iceman if in direct line of sight with no obstructions
		Direction dirToPlayer = none;
		if (getWorld()->lineOfSightToIceMan(this, dirToPlayer)) {
			setDirection(dirToPlayer);
			takeStep(getX(), getY(), getDirection());
			numSquaresToMoveInCurrentDirection = 0;
			return;
		}


		// generate new direction when move distance is at zero
		if (numSquaresToMoveInCurrentDirection <= 0) {
			// generate a random direction from up, down, right, left (avoid none)
			Direction randDir = randomizeDirection();

			// check if it can go that direction:
			while (!getWorld()->canMoveTo(getX(), getY(), randDir)) {
				randDir = randomizeDirection();
			}

			// update protester's direction and distance value
			setDirection(randDir);
			randomizeMoveNum();
		}

		// try to make a turn
		if (turnWaitTime <= 0 && canSideTurn(getX(), getY(), getDirection())) {
			switch (getDirection()) {
			case up:
			case down:
				if (getWorld()->canMoveTo(getX(), getY(), right) && getWorld()->canMoveTo(getX(), getY(), left)) { // can turn to both
					// randomize using enum values (left = 3 and right = 4)
					Direction dir = static_cast<Direction>(3 + (rand() % 2));
					setDirection(dir);
				}
				else if (getWorld()->canMoveTo(getX(), getY(), right)) {
					setDirection(right);
				}
				else {
					setDirection(left);
				}
				break;
			case right:
			case left:
				if (getWorld()->canMoveTo(getX(), getY(), up) && getWorld()->canMoveTo(getX(), getY(), down)) { // can turn to both
					// randomize using enum values (up = 1 and down = 2)
					Direction dir = static_cast<Direction>(1 + (rand() % 2));

				}
				else if (getWorld()->canMoveTo(getX(), getY(), up)) {
					setDirection(up);
				}
				else {
					setDirection(down);
				}
				break;
			}
			randomizeMoveNum();
			resetTurn();
		}

		// try to take a step
		if (getWorld()->canMoveTo(getX(), getY(), getDirection())) {
			takeStep(getX(), getY(), getDirection());
			numSquaresToMoveInCurrentDirection--;
		}

		// reset distance so it will force to get a new direction
		else if (!getWorld()->canMoveTo(getX(), getY(), getDirection())) {
			numSquaresToMoveInCurrentDirection = 0;
		}
	}
	
}

GraphObject::Direction Protester::randomizeDirection() {
	// generate a random direction from up, down, right, left (avoid none)
	Direction random = static_cast<Direction>(1 + (rand() % right));
	return random;
}


bool Protester::canSideTurn(const int x, const int y, Direction direction) {
	// perpendicular turns: 
	// if looking up or down -> can turn right  left
	// right or left -> up or down
	if (direction == right || direction == left) {
		if (getWorld()->canMoveTo(x, y, up) || getWorld()->canMoveTo(x, y, down)) {
			return true;
		}
	}
	else if (direction == up || direction == down) {
		if (getWorld()->canMoveTo(x, y, right) || getWorld()->canMoveTo(x, y, left)) {
			return true;
		}
	}
	return false;
}

bool Protester::annoy(unsigned int amt) {
	decrementHitPoints(amt);

	if (getHitPoints() <= 0) {
		leaveTheOilFieldState = true;
		getWorld()->increaseScore(100);
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		return true;
	}

	return false;
}