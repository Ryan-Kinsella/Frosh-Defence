#include "FrecAndFroshController.h"

using std::weak_ptr; //used for a projectile to temporarily own ( using .lock() ) a frosh object shared_ptr
using std::shared_ptr;

FrecAndFroshController::FrecAndFroshController(sf::RenderWindow* _window,
		GameState* _gameState, FroshController* _froshController,
		vector<Frec*>* _allFrecs, vector<shared_ptr<Frosh>>* _allFrosh) :
		window(_window), gameState(_gameState), froshController(
				_froshController), allThrowFrecs(_allFrecs), allFrosh(_allFrosh) {
}
FrecAndFroshController::~FrecAndFroshController() {}

/*___________________________________________OBJECT BEING THROWN_________________________________________________*/
void FrecAndFroshController::addThrowObjectToList(int index, int damage,
		sf::Vector2f frecPosition, weak_ptr<Frosh> froshPtr) {
	allThrowObjects.push_back(
			new throwProjectile(index, damage, frecPosition, froshPtr));
}

void FrecAndFroshController::deleteProjectile(throwProjectile* projectile) {
	//This is an acceptable computational cost due to rarity of action.
	//O(N) for each deletion
	for (int i = 0, size = allThrowObjects.size(); i < size; i++) {
		if ((allThrowObjects)[i] == projectile) {
			projectile = nullptr;
			allThrowObjects.erase(allThrowObjects.begin() + i);
			break;
		}
	}
}

void FrecAndFroshController::deleteThrowObjectAtIndex(int index) {
	delete allThrowObjects[index];
	allThrowObjects[index] = nullptr; 
	allThrowObjects.erase(allThrowObjects.begin() + index); //helps future iterations
}
void FrecAndFroshController::drawAllThrowObjectsOnGrid(sf::RenderWindow* _window) {	
	//draws each projectile onto the grid
	for (auto& projectile : allThrowObjects) {
		projectile->drawProjectile(_window);
	}
}
/*__________________________________________OBJECT BEING THROWN_________________________________________________*/

/*___________________________________________Attack Logic_______________________________________________________*/

//if missile to frosh collision occurs, delete throwObject, else, move towards frosh. Else, delete projectiles that have no target
void FrecAndFroshController::updateProjectiles() {
	int i = 0;
	while(i<allThrowObjects.size()){ //for each projectile in the vector
		if (auto frosh = allThrowObjects[i]->getFroshTarget().lock()) { //if shared_ptr != nullptr
			if (allThrowObjects[i]->projectileFroshCollision(frosh->getCenterPosition())) { //if collision occurs
				//deal damage to frosh
				froshController->dealDamage(frosh,allThrowObjects[i]->getDamage());
				//delete throw object
				deleteProjectile(allThrowObjects[i]);
				i--; //deleteProjectile() removes the index at i, so re-adjust i such that no projectiles are skipped
			} else { //if the projectile assigned to a frosh hasn't hit the frosh yet, move towards frosh
				allThrowObjects[i]->moveObjectTowardsFrosh(frosh->getCenterPosition());
			}
		} 
		else {	//getFroshTarget().lock() fails thefore the frosh target has been deleted,
				//so delete the projectile assigned to the deleted frosh
			deleteProjectile(allThrowObjects[i]);
			i--; //deleteProjectile() removes the index at i, so re-adjust i such that no projectiles are skipped
		}
		i++;
	}
}

//FREC AND FROSH COLLISION
//Uses Pythagorean to detect a point collision with a circle
bool FrecAndFroshController::collisionDetected(Frec* frec,
		weak_ptr<Frosh> _frosh) {
	if (auto frosh = _frosh.lock()) {
		float distX = frosh->getCenterPosition().x
				- frec->getCenterPosition().x;
		float distY = frosh->getCenterPosition().y
				- frec->getCenterPosition().y;
		float distance = sqrt((distX * distX) + (distY * distY));
		if (distance <= frec->getRange())
			return true;
	}
	return false;
}

//ascocitate each frec with a frosh within range, each with a vector of pointers (eg. frec index 0 will fire at frosh index 0)
void FrecAndFroshController::updateFrecFroshRange() {
	//clear contents of allThrowFrecsInRangeOfFrosh
	if (allThrowFrecsInRangeOfFrosh.size() > 0)
		allThrowFrecsInRangeOfFrosh.erase(allThrowFrecsInRangeOfFrosh.begin(),
				allThrowFrecsInRangeOfFrosh.end());
	allFroshInRangeOfFrecs.erase(allFroshInRangeOfFrecs.begin(),
			allFroshInRangeOfFrecs.end());
	for (int i = 0, allFrecSize = allThrowFrecs->size(); i < allFrecSize; i++) { //for each frec object
		for (int j = 0, allFroshSize = allFrosh->size(); j < allFroshSize;j++) { //for each frosh object
			if (collisionDetected((*allThrowFrecs)[i], (*allFrosh)[j])) {	
				weak_ptr<Frosh> frosh = (*allFrosh)[j];
				allFroshInRangeOfFrecs.push_back(frosh);
				allThrowFrecsInRangeOfFrosh.push_back((*allThrowFrecs)[i]);
			}
		}
	}
}


//runs many times per second (around 30-60)
void FrecAndFroshController::update() { 
	//for each frec, decrease its cooldown by one tick, when at 0 it can fire
	for (int i = 0, frecSize = allThrowFrecs->size(); i < frecSize; i++) {
		if ((*allThrowFrecs)[i]->getCooldown() >= 1) {
			(*allThrowFrecs)[i]->decreaseCooldown();
		}
	}
	//add new frosh targets if they join the range of any frecs
	updateFrecFroshRange();
	//if the frec is off cooldown, create a throwProjectile object that is assigned to a frosh object
	for (int i = 0, availableFrosh = allThrowFrecsInRangeOfFrosh.size();
			i < availableFrosh; i++) { //frecs and frosh will have same index i
		//if the frec is not on cooldown fire a new projectile, else do nothing
		if (allThrowFrecsInRangeOfFrosh[i]->getCooldown() <= 0) { 
			allThrowFrecsInRangeOfFrosh[i]->resetCooldown();
			auto frosh = allFroshInRangeOfFrecs[i].lock();
			(allThrowFrecsInRangeOfFrosh)[i]->froshDirection(frosh->getPosition());
			(allThrowFrecsInRangeOfFrosh)[i]->Attack(); //sets directioon
			addThrowObjectToList(allThrowObjects.size() - 1,
					allThrowFrecsInRangeOfFrosh[i]->getDamage(),
					allThrowFrecsInRangeOfFrosh[i]->getPosition(),
					allFroshInRangeOfFrecs[i]); //append a throw object to the end of the vector
		}
	}
	//move each projectile towards its assigned frosh target, delete and deal damage if hit
	updateProjectiles();
}

void FrecAndFroshController::render() {
	drawAllThrowObjectsOnGrid(window);
}

/*___________________________________________Attack Logic_______________________________________________________*/