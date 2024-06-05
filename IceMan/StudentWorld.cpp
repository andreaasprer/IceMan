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

void StudentWorld::spawnProtesters(int level) {
	int probabilityOfHardcore = min(90, level * 10 + 30);
	int chance = rand() % 100 + 1; // 1 - 100 chance


	if (chance < probabilityOfHardcore) {
		actorList.push_back(new HardCoreProtester(this, getLevel(), IID_HARD_CORE_PROTESTER, 20));
	}
	else {
		actorList.push_back(new Protester(this, getLevel(), IID_PROTESTER, 5));
	}
}

bool StudentWorld::canSpawnProtester(int maxNum) {
	int protsNum = 0;
	// count how many protesters there currently are
	for (Actor* actor : actorList) {
		if (actor->getIfProt() == true) {
			protsNum++;
		}
	}

	if (protsNum < maxNum) {
		return true;
	}
	return false;
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


bool StudentWorld::isNearIceMan(Actor* a, int radius) {
	if (withinEuclideanDistance(a->getX(), a->getY(), getPlayer()->getX(), getPlayer()->getY(), radius)) {
		return true;
	}
	return false;
}

bool StudentWorld::blockedByBoulder(const int x, const int y, Actor::Direction direction) {
	// go through actorList and find the boulders
	for (auto actor : actorList) {
		if (actor->getBlockAbility() == true) {
			int boulderX = actor->getX();
			int boulderY = actor->getY();

			// for now, won't depend it on radius of boulder. Will just do a box around and within boulder.
			switch (direction) {
			case Actor::right:
				if (x + 4 >= boulderX && x < boulderX + 4 && abs(y - boulderY) < 4) {
					return true;
				}
				break;
			case Actor::left:
				if (x <= boulderX + 4 && x > boulderX && abs(y - boulderY) < 4) {
					return true;
				}
				break;
			case Actor::up:
				if (y + 4 >= boulderY && y < boulderY + 4 && abs(x - boulderX) < 4) {
					return true;
				}
				break;
			case Actor::down:
				if (y <= boulderY + 4 && y > boulderY && abs(x - boulderX) < 4) {
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

	// bounds checking and check if ice overlaps squirt object
	switch (direction) {
	case Actor::right:
		for (int i = 0; i <= 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (m_iceField[x + i][y + j] != nullptr || x + i >= 64) {
					return false;
				}
			} 
		}
		break;
	case Actor::left:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				if (m_iceField[x + i - 1][y + j] != nullptr || x - 1 < 0) {
					return false;
				}
			}
		}
		break;
	case Actor::up:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j <= 4; j++) {
				if (m_iceField[x + i][y + j] != nullptr || y + j >= 64) {
					return false;
				}
			}
		}
		break;
	case Actor::down:
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j <= 4; j++) {
				if (m_iceField[x + i][y + j - 1] != nullptr || y - 1 < 0) {
					return false;
				}
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

bool StudentWorld::isFacingIceMan(const int x, const int y, Actor::Direction direction) {
	int iceManX = m_iceman->getX();
	int iceManY = m_iceman->getY();
	// iceman to protester's right. Should be facing right
	if (iceManX > x && Actor::Direction::right == direction
		&& iceManY >= y - 4 && iceManY <= y + 4) {
		return true;
	}

	// left
	else if (iceManX < x && Actor::Direction::left == direction
		&& iceManY >= y - 4 && iceManY <= y + 4) {
		return true;
	}

	// up
	else if (iceManY > y && Actor::Direction::up == direction
		&& iceManX >= x - 4 && iceManX <= x + 4) {
		return true;
	}

	// down
	else if (iceManY < y && Actor::Direction::down == direction
		&& iceManX >= x - 4 && iceManX <= x + 41) {
		return true;
	}

	return false;
}

bool StudentWorld::lineOfSightToIceMan(Actor* a, GraphObject::Direction& dirToPlayer) {
	int iceManX = m_iceman->getX();
	int iceManY = m_iceman->getY();
	int actX = a->getX();
	int actY = a->getY();

	bool clearLine = true;

	// check vertical line of sight
	if (iceManX == actX) {
		if (iceManY > actY) { // iceman above
			for (int i = actY + 1; i < iceManY; i++) {
				if (!canMoveTo(actX, i, GraphObject::Direction::up)) {
					clearLine = false;
					return false;
				}
				if (clearLine) {
					dirToPlayer = GraphObject::Direction::up;
					return true;
				}
			}
		}
		else if (iceManY < actY) { // iceman below
			for (int i = actY - 1; i > iceManY; i--) {
				if (!canMoveTo(actX, i, GraphObject::Direction::down)) {
					clearLine = false;
					return false;
				}
				if (clearLine) {
					dirToPlayer = GraphObject::Direction::down;
					return true;
				}
			}
		}
	}

	// check horizontal line of sight
	if (iceManY == actY) {
		if (iceManX > actX) { // iceman to the right
			for (int i = actX + 1; i < iceManX; i++) {
				if (!canMoveTo(i, actY, GraphObject::Direction::right)) {
					clearLine = false;
					return false;
				}
				if (clearLine) {
					dirToPlayer = GraphObject::Direction::right;
					return true;
				}
			}
		}
		else if (iceManX < actX) { // iceman to the left
			for (int i = actX - 1; i > iceManX; i--) {
				if (!canMoveTo(i, actY, GraphObject::Direction::left)) {
					clearLine = false;
					return false;
				}
				if (clearLine) {
					dirToPlayer = GraphObject::Direction::left;
					return true;
				}
			}
		}
	}

	return false;
}


bool StudentWorld::findShortestPath(int startX, int startY, int endX, int endY) {
	queue<Vertex> path;
	int curSteps;
	bool reachedDest = false;

	// initialize grid with -1, marks as univisited
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			shortestPath[i][j] = -1;
		}
	}

	// initialize starting position
	path.push(Vertex(startX, startY));
	shortestPath[startX][startY] = 0;

	while (!path.empty()) {
		// store guess coordinate
		int x = path.front().m_x;
		int y = path.front().m_y;
		path.pop();

		if (x == endX && y == endY) { // found destination
			reachedDest = true;
			return reachedDest;
		}

		curSteps = shortestPath[x][y] + 1;

		// check right
		if (shortestPath[x + 1][y] < 0 && canMoveTo(x, y, Actor::Direction::right)) {
			path.push(Vertex(x + 1, y));
			shortestPath[x + 1][y] = curSteps;
		}

		// check left 
		if (shortestPath[x - 1][y] < 0 && canMoveTo(x, y, Actor::Direction::left)) {
			path.push(Vertex(x - 1, y));
			shortestPath[x - 1][y] = curSteps;
		}

		// check up
		if (shortestPath[x][y + 1] < 0 && canMoveTo(x, y, Actor::Direction::up)) {
			path.push(Vertex(x, y + 1));
			shortestPath[x][y + 1] = curSteps;
		}

		if (shortestPath[x][y - 1] < 0 && canMoveTo(x, y, Actor::Direction::down)) {
			path.push(Vertex(x, y - 1));
			shortestPath[x][y - 1] = curSteps;
		}
	}

	return false;
}


GraphObject::Direction StudentWorld::dirToShortestPath(int startX, int startY, int x, int y) {
	GraphObject::Direction shortestDir = GraphObject::Direction::none;

	// temporary smallest step
	int shortest = 999999;

	if (shortestPath[x][y + 1] < shortest && shortestPath[x][y + 1] != -1) {
		shortest = shortestPath[x][y + 1];
		shortestDir = GraphObject::Direction::up;
	}
	else if (shortestPath[x][y - 1] < shortest && shortestPath[x][y - 1] != -1) {
		shortest = shortestPath[x][y - 1];
		shortestDir = GraphObject::Direction::down;
	}
	else if (shortestPath[x + 1][y] < shortest && shortestPath[x + 1][y] != -1) {
		shortest = shortestPath[x + 1][y];
		shortestDir = GraphObject::Direction::right;
	}
	else if (shortestPath[x - 1][y] < shortest && shortestPath[x - 1][y] != -1) {
		shortest = shortestPath[x - 1][y];
		shortestDir = GraphObject::Direction::left;
	}
	else {
		shortestDir = GraphObject::Direction::none;
	}
	return shortestDir;
}


