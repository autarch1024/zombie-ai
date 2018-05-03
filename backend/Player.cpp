/*
 * The player character
 */
#include "entities.h"
#include "enttypes.h"
#include "neuralnet.h"
#include "gm.h"
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

double Player::normalizeAngleDiff(double phi, double theta) {
	while (phi - theta > M_PI) phi -= 2*M_PI;
	while (phi - theta < -M_PI) phi += 2*M_PI;

	return phi-theta;
}

//Returns the data from the sensors as a vector
vector<double> Player::getSensorData(ents::EntityType et) {
	vector<double> v = *(new vector<double>());

	vector<Entity*> zombies = (*gm).getEntities(ents::Zombie);

	for (double i = 0; i < 2*M_PI; i += 2*M_PI/(sensorNum)) {
		double num = 0;
		for (unsigned int j = 0; j < zombies.size(); j++) {
			Entity* z = zombies.at(j);

			double distance = pow((*z).getY()-getY(),2) + pow((*z).getX()-getX(),2);
			distance = sqrt(distance);

			double phi = atan2((*z).getY()-getY(), (*z).getX()-getX());
			phi = normalizeAngleDiff(phi, i);
			num += 1.0/(pow(phi,2)*distance);
		}
		v.push_back(num);
	}

	return v;
}

//Prepares the input vector for the neural network
vector<double> Player::processNNInput() {

	//Start off with the sensor data
	vector<double> v = getSensorData(ents::Zombie);

	//Next is whether the gun is ready to fire or not
	if (gunTimer >= gunTimer) v.push_back(1.0);
	else v.push_back(0.0);

	//Maybe add wall sensors later?

	return v;
}

//Interperets the neural networks output
void Player::processNNOutput(vector<double> output) {
	move(output.at(0), output.at(1));
	rotate(output.at(2));

	if (output.at(3) > 1) {
		fire();
	}

}

//Shoots the gun
void Player::fire() {
	if (gunTimer < gunCooldown) return;

	gunTimer = -1;
	Bullet* b = new Bullet(gm);
	(*b).setLocation(getX(), getY());
	(*b).setRotation(getRotation());
	(*getGM()).addEntity(b, getX(), getY());
}


Player::Player(GameMaster* m, NeuralNet nn) : Entity(m) {
	brain = nn;
	typeID = ents::Player;
	gunTimer = 0;
	radius = 16;
	speed = 5;
	turnSpeed = 0.4;
}

void Player::act() {
	if (gm == 0 || gm == nullptr) return;

	processNNOutput(brain.getOutput(processNNInput()));
	gunTimer++;
}


