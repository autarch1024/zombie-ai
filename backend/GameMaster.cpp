#include "entities.h"
#include "neuralnet.h"
#include "gm.h"
#include <time.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
using std::vector;

double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}


//Spawns a zombie at a random spot on the world's edge
void GameMaster::spawnZombie() {
	int side = rand() % 4;

	double x = 0;
	double y = 0;
	double rot = 0;

	//Determines which side to spawn the zombie on
	switch(side) {
	case 0:
		x = -width/2;
		y = fRand(-height/2, height/2);
		rot = 0;
		break;
	case 1:
		x = width/2;
		y = fRand(-height/2, height/2);
		break;
	case 2:
		x = fRand(width/2, width/2);
		y = -height/2;
		break;
	case 3:
		x = fRand(-width/2, width/2);
		y = height/2;
		break;
	}

	//Spawns the zombie at x,y
	Zombie* z = new Zombie(this);
	addEntity(z);
	(*z).setLocation(x,y);
	(*z).setRotation(rot);
}

//Kill any entities that need to die
void GameMaster::killEntities() {
	vector<Entity*> toDamage = *(new vector<Entity*>());

	vector<Entity*> zombies = getEntities(ents::Zombie);
	vector<Entity*> bullets = getEntities(ents::Bullet);
	vector<Entity*> players = getEntities(ents::Player);

	//Check for collisions between bullets and zombies
	for (unsigned int b = 0; b < bullets.size(); b++) {
		Entity* bullet = bullets.at(b);

		for (unsigned int z = 0; z < zombies.size(); z++) {
			Entity* zombie = zombies.at(z);

			if ((*zombie).intersects(*bullet)) {
				toDamage.push_back(zombie);
				toDamage.push_back(bullet);
				addToScore(1);
			}
		}
	}

	//Check for collisions between players and zombies
	for (unsigned int z = 0; z < zombies.size(); z++) {
		Entity* zombie = zombies.at(z);

		for (unsigned int p = 0; p < players.size(); p++) {
			Entity* player = players.at(p);

			if ((*player).intersects(*zombie)) {
				toDamage.push_back(player);
			}
		}
	}


	//Damage the entities
	for (unsigned int i = 0; i < toDamage.size(); i++) {
		Entity* e = toDamage.at(i);
		(*e).damage();
	}

}

GameMaster::GameMaster(NeuralNet brain) {

	score = 0;
	zombieCooldown = 20;
	zombieTimer = zombieCooldown;
	scoreLandmark = 20;

	objects = *(new vector<Entity*>());

	Entity* p = new Player(this, brain);
	addEntity(p);
}

//Delete all the objects
GameMaster::~GameMaster() {
	while (!objects.empty()) {
		removeEntity( objects[0] );
	}
}

//Runs the simulation, then returns (score+1) * time
int GameMaster::run(long int sanityCheck, long int seed) {

	long int sc = sanityCheck;
	srand(seed);

	while ((sanityCheck > 0 && getEntities(ents::Player).size() > 0) || sanityCheck == sc) {

		//Add any pending objects to be added
		int numPend = 0;
		while (!pending.empty()) {
			objects.push_back(pending.at(pending.size()-1));
			pending.pop_back();
			numPend++;
		}

		//Run each entity's act() function
		for (unsigned int i = 0; i < objects.size(); i++) {
			(*objects[i]).act();
		}

		//If it is time to spawn a zombie, do so!
		if (zombieTimer >= zombieCooldown) {
			spawnZombie();
			zombieTimer = -1;
		}
		zombieTimer++;

		//Remove any entities that need to be killed
		killEntities();

		sanityCheck--;
	}
	//cout << "score=" << score << " time=" << sc-sanityCheck << " totalValue=" << (score + 1) * (sc - sanityCheck) << endl;
	return (score + 1) * (sc - sanityCheck);
}

//Runs the simulation and prints the game state every click to the given file
void GameMaster::run(string fileName, long int sanityCheck, long int seed) {

	ofstream file;
	file.open(fileName);

	zombieCooldown = 45;
	zombieTimer = 0;
	scoreLandmark = 10;

	long int sc = sanityCheck;
	srand(seed);

	while ((sanityCheck > 0 && getEntities(ents::Player).size() > 0) || sanityCheck == sc) {

		//Add any pending objects to be added
		int numPend = 0;
		while (!pending.empty()) {
			objects.push_back(pending.at(pending.size()-1));
			pending.pop_back();
			numPend++;
		}

		//Run each entity's act() function
		for (unsigned int i = 0; i < objects.size(); i++) {
			(*objects[i]).act();
		}

		//If it is time to spawn a zombie, do so!
		if (zombieTimer >= zombieCooldown) {
			spawnZombie();
			zombieTimer = -1;
		}
		zombieTimer++;

		//Remove any entities that need to be killed
		killEntities();

		file << objects.size() << " ";
		for (Entity* e : objects) {
			file << (*e).getType() << " ";
			file << (*e).getX() << " ";
			file << (*e).getY() << " ";
			file << (*e).getRotation() << " ";
		}
		file << endl;

		sanityCheck--;
	}
}

//Gets all objects currently in the world regardless of type as a vector
vector<Entity*> GameMaster::getEntities() {
	return objects;
}

//Gets all objects currently in the world of a specific type as a vector
vector<Entity*> GameMaster::getEntities(ents::EntityType type) {
	vector<Entity*> r = *(new vector<Entity*>());

	for (unsigned int i = 0; i < objects.size(); i++) {
		Entity* e = objects.at(i);
		if ((*e).isType(type)) r.push_back(e);
	}

	return r;
}

//Removes the specified entity from the world
void GameMaster::removeEntity(Entity* e) {
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects[i] == e) {

			objects.erase(objects.begin()+i);
			delete e;

			break;
		}
	}

}

//Adds the given entity to the game world at the origin
void GameMaster::addEntity(Entity* e) {
	for (unsigned int i = 0; i < objects.size(); i++) {
		Entity* pointer = objects[i];
		if (pointer == e) return;
	}

	for (unsigned int i = 0; i < pending.size(); i++) {
		Entity* pointer = pending.at(i);
		if (pointer == e) return;
	}

	pending.push_back(e);
}

//Adds the given entity to the game world at the given coordinates
void GameMaster::addEntity(Entity* e, double x, double y) {

	addEntity(e);
	(*e).setLocation(x, y);

}

//Adds i to the score.
void GameMaster::addToScore(int i) {
	score += i;
	if (score % scoreLandmark == 0) zombieCooldown--;
}

double GameMaster::getWorldWidth() {
	return width;
}

double GameMaster::getWorldHeight() {
	return height;
}
