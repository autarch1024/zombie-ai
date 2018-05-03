/*
 * Includes the game master
 */

#ifndef GM_H_
#define GM_H_

class Entity;
#include "neuralnet.h"
#include "enttypes.h"
#include <vector>


class GameMaster {
	int score;
	int zombieCooldown;
	int zombieTimer;
	int scoreLandmark;
	const double width = 1000;
	const double height = 1000;

	std::vector<Entity*> objects;
	std::vector<Entity*> pending;

	private:
		void spawnZombie();
		void killEntities();

	public:
		GameMaster(NeuralNet nn);
		~GameMaster();

		int run(long int sanity, long int seed);
		std::vector<Entity*> getEntities();
		std::vector<Entity*> getEntities(ents::EntityType type);
		void removeEntity(Entity* e);
		void addEntity(Entity* e);
		void addEntity(Entity* e, double x, double y);
		void addToScore(int i);
		double getWorldWidth();
		double getWorldHeight();
};

#endif /* GM_H_ */
