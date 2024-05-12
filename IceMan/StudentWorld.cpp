#include "StudentWorld.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

void StudentWorld::clearIce(int x, int y) {
	// flag to see if ice was cleared
	bool cleared = false;

	if (y < 60) { // while iceman is below top limit of ice field
		// 4x4 box around iceman to clear
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


void StudentWorld::spawnBoulders(int boulderNum) {
	int currentNum = 0;
	bool canPlace = true;

	while (currentNum < boulderNum) {
		// randomize coordinates boulder
		int x = rand() % 61; // 0 - 60
		int y = 20 + (rand() % 37); // 20 - 56


		if (x > 26 && x < 34 && y > 3) { // check if in mine shaft
			canPlace = false;
		}
		else {
			// go through actor list and check if distance of boulder not too close to other actors
			for (Actor* actor : actorList) {
				if (!validEuclideanDistance(x, y, actor->getX(), actor->getY())) {
					canPlace = false;
					break;
				}
			}
		}

		if (canPlace) {
			actorList.push_back(new Boulder(this, x, y));
			currentNum++;
		}


		// TODO: clear out ice where boulder is


		canPlace = true;
	}
}


bool StudentWorld::validEuclideanDistance(int x1, int y1, int x2, int y2) {
	int distance = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
	if (distance < 6) { return false; }
	return true;
}


