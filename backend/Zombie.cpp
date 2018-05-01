#include "gm.h"
#include "entities.h"
#include "enttypes.h"
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

double Zombie::normalizeAngleDiff(double phi, double theta) {
	while (phi - theta > M_PI) phi -= 2*M_PI;
	while (phi - theta < -M_PI) phi += 2*M_PI;

	return phi-theta;
}

Zombie::Zombie() : Entity(){
	typeID = ents::Zombie;
	radius = 16;
	speed = 2;
	turnSpeed = 0.27925;
}

void Zombie::act() {

	vector<Entity*> players = (*gm).getEntities(ents::Player);
	if (!players.empty()) {
		Entity player = *(players.at(0));

		if (turnSpeed != 0) {
			double theta = atan2(player.getY()-getY(), player.getX()-getX());

			theta = normalizeAngleDiff(theta,getRotation());

			if (theta != 0 && abs(theta) <= turnSpeed) rotate(theta);
			else if (theta != 0) rotate(turnSpeed * (theta/abs(theta)));
		}

		move(speed);
	}

}