#include "StudentWorld.h"
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

void StudentWorld::removeDeadGameObjects() {
	for (auto it = actorList.begin(); it != actorList.end();) {
		if (!(*it)->isAlive()) {
			delete* it;
			it = actorList.erase(it); // iterator becomes invalid
		}
		else {
			it++; // increment iterator only when an actor is not deleted
		}
	}
}

void StudentWorld::setDisplayText() {
	int level = getLevel();
	int lives = getLives();
	int health = m_iceman->getHitPoints() * 10;
	int squirts = m_iceman->getSquirtsLeft();
	int gold = m_iceman->getGoldCount();
	int barrelsLeft = numOfBarrels - m_iceman->getBarrelCount();
	int sonar = m_iceman->getSonarCount();
	int score = getScore();

	string s = formatString(level, lives, health, squirts, gold, barrelsLeft, sonar, score);
	setGameStatText(s);
}

string StudentWorld::formatString(int level, int lives, int health, int squirts, int gold, int barrelsLeft, int sonar, int score) {
	std::stringstream sout;
	sout << "Lvl: " << setw(2) << level << "  "
		<< "Lives: " << setw(1) << lives << "  "
		<< "Hlth: " << setw(3) << health << "%  "
		<< "Wtr: " << setw(2) << squirts << "  "
		<< "Gld: " << setw(2) << gold << "  "
		<< "Oil Left: " << setw(2) << barrelsLeft << "  "
		<< "Sonar: " << setw(2) << sonar << "  "
		<< "Scr: " << setw(6) << setfill('0') << score;

	return sout.str();
}

void StudentWorld::clearIce(int x, int y) {
	// flag to see if ice was cleared
	bool cleared = false;

	if (y < 60) { // if object is below top limit of ice field
		// 4x4 box around object to clear
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if ((y + j) < 60) {
					if (m_iceField[x + i][y + j] != nullptr) {
						Ice* ice = m_iceField[x + i][y + j];
						m_iceField[x + i][y + j] = nullptr;
						delete ice;
						cleared = true;
					}
				}
			}
		}
	}

	if (cleared) {
		playSound(SOUND_DIG);
	}
}

bool StudentWorld::inTunnel(int x, int y) {
	if (x > 26 && x < 34 && y > 1) {
		return true;
	}
	return false;
}

void StudentWorld::spawnBoulders(int boulderNum) {
	int currentNum = 0;
	bool canPlace = true;

	while (currentNum < boulderNum) {
		// randomize coordinates boulder
		int x = rand() % 61; // 0 - 60
		int y = 20 + (rand() % 37); // 20 - 56


		if (inTunnel(x, y)) { // check if will be in mine shaft
			canPlace = false;
		}
		else {
			// go through actor list and check if distance of boulder not too close to other actors
			for (Actor* actor : actorList) {
				if (!outsideEuclideanDistance(x, y, actor->getX(), actor->getY(), 6)) {
					canPlace = false;
					break;
				}
			}
		}

		if (canPlace) {
			actorList.push_back(new Boulder(this, x, y));
			clearIce(x, y);
			currentNum++;
		}

		canPlace = true;
	}
}


void StudentWorld::spawnBarrels(int barrelNum) {
	int currentNum = 0;
	bool canPlace = true;

	while (currentNum < barrelNum) {
		int x = rand() % 61; // 0 - 60
		int y = rand() % 57; // 0 - 56

		if (inTunnel(x, y)) {
			canPlace = false;
		}
		else {
			// go through actor list and check if distance of barrel not too close to other actors
			for (Actor* actor : actorList) {
				if (!outsideEuclideanDistance(x, y, actor->getX(), actor->getY(), 6)) {
					canPlace = false;
					break;
				}
			}
		}

		if (canPlace) {
			actorList.push_back(new Barrel(this, x, y));
			currentNum++;
		}

		canPlace = true;
	}
}

void StudentWorld::spawnGoldNuggets(int nuggetNum) {
	int currentNum = 0;
	bool canPlace = true;

	while (currentNum < nuggetNum) {
		int x = rand() % 61; // 0 - 60
		int y = rand() % 57; // 0 - 56

		if (inTunnel(x, y)) {
			canPlace = false;
		}
		else {
			// go through actor list and check if distance of gold not too close to other actors
			for (Actor* actor : actorList) {
				if (!outsideEuclideanDistance(x, y, actor->getX(), actor->getY(), 6)) {
					canPlace = false;
					break;
				}
			}
		}

		if (canPlace) {
			actorList.push_back(new GoldNugget(this, x, y, false));
			currentNum++;
		}

		canPlace = true;
	}
}

// used for spacing objects
bool StudentWorld::outsideEuclideanDistance(int x1, int y1, int x2, int y2, int radius) {
	int distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	if (distance < radius) { return false; }
	return true;
}

// used to see if things are within specified radius
bool StudentWorld::withinEuclideanDistance(int x1, int y1, int x2, int y2, int radius) {
	int distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	if (distance <= radius) { return true; }
	return false;
}

bool StudentWorld::blockedByBoulder(const int x, const int y, Actor::Direction direction) {
	// go through actorList and find the boulders
	for (auto actor : actorList) {
		if (actor->getBlockAbility() == true) {
			// for now, won't depend it on radius of boulder. Will just do a box around boulder.
			switch (direction) {
			case Actor::right:
				if (x + 4 == actor->getX() && abs(y - actor->getY()) < 4) {
					return true;
				}
				break;
			case Actor::left:
				if (x == actor->getX() + 4 && abs(y - actor->getY()) < 4) {
					return true;
				}
				break;
			case Actor::up:
				if (y + 4 == actor->getY() && abs(x - actor->getX()) < 4) {
					return true;
				}
				break;
			case Actor::down:
				if (y == actor->getY() + 4 && abs(x - actor->getX()) < 4) {
					return true;
				}
				break;
			}
		}
	}
	return false;
}

bool StudentWorld::canMoveTo(const int x, const int y, Actor::Direction direction) {
	// check if boulder on the way
	if (blockedByBoulder(x, y, direction)) {
		return false;
	}

	// check ice depending on direction
	switch (direction) {
	case Actor::right:
		for (int i = 0; i < 4; i++) {
			if (m_iceField[x + 4][y + i] != nullptr) {
				return false;
			}
		}
		break;
	case Actor::left:
		for (int i = 0; i < 4; i++) {
			if (m_iceField[x][y + i] != nullptr) {
				return false;
			}
		}
		break;
	case Actor::up:
		for (int i = 0; i < 4; i++) {
			if (m_iceField[x + i][y + 4] != nullptr) {
				return false;
			}
		}
		break;
	case Actor::down:
		for (int i = 0; i < 4; i++) {
			if (m_iceField[x + i][y - 1] != nullptr) {
				return false;
			}
		}
		break;
	}

	return true;
}


void StudentWorld::useSquirt(int x, int y, Actor::Direction direction) {
	actorList.push_back(new Squirt(this, x, y, direction));
	m_iceman->sprayedSquirt();
	playSound(SOUND_PLAYER_SQUIRT);
}



void StudentWorld::dropGold(int x, int y) {
	actorList.push_back(new GoldNugget(this, x, y, true));
	m_iceman->droppedGoldNugget();
	playSound(SOUND_SONAR);
}


void StudentWorld::spawnSonarOrWater(int level) {
	int G = level * 25 + 300;
	int toBeAddedProb = 1 + (rand() % G);
	int lifetime = max(100, 300 - 10 * level);

	// 1 in G chance that a new Water Pool or Sonar Kit should be added
	if (toBeAddedProb == 1) {
		int outOfFive = 1 + (rand() % 5);

		// 1/5 chance for sonar kit
		if (outOfFive == 1) {
			actorList.push_back(new Sonar(this, lifetime));
		}

		// 4/5 chance for water pool
		else {
			bool addedWater = false;
			while (!addedWater) {
				bool clearBox = true;

				// randomize coordinates
				int x = rand() % 61;
				int y = rand() % 57;

				// check if there is ice in the 4x4 box
				for (int i = 0; i < 4; i++) {
					for (int j = 0; j < 4; j++) {
						if ((y + j) < 60) {
							if (m_iceField[x + i][y + j] != nullptr) {
								clearBox = false;
							}
						}
					}
				}

				if (clearBox) {
					actorList.push_back(new WaterPool(this, x, y, lifetime));
					addedWater = true;
				}
			}
		}
	}
}

void StudentWorld::SonarAbility(int x, int y) {
	for (Actor* actor : actorList) {
		if (withinEuclideanDistance(x, y, actor->getX(), actor->getY(), 12)) {
			actor->setVisible(true);
		}
	}
}