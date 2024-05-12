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