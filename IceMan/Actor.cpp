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