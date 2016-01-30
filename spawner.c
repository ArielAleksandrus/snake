#include "spawner.h"
#include <ncurses.h>
#include <stdio.h>
void spawnSnake(Level* l, Snake* s){
	mapSnake(l->map, l->height, l->width, s->bodyPositions);
}
void spawnFood(Level* l){
	if(l->curFoodQty > 0)
		return;

	int randX = rand() % l->width;
	int randY = rand() % l->height;

	while(l->map[randX][randY] != BLANK){
		randX = rand() % l->width;
		randY = rand() % l->height;
	}
	l->map[randX][randY] = FOOD;
	l->curFoodQty++;
}
void spawnBonus(Level* l, SpawnData sd){
	if(l->bonuses->size >= sd.maxItemsAtOnce)
		return;
	
	Bonus* b = malloc(sizeof(Bonus));
	
	int randomRarity = rand() % 7;
	if(randomRarity != 0)
		randomRarity = rand() % randomRarity;
	
	int randomGenerosityFactor = rand() % sd.generosity;
	
	randomRarity += randomGenerosityFactor;
	if(randomRarity > 6)
		randomRarity = 6;
	
	*b = *(getBonus(sd.whatCanBeSpawned, randomRarity));
	b->position.x = rand() % l->width;
	b->position.y = rand() % l->height;
	
	while(l->map[b->position.x][b->position.y] != BLANK){
		b->position.x = rand() % l->width;
		b->position.y = rand() % l->height;
	}
	
	l->map[b->position.x][b->position.y] = BONUS;
	
	lappend(l->bonuses, b);
}
void spawnFoodThreadFunction(SpawnFoodData* sfd){
	srand(time(NULL));
	struct timespec spec, spec2;
	
	while(1){

		spawnFood(sfd->level);

		spec.tv_sec = 0;
		spec.tv_nsec = FOOD_THREAD_RATE * 1.0e6;
		nanosleep(&spec, &spec2);
	}
}
void spawnBonusThreadFunction(SpawnBonusData* sbd){
	srand(time(NULL));
	struct timespec spec, spec2;
	while(1){
		
		spawnBonus(sbd->level, sbd->spawnData);
				
		spec = toTimespec(BONUS_THREAD_RATE * sbd->spawnData.frequency);
		nanosleep(&spec, &spec2);
	}
}

Bonus* getBonus(List* whatCanBeSpawned, int rarity){
	Bonus* res = NULL;
	Bonus* aux;
	List* possibilities = newList();
	int i;
	int maxLowestRarity = 0;
	int minHigherRarity = 6;
	for(i = 0; i < whatCanBeSpawned->size; i++){
		aux = lAt(whatCanBeSpawned, i);
		if(aux->rarity > rarity){
			if(aux->rarity < minHigherRarity)
				minHigherRarity = aux->rarity;
			continue;
		}
		
		if(maxLowestRarity < aux->rarity){
			maxLowestRarity = aux->rarity;
		}
	}
	for(i = 0; i < whatCanBeSpawned->size; i++){
		aux = lAt(whatCanBeSpawned, i);
		if(aux->rarity == maxLowestRarity)
			lappend(possibilities, aux);
	}
	if(possibilities->size == 0){
		for(i = 0; i < whatCanBeSpawned->size; i++){
			aux = lAt(whatCanBeSpawned, i);
			if(aux->rarity == minHigherRarity)
				lappend(possibilities, aux);
		}
	}
	
	if(possibilities->size == 0)
		return NULL;
	
	int randBonus = rand() % possibilities->size;
	
	res = lAt(possibilities, randBonus);	
	lfree(possibilities);
	
	return res;
}

int hasBonus(List* bonuses, int type){
	int i;
	Bonus* b;
	for(i = 0; i < bonuses->size; i++){
		b = lAt(bonuses, i);
		if(b->type == type)
			return 1;
	}
	return 0;
}
void setBonusEffect(Snake* s, Player* p, Bonus* b, int apply){
	switch(b->type){
		case EXTRA_POINTS:{
			if(apply)
				p->score.extraModifier += 5;
			else
				p->score.extraModifier -= 5;
			break;
		}
		case EXTRA_SIZE:{
			
			break;
		}
		case DOUBLE_POINTS:{
			if(apply)
				p->score.multiplier *= 2;
			else
				p->score.multiplier /= 2;
			break;
		}
		case REDUCED_SPEED:{
			if(apply)
				s->speed *= 2;
			else
				s->speed /= 2;
			break;
		}
		case INVINCIBLE:{
			if(apply)
				s->invincible++;
			else
				s->invincible--;
			break;
		}
		case EXTRA_LIFE:{
			
			break;
		}
		case RESET_SIZE:{
			
			break;
		}
	}
}