#include <iostream>
#include "entities.h"

Bullet::Bullet(GameMaster* m) : Entity(m) {
	typeID = ents::Bullet;
	radius = 2;
	speed = 8;
	turnSpeed = 0;
}

void Bullet::act() {
	if (gm == 0 || gm == nullptr) return;
	move();
	if (isAtEdge()) {
		damage();
	}
}



