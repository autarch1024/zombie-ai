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
using std::vector;

//Spawns a zombie at a random spot on the world's edge
void GameMaster::spawnZombie() {
	int side = rand() % 4;

	double x = 0;
	double y = 0;
	double rot = 0;

	//Determines which side to spawn the zombie on
	if (side == 0) {
		x = 0;
		y = fmod(rand(), height);
	} else if (side == 1) {
		x = width;
		y = fmod(rand(), height);
		rot = M_PI;
	} else if (side == 2) {
		y = 0;
		x = fmod(rand(), width);
		rot = M_PI/2;
	} else {
		y = height;
		x = fmod(rand(), width);
		rot = -M_PI/2;
	}

	//Spawns the zombie at x,y
	Zombie* z = new Zombie();
	addEntity(z,x,y);
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
	time = 0;
	zombieCooldown = 20;
	zombieTimer = 15;
	scoreLandmark = 20;

	objects = *(new vector<Entity*>());

	Entity* p = new Player(brain);
	(*p).setGM(this);
	addEntity(p, width/2, height/2);
}

//Delete all the objects
GameMaster::~GameMaster() {
	for (Entity* e : objects) {
		removeEntity( e );
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

	return (score + 1) * (sc - sanityCheck);
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
	cout << "Removing entity " << e;
	for (unsigned int i = 0; i < objects.size(); i++) {
		if (objects[i] == e) {

			objects.erase(objects.begin()+i);
			delete e;

			break;
		}
	}

}

//Adds the given entity to the game world at the given coordinates
void GameMaster::addEntity(Entity* e, double x, double y) {
	for (unsigned int i = 0; i < objects.size(); i++) {
		Entity* pointer = objects[i];
		if (pointer == e) return;
	}

	for (unsigned int i = 0; i < pending.size(); i++) {
			Entity* pointer = pending.at(i);
			if (pointer == e) return;
		}

	pending.push_back(e);
	(*e).setGM(this);
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
