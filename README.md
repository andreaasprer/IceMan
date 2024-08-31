# IceMan
IceMan is an object-oriented program that utilizes inheritance, data structures, and algorithms to create a functioning 2D arcade-style game.

Project for CS 30: Advanced Programming in C++ taught by Professor Solomon Russell. Please refrain from copying my code for homework assignments.
<p align="center">
  <img src="https://github.com/user-attachments/assets/dcdfaab8-ce83-4138-bd31-1e5027786185" width="500" />
</p>

## Game Details
In IceMan, a player controls an Iceman character to dig tunnels in an oil field to search and collect oil barrels. The player starts the game with three lives and must continue playing until all lives are exhausted. There are multiple levels, with each having its own unique oil field configuration and the Iceman must find all the hidden oil barrels to move on to the next level. 

At the start of each level, the Iceman is equipped with full health, water for their squirt gun, and a single sonar charge. Each level has additional game objects such as boulders, additional sonar charges, water pools, and hidden gold nuggets. While traversing the field, a band of Regular and Hardcore Protesters are roaming around to stop the Iceman. 

#### Game Play
* Difficulty increases with each level. The number of Protesters, boulders, and oil barrels to collect increases.
* Game play is divided into ticks. There are dozens of ticks per second to provide smooth animation and gameplay. Each tick allows the Iceman and other actors to perform actions.

#### Iceman's Actions
* **Movement**: Player can move Iceman up, down, left, and right. Iceman digs through ice in the direction of movement. Boulders blocks Iceman’s movement.
* **Collecting**: Iceman must collect all oil barrels in the field to complete the level. He can also collect different items to help him and earn points. 
* **Squirt Gun**: Used to stun and irritate protesters
* **Dropping Gold Nuggets**: Used to bribe protesters
* **Sonar**: Used to reveal hidden items within a radius

#### Enemies
* **Regular Protesters** - less dedicated
  * Can be bribed with gold nuggets. When bribed they move back to their spawn point and reset
  * Takes fewer hits by the squirt gun to lose all health
* **Hardcore Protesters** - more dedicated
  * Cannot be bribed with gold nuggets. When it encounters gold, it inspects it for a few ticks then continues.
  * Takes more hits by the squirt gun to lose all health
  * Can track the Iceman’s location if close enough

 #### Hazards: 
 * **Boulders**: Can be dropped on Protesters to fully irritate them by digging the ice beneath them, but they can also bonk the Iceman if not careful.
* **Protesters**: Shouts from the protesters irritate the Iceman and reduce his health. Losing all health results in losing a life and restarting the level.

#### Game Elements:
* **Oil Barrels**: Hidden throughout the oil field. Iceman must collect all of them to complete the level. 
* **Gold Nuggets**: Hidden throughout the oil field. Used to bribe Regular Protesters or distract Hardcore Protesters
* **Water Pools**: Spawn randomly in the tunnels but disappear quickly. Used to refill his squirt gun.
* **Sonar Kits**: Occasionally dropped in the upper-left corner by the Iceman’s friends. Used to reveal hidden items in the field.

#### Controls
* Move - WASD or Arrow Keys
* Restart Level - ESC
* Drop Gold - TAB
* Sonar - Z
* Shoot - Space

## Assignment Overview
For this assignment, we were tasked to edit 4 files: StudentWorld.h, StudentWorld.cpp, Actor.h, and Actor.cpp. 
### Key Features
#### Actor Inheritance
![ClassDiagram1](https://github.com/user-attachments/assets/646e32f8-f268-48cd-9829-83cd11c9611e)
All live actors in the game are managed using a list of pointers to the base abstract Actor class. The Actor class includes pure virtual functions like doSomething(), which ensure code reusability and polymorphism. This design allows the game engine to treat all actors uniformly, regardless of their specific derived type. During each game tick, the list is iterated through, and the doSomething() method is called on each actor.  

## How to Run
#### Windows
1. In terminal: `git clone https://github.com/andreaasprer/IceMan.git`
2. Open in Visual Studio and run the project.

