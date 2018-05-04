/*
 * Base class for game objects
 */
#include <cmath>
#include "gm.h"
#include "entities.h"
#include <iostream>

Entity::Entity(GameMaster* m) : x(0), y(0), rotation(0), hp(0) {
	gm = m;
	(*gm).addEntity(this, 0, 0);

	typeID = ents::Null;
	turnSpeed = 0;
	speed = 0;
	radius = 0;
}

Entity::~Entity() {}

void Entity::damage() {
	(*getGM()).removeEntity(this);
}

//Pretty self explanatory functions
double Entity::getX() {
	return x;
}


double Entity::getY() {
	return y;
}


double Entity::getRotation() {
	return rotation;
}

//Gets the hit radius
double Entity::getHitSphere() {
	return radius;
}

//Move forward at top speed
void Entity::move() {
	move(speed);
}

//Move forward at (mag%speed)
void Entity::move(double mag) {
	move(mag, 0);
}

//Move in the given direction at (mag%speed)
void Entity::move(double mag, double theta) {
	if (mag == 0 || speed == 0) return;
	if (mag < 0) mag = -mag;
	if (mag > speed) mag = speed;

	setLocation(getX() + mag * cos(rotation+theta), getY() + mag * sin(rotation+theta));
}

//Rotate d radians counterclockwise
void Entity::rotate(double d) {

	if (d == 0 || turnSpeed == 0) return;
	if (d > 2*M_PI || d < -2*M_PI) d = fmod(d, 2*M_PI);
	if (d >  turnSpeed) d = turnSpeed;
	if (d < -turnSpeed) d = -turnSpeed;

	rotation += d;
	rotation = fmod(rotation, 2*M_PI);
}

ents::EntityType Entity::getType() {
	return typeID;
}

//Self explanatory
void Entity::setLocation(double a, double b) {
	x = a;
	y = b;

	if (x < -(*gm).getWorldWidth()/2) x = -(*gm).getWorldWidth()/2;
	else if (x > (*gm).getWorldWidth()/2) x = (*gm).getWorldWidth()/2;

	if (y < -(*gm).getWorldHeight()/2) y = -(*gm).getWorldHeight()/2;
		else if (y > (*gm).getWorldHeight()/2) y = (*gm).getWorldHeight()/2;
}

//Self explanatory
void Entity::setRotation(double r) {
	rotation = r;
}

//Sets this object's GameMaster
void Entity::setGM(GameMaster* m) {
	gm = m;
}

//Returns the game master
GameMaster* Entity::getGM() {
	return gm;
}

//Returns whether the given string is the name of this class
bool Entity::isType(ents::EntityType type) {
	if (type == typeID) return true;
	return false;
}

bool Entity::intersects(Entity e) {
	double distance = pow(e.getX()-x,2) + pow(e.getY()-y,2);
	double testRadius = getHitSphere() + e.getHitSphere();
	return distance < testRadius*testRadius;
}

bool Entity::intersects(Entity* e) {
	return intersects(*e);
}

bool Entity::isAtEdge() {
	if (x == 0 || y == 0) return true;
	if (x == (*gm).getWorldWidth()) return true;
	if (y == (*gm).getWorldHeight()) return true;
	return false;
}

//Called on every step
void Entity::act() {}
