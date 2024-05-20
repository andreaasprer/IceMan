#include "Actor.h"
#include "StudentWorld.h"


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
		if (hitsCharacter(getWorld())) {
			getWorld()->getPlayer()->setDead();
			getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
		}

		moveTo(getX(), getY() - 1);
	}
}

bool Boulder::hitsCharacter(StudentWorld* sw) {
	if (abs(getX() - sw->getPlayer()->getX()) < 4 && getY() == sw->getPlayer()->getY() + 4) {
		return true;
	}
	return false;
}

void Barrel::doSomething() {
	if (!isAlive()) {
		return;
	}

	int iceManX = getWorld()->getPlayer()->getX();
	int iceManY = getWorld()->getPlayer()->getY();

	// iceman 4 units away
	if (!isVisible() && !getWorld()->validEuclideanDistance(iceManX, iceManY, getX(), getY(), 5)) {
		setVisible(true);
		return;
	}

	// 3 units away
	if (!getWorld()->validEuclideanDistance(iceManX, iceManY, getX(), getY(), 4)) {
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
	if (!isVisible() && !getWorld()->validEuclideanDistance(iceManX, iceManY, getX(), getY(), 5)) {
		setVisible(true);
		return;
	}

	// 3 units away and permanent state
	if (currentState == permanent && !getWorld()->validEuclideanDistance(iceManX, iceManY, getX(), getY(), 4)) {
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
	if (!getWorld()->validEuclideanDistance(iceManX, iceManY, getX(), getY(), 4)) {
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
	if (!getWorld()->validEuclideanDistance(iceManX, iceManY, getX(), getY(), 4)) {
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