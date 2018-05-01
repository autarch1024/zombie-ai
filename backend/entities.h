/*
 * Includes the game entities
 */
#ifndef ENTITIES_H_
#define ENTITIES_H_

#include "enttypes.h"
#include "neuralnet.h"
#include <vector>

class GameMaster;

class Entity {

	private:
		double x;
		double y;
		double rotation;
		double hp;
	protected:
		double radius;
		double speed;
		double turnSpeed;
		GameMaster* gm;
		ents::EntityType typeID;
	public:

		Entity();
		virtual ~Entity();

		double getX();
		double getY();
		double getRotation();
		double getHitSphere();
		ents::EntityType getType();
		void damage();
		GameMaster* getGM();
		void move();
		void move(double d);
		void move(double mag, double theta);
		void setLocation(double x, double y);
		void setRotation(double r);
		void setGM(GameMaster* m);
		void rotate(double theta);
		bool isType(ents::EntityType e);
		bool intersects(Entity e);
		bool intersects(Entity* e);
		bool isAtEdge();
		virtual void act();

};

class Zombie : public Entity {
	private:
		static double normalizeAngleDiff(double phi, double theta);

	public:
		Zombie();
		void act();
};

class Player : public Entity {
	NeuralNet brain;
	const static int sensorNum = 8;
	const static int gunCooldown = 10;
	int gunTimer;

	private:
		static double normalizeAngleDiff(double phi, double theta);
		std::vector<double> getSensorData(ents::EntityType et);
		std::vector<double> processNNInput();
		void processNNOutput(std::vector<double> output);
		void fire();

	public:
		Player(NeuralNet nn);
		void act();
};

class Bullet : public Entity {

	public:
		Bullet();
		void act();
};

#endif
