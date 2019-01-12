#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>
#include <math.h>
#include "GameState.h"
#include "throwProjectile.h"
#include "Frec.h"
#include "Frosh.h"
#include "FroshController.h"

using std::weak_ptr;
using std::shared_ptr;

class FrecAndFroshController {
public:
	FrecAndFroshController(sf::RenderWindow* window, GameState* gameState,
			FroshController* froshController, vector<Frec*>* allFrecs,
			vector<shared_ptr<Frosh>>* allFrosh);
	~FrecAndFroshController();
	//Throw/Projectile Objects
	void addThrowObjectToList(int index, int damage, sf::Vector2f frecPosition,
			weak_ptr<Frosh> froshPtr);
	void drawAllThrowObjectsOnGrid(sf::RenderWindow* theWindow);
	void deleteProjectile(throwProjectile* projectile);
	void deleteThrowObjectAtIndex(int index);
	void updateProjectiles();
	
	void updateFrecFroshRange();
	void update();
	void render();

	//used for testing
	void addFroshObjectToList(sf::Vector2f _position, sf::Vector2f _size,
		sf::Texture* _texture, int type, double modifier);
private:
	sf::RenderWindow* window;
	GameState* gameState;
	FroshController* froshController;
	vector<throwProjectile*> allThrowObjects;

	vector<Frec*>* allThrowFrecs;
	vector<Frec*> allThrowFrecsInRangeOfFrosh;

	//This is a reference to the true frosh vector. Duplication
	//should always be as a weak pointer, to allow for projectiles
	//to properly delete themselves once a target frosh is removed.
	vector<shared_ptr<Frosh>>* allFrosh;
	vector<weak_ptr<Frosh>> allFroshInRangeOfFrecs;

	bool collisionDetected(Frec*, weak_ptr<Frosh>);

};

