#include "throwProjectile.h"
#include <math.h>

throwProjectile::throwProjectile(int ind, int _damage,
		sf::Vector2f frecPosition, weak_ptr<Frosh> froshToFireAt) : index(ind),
		damage(_damage), projectilePosition(frecPosition), froshTarget(froshToFireAt) 
{
	speed = 8.0f; //projectile speed is currently set at a static value of 8. 
	if (auto frosh = froshToFireAt.lock()) {
		if (frosh->getPixelSpeed() > speed - 3.5) {
			speed = frosh->getPixelSpeed() + 3.5;
		}
	}
	projectileShape = sf::RectangleShape(sf::Vector2f(30.0f, 30.0f));
	projectileShape.setSize(sf::Vector2f(30.0f, 30.0f)); //30 will be the size of projectile
	projectileShape.setPosition(frecPosition);
	if (!projectileTexture.loadFromFile(throwProjImage)) {
		std::cerr << "Error finding image\n";
	} else {
		projectileTexture.loadFromFile(throwProjImage);
		projectileShape.setTexture(&projectileTexture);
	}
	projectileCenter = sf::Vector2f(frecPosition.x + 30.0f / 2,
			frecPosition.y + 30.0f / 2);
}
throwProjectile::~throwProjectile() {}

void throwProjectile::drawProjectile(sf::RenderWindow* _window) {
	_window->draw(projectileShape);
}

sf::Vector2f throwProjectile::normalize(sf::Vector2f v) {
	float len = float(sqrt((v.x * v.x) + (v.y * v.y)));
	if (len != 0) {
		v.x = v.x / len;
		v.y = v.y / len;
	}
	return v;
}

//draws the projectile to the window, and updates the positional coordinates
void throwProjectile::moveObjectTowardsFrosh(sf::Vector2f froshPos) {
	sf::Vector2f movePos = normalize(froshPos - projectilePosition);
	projectileShape.move(movePos * getSpeed()); //using the x,y positions, moves at a percentage towards the frosh object
	projectilePosition = projectilePosition + movePos * getSpeed();
	projectileCenter = projectileCenter + movePos * getSpeed();
}

//returns distance between two points, used in projectileFroshCollision
float throwProjectile::DistanceFromFrosh(sf::Vector2f frosh) {
	//use abs to prevent a negative distance
	float x = abs(
			(frosh.x - projectilePosition.x)
					* (frosh.x - projectilePosition.x));
	float y = abs(
			(frosh.y - projectilePosition.y)
					* (frosh.y - projectilePosition.y));
	return (int) (powf(x + y, 0.5)); //square root ( x^2+y^2 )
}
bool throwProjectile::projectileFroshCollision(sf::Vector2f frosh) { // simple collision detection between two circles
	return DistanceFromFrosh(frosh) < 20;
}

//Accessors:
weak_ptr<Frosh> throwProjectile::getFroshTarget() {
	return froshTarget;
}
sf::Vector2f throwProjectile::getThrowProjectilePosition() {
	return projectilePosition;
}
float throwProjectile::getSpeed() {
	return speed;
}
int throwProjectile::getDamage() {
	return damage;
}
sf::Vector2f throwProjectile::getProjectileCenter() {
	return projectileCenter;
}
