#include <iostream>
#include "entities.h"

Bullet::Bullet() : Entity() {
	typeID = ents::Bullet;
	radius = 2;
	speed = 8;
	turnSpeed = 0;
}

void Bullet::act() {
	cout << "		Bullet acting" << endl;
	move();
	cout << "		Bullet moved" << endl;

	if (isAtEdge()) {
		damage();
		cout << "		Bullet was at edge" << endl;
	}
	cout << "	Bullet acted" << endl;
}



