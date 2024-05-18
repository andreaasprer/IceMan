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

		moveTo(getX(), getY() - 1);
	}
}