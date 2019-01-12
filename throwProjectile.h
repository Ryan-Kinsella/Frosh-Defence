#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include "Frec.h"
#include "Frosh.h"
#include <vector>
#include <memory>
#include <string>

using std::string;
using std::weak_ptr;

class throwProjectile {
public:
	throwProjectile(int ind, int damage, sf::Vector2f frecPosition, weak_ptr<Frosh> froshToFireAt);
	~throwProjectile();
	void drawProjectile(sf::RenderWindow* _window); //using frosh position, heatseeks towards the frosh until collision occurs
	void moveObjectTowardsFrosh(sf::Vector2f froshPos); //draws the projectile to the window, and updates the positional coordinate private members
	float DistanceFromFrosh(sf::Vector2f frosh); //returns distance between two points, used in projectileFroshCollision
	bool projectileFroshCollision(sf::Vector2f frosh); //collision detection between two objects, using the distance function
	sf::Vector2f normalize(sf::Vector2f v); //gets a vector of length 1, to be multipled by a speed. Used in moveObjectTowardsFrosh

	//Accessors:
	sf::Vector2f getThrowProjectilePosition();
	float getSpeed();
	weak_ptr<Frosh> getFroshTarget();
	int getDamage();
	sf::Vector2f getProjectileCenter();

private:
	int index;
	int damage;
	sf::Texture projectileTexture;
	sf::RectangleShape projectileShape;
	sf::Vector2f projectilePosition;
	sf::Vector2f projectileCenter;
	float speed;
	string throwProjImage = "assets/jacket.png"; //using as a private member to enable future use of multiple jacket textures 
	weak_ptr<Frosh> froshTarget;
};
